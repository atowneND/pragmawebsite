#ifndef USBPIPE_H_
#define USBPIPE_H_


#define MAXSIZE 10
#define STOP_DELIMITER 'z'

char *getDataLinux(char **mydata);
void writeToFile(int indata);

#endif
