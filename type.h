#ifndef __TYPE_H
#define __TYPE_H

#ifdef  LINUX32
typedef int sint32;
typedef unsigned int uint32;
typedef long long sint64;
typedef unsigned long long uint64;
#endif

#ifdef  LINUX64
typedef int sint32;
typedef unsigned int uint32;
typedef long sint64;
typedef unsigned long uint64;
#endif

typedef short sint16;
typedef unsigned short uint16;

//for libevent:begain
typedef unsigned char u_char;
//for libevent:end
typedef unsigned char ubyte;

#endif /* __TYPE_H */
