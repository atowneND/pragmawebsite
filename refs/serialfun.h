#ifndef SERIALFUN_H_
#define SERIALFUN_H_

// wrapper funtion
void serialReceiveInit(int verbose, int settings);

// get pathname of USB
char *getPath();

// check for permissions
void isSudo();

// returns file descriptor
int myOpen(const char *device); 

// close USB device
int myClose(int fd);

// initialize attributes
struct termios initTerms(int fd, struct termios config, int settings);

// return struct read after the port attributes have been set
struct termios setTerms(int fd, struct termios config);

// print all attributes (integer values)
void printTermistruct(struct termios config);

// set baud - return line out speed
int setBaudRate(int fd, struct termios config);

#endif
