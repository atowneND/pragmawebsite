// readSerialPort.c
// #pragma

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "serialfun.h"
#include "usbpipe.h"

int main(){
    int verbose = 0;
    int settings = 1;

    serialReceiveInit(verbose, settings);

    char *mydata = malloc(sizeof(char)*MAXSIZE);
    char *outdata = getDataLinux(&mydata);
    if(outdata){
        int fd = open("foofile",O_WRONLY|O_APPEND|O_CREAT,0666);
        write(fd,"\n",sizeof("\n"));
    }

    return 0;
}
