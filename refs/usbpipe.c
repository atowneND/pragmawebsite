#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "usbpipe.h"

char *getDataLinux(char **mydata){
	// initialize
    char *cat_command = "sudo cat /dev/ttyUSB0";

	// open pipe
	FILE *usbptr = popen(cat_command,"r");
    if (usbptr==NULL){printf("USB is NULL\n");}

    int ctr = 0;
    int numSamp = 100;

    // read from pipe
    printf("reading\n");
    int indata = fgetc(usbptr);
    printf("done reading\n");

    while (indata!=EOF){
        if (feof(usbptr)){ // while not at end of file
            printf("At EOF\n");
            break;
        } 
        
        // upon finding stop bit, take what is before stop bit
        // write to pipe
        writeToFile(indata);
        if (indata==STOP_DELIMITER){
            break;
        }
        else{
            indata = fgetc(usbptr);
            ctr++;
            if (ctr>numSamp){break;}
        }
    } 

    // close pipe
    int closeflag = pclose(usbptr);
    if (closeflag<0){printf("error on close\n");}
    
    return "z";
}

void writeToFile(int indata){
    // create file if DNE, open for writing and appending
    char *path = "foofile";
    int fd = open(path,O_WRONLY|O_APPEND|O_CREAT);
    // modify permissions to 666
    int foo = fchmod(fd,00666);
    if (foo<0){printf("error in file permissions: %s\n",strerror(errno));}

    // write to file
    write(fd,&indata,sizeof(indata));

    // close file
    close(fd);

    // DO LATER
    // put this function in getData* function rather than main
    // keep file open if not the first time, close only if done reading
}
