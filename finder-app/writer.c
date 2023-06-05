#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

int main (int argc, char *argv[])
{
    openlog(NULL,LOG_PID|LOG_CONS,LOG_USER);
    if (argc != 3)
    {
        syslog(LOG_ERR, "Usage: %s <filePath> <stringToFillWith>\n", argv[0]);
        return 1;
    }
    else{
        int opendFileDes = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
        if (opendFileDes == -1){
            syslog(LOG_ERR, "Error %d : %s \n", errno, strerror(errno));
        }
        else{
            syslog(LOG_DEBUG, "Writing %s to %s\n", argv[2] , argv[1]); 
            char *writingBuf = argv[2];
            int writingBufSize = strnlen(writingBuf, __INT_MAX__);
            ssize_t writingReturn = writingBufSize;
            
            while (writingBufSize != 0 && ( writingReturn = write(opendFileDes,writingBuf,writingBufSize) )){
                if (writingReturn == -1){
                    if (errno == EINTR)
                        continue;
                    else{
                        syslog(LOG_ERR, "Error %d : %s \n", errno, strerror(errno));
                        //break;
                        return -1;
                    }
                }
                writingBufSize  -= writingReturn;
                writingBuf      += writingReturn;
            }
        }
        close (opendFileDes);
    }
    return 0;
}