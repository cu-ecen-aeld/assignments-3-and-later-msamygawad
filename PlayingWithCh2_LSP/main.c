#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char *arg[])
{
    int r = open("file0.txt", O_RDONLY);
    ssize_t nr;
    #define BUFSIZE 100
    char word[BUFSIZE];
    nr = read(r, &word, sizeof (unsigned long));
    
    //print error
    if (nr == -1)
    {
        const int err = errno;
        printf("Error %d : %s\n",errno, strerror(err));
    }
    else
    {
        printf("File opened successfully\n");
        printf("nr = %ld\n", nr);
        printf("word = %s\n", word);
        close(r);
    }


    /*
    printf("r = %d\n", r);
    fp = fopen("file.txt", "r");
    if (fp == NULL)
    {
        const int err = errno;
        char x [100];
        int err_str = strerror_r(err, x, 100);
        printf("Error %d : %s\n",errno, x);
        fprintf(stderr, "Error %d : %s\n",errno, strerror(err));      
        
    }
    else
    {
        printf("File opened successfully\n");
        fclose(fp);
    }
    */
    return 0;
}