#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

#SIDDHANT   using set -e and set -u to exit the script if any command fails
#           or if any variable is used before it is set.
#echo "<DEBUG> Curren path : " && pwd && read -p "Press enter to continue.."

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
FINDER_APP_WD=$(realpath $(dirname $0))


#SIDDHANT check if the directory is passed or not
if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi
#SIDDHANT create the directory and cd into it
mkdir -p ${OUTDIR}
cd "$OUTDIR"

#SIDDHANT check if $OUTDIR has linux-stable directory or not to clone it from scratch
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi

if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # SAMY: Add your kernel build steps here
    if [ ! -f ${OUTDIR}/linux-stable/.config ]; then
        make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    fi
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    make -j6 ARCH=arm64 CROSS_COMPILE=${CROSS_COMPILE} all 
    #make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules  #Skipped as per requirments
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs

    #SAMY another way to perform the above steps

    #for item in mrproper defconfig all modules dtbs
    #do
    #   if [${item} == "all"]; then
    #      make -j6 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} ${item}
    #   else
    #      if [[ -f ${OUTDIR}/linux-stable/.config && {item} == "mrproper"]]; then
    #          continue
    #      fi
    #      make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} ${item}
    #   fi
    #done   
fi

echo "Adding the Image in outdir"
#SAMY adding the Image in outdir (home directory of the kernel)
cp "${OUTDIR}"/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# SAMY: Create necessary base directories
# SAMY root file system as per module videos and Mastering Embedded Linux Programming book
mkdir -p ${OUTDIR}/rootfs
if [ ! -d "${OUTDIR}/rootfs" ]; then
    echo "rootfs directory not created"
    exit 1
fi

for item in bin dev etc home lib lib64 proc sbin sys tmp usr var usr/bin usr/lib usr/sbin var/log
do   
    mkdir -p ${OUTDIR}/rootfs/${item}
done

cd "$OUTDIR"
echo "before busybox"
if [ ! -d "${OUTDIR}/busybox" ]
then
    git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # SAMY:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# SAMY: Make and install busybox
echo "before Make and install busybox"
pwd
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX="${OUTDIR}/rootfs" ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
echo "before Library dependencies"
cd ${OUTDIR}/rootfs
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"


# SAMY: Add library dependencies to rootfs
#SAMY : -fetch dependencies for busybox 2,3
#       -extract dependencies binary files' names 4,5
#       -find the dependencies in the gcc toolchain path 6,7
#       -copy the dependencies to the rootfs 8,9,10,...etc
cd ${OUTDIR}/rootfs
GCC_TOOL_CHAIN_PATH=$(${CROSS_COMPILE}gcc -print-sysroot)
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter" >> lib_dep_output.txt
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"   >> lib_dep_output.txt
grep -o '\S*so\S*' lib_dep_output.txt | sed 's/[][]//g' | sed 's/.*\///' > lib_dep_output2.txt
cat lib_dep_output2.txt > lib_dep_output.txt && rm lib_dep_output2.txt
xargs -a lib_dep_output.txt -I {} find $GCC_TOOL_CHAIN_PATH -name "{}" > lib_dep_output2.txt
for item in $(cat lib_dep_output2.txt)
do
    if   [[ $item =~ .*/lib/.* ]]; then        cp -v $item ./lib
    elif [[ $item =~ .*/lib64/.* ]]; then      cp -v $item ./lib64
    elif [[ $item =~ .*/etc/.* ]]; then        cp -v $item ./etc    
    elif [[ $item =~ .*/sbin/.* ]]; then       cp -v $item ./sbin
    elif [[ $item =~ .*/usr/bin/.* ]]; then    cp -v $item ./usr/bin
    elif [[ $item =~ .*/usr/sbin.* ]]; then    cp -v $item ./usr/sbin
    elif [[ $item =~ .*/usr/.* ]]; then        cp -v $item ./usr
    elif [[ $item =~ .*/var/.* ]]; then        cp -v $item ./var
    elif [[ $item =~ .*/usr/.* ]]; then        cp -v $item ./usr
    fi  
done
rm lib_dep_output.txt lib_dep_output2.txt
#OR
#cp ${GCC_TOOL_CHAIN_PATH}/lib/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib
#cp ${GCC_TOOL_CHAIN_PATH}/lib64/libm.so.6 ${OUTDIR}/rootfs/lib64
#cp ${GCC_TOOL_CHAIN_PATH}/lib64/libresolv.so.2 ${OUTDIR}/rootfs/lib64
#cp ${GCC_TOOL_CHAIN_PATH}/lib64/libc.so.6 ${OUTDIR}/rootfs/lib64


# SAMY: Make device nodes
echo "Make device nodes"
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/console c 5 1
# SAMY: Clean and build the writer utility
echo "Clean and build the writer utility"
cd ${FINDER_APP_WD}
make clean
make CROSS_COMPILE=${CROSS_COMPILE}
echo "finished Clean and build the writer utility"

# SAMY: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp -r conf/ ${OUTDIR}/rootfs/home
for item in finder.sh finder-test.sh autorun-qemu.sh writer
do
    cp -v ${item} ${OUTDIR}/rootfs/home
done

# SAMY: Chown the root directory
echo "Chown the root directory"
sudo chown -R root:root ${OUTDIR}/rootfs

# SAMY: Create initramfs.cpio.gz
cd ${OUTDIR}/rootfs
echo "Create initramfs.cpio.gz"
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ${OUTDIR}
echo "gzip initramfs.cpio"
gzip -f initramfs.cpio
