CC=${CROSS_COMPILE}gcc
all: writer.c
	echo "using CC = ${CROSS_COMPILE}gcc to build"
	$(CC) -o writer writer.c


gcc:
	gcc -o writer writer.c

clean:
	rm -rf writer
