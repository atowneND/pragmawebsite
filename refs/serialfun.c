#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "serialfun.h"

#define BAUDRATE B9600

void serialReceiveInit(int verbose, int settings){
    isSudo(); // check for proper permissions
    char *pathname = getPath(); // get name of device
    int fdUSB = myOpen(pathname); // get file descriptor
    if(!isatty(fdUSB)){printf("not a terminal device: %i\n\t%s\n",fdUSB,strerror(errno));}
    
    // get current attributes
    struct termios config;
    int attrFlag = tcgetattr(fdUSB,&config);
    if (attrFlag<0){printf("serialReceiveInit: tcgetattr: %s\n",strerror(errno));}
    if (verbose){
        printf("Initial (default) attributes:\n");
        printTermistruct(config);
    }

    // set baud rate
    int BRFlag = setBaudRate(fdUSB,config);
    if (BRFlag<0){printf("Baud rate mismatch\n");}
    
    // initialize attributes
    config = initTerms(fdUSB,config,settings);

    // set attributes
    config = setTerms(fdUSB,config);
    if (verbose){
        printf("New attributes\n");
        printTermistruct(config);
    }
}

char *getPath(){
    return "/dev/ttyUSB0";
}

void isSudo(){
	// check if current user is root
	char *foo = getenv("USER");
	if (strcmp(foo,"root")==0){
		printf("isSudo: foo = %s\n   You may continue.\n",foo);
	}
	else{
		// if current user is not root, exit
		printf("isSudo: user = %s\n   You must run as root.\n",foo);
		printf("isSudo: usage: sudo ./test\n");
		_exit(1);
	}
}

int myOpen(const char *device){
    // open read-write, USB does not control the code, non-blocking
	printf("myOpen: Device Path: %s\n",device);
	int fd = open(device,O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd==-1){
		printf("myOpen: fail: %s\n",strerror(errno));
	}
	return fd;
}

int myClose(int fd){
    // close USB
	int diditwork;
	if (fd>=0){
		diditwork = close(fd);
		if (diditwork<0){
			printf("failed to close: %s\n",strerror(errno));
		}
	}else{
		diditwork = -10;
	}
	return diditwork;
}

struct termios initTerms(int fd,struct termios config,int settings){
    if(settings){
// SET TO RAW (basically)
// turn off input processing
// convert break to null byte
// no CR to NL translation
// no NL to CR translation
// don't mark parity errors or break
// no input parity check
// don't strip high bit off
// no XON/XOFF software flow control
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

// turn off output processing
// no CR to NL translation
// no NL to CR-NL translation
// nl NL to CR translation
// no column 0 CR suppression
// co ^D suppression - needs to go somewhere else
// no fill characters
// no case
	config.c_oflag = 0;

// no line processing
// echo off
// echo newline off
// canonical mode off
// extended input processing off
// signal chars off
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

// turn off character processing
// clean current char size mask
// no parity checking
// no output processing
// force 8 bit input
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= CS8;

// one input byte to return from read()
// inter-character timer off
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 0;
    }
    else {
        config.c_lflag &= ~ICANON;
    }

	return config;
}

struct termios setTerms(int fd, struct termios config){
    // set attributes
	int setflag = tcsetattr(fd,TCSANOW,&config);
	if (setflag<0){printf("setTerms: setflag = %i\n",setflag);}

    // confirm that attributes were  set properly
    struct termios testterm = config;
	setflag = tcgetattr(fd,&testterm);
	if (setflag<0){printf("setTerms: getflag = %i\n",setflag);}

	if(config.c_iflag!=testterm.c_iflag){
		printf("setTerms: c_iflags didn't set properly.\n");
	}
	if(config.c_oflag!=testterm.c_oflag){
		printf("setTerms: c_oflags didn't set properly.\n");
	}
	if(config.c_cflag!=testterm.c_cflag){
		printf("setTerms: c_cflags didn't set properly.\n");
	}
	if(config.c_lflag!=testterm.c_lflag){
		printf("setTerms: c_lflags didn't set properly.\n");
	}

	return testterm;
}

void printTermistruct(struct termios config){
    // print all values of configuration structure
	printf("size of config: %lu\n",sizeof(config));
	printf("config->c_iflag: %u\n",config.c_iflag);
	printf("config->c_oflag: %u\n",config.c_oflag);
	printf("config->c_cflag: %u\n",config.c_cflag);
	printf("config->c_lflag: %u\n",config.c_lflag);
}

int setBaudRate(int fd, struct termios config){
	// get current speed
	speed_t speedin = cfgetispeed(&config); // line in
	if (speedin<0){printf("setBaudRate: %s\n",strerror(errno));}
	speed_t speedout = cfgetospeed(&config); // line out
	if (speedout<0){printf("setBaudRate: %s\n",strerror(errno));}
	if (speedin!=speedout){printf("Speed difference:\n\tin = %i\n\tout = %i\n",speedin,speedout);}

    /// set speed
	speedin = cfsetispeed(&config,BAUDRATE); // line in
	if (speedin<0){printf("set in error: %s\n",strerror(errno));}
	speedout= cfsetospeed(&config,BAUDRATE); // line out
	if (speedout<0){printf("set out error: %s\n",strerror(errno));}
	if (speedin!=speedout){
	    printf("Speed difference:\n\tin = %i\n\tout = %i\n",speedin,speedout);
	    return -1;
	}
    else{
        return speedin;
    }
}
