#ifndef PACK_H
#define PACK_H

#include <cstdio>
#include <cctype>
#include <cstdarg>
#include <cstring>
#include <cstdint>

// macros for packing floats and doubles:
#define pack754_16(f) (pack754((f), 16, 5))
#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_16(i) (unpack754((i), 16, 5))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

/*
** pack754() -- pack a floating point number into IEEE-754 format
*/ 
unsigned long long int pack754(long double f, unsigned bits, unsigned expbits);

/*
** unpack754() -- unpack a floating point number from IEEE-754 format
*/ 
long double unpack754(uint64_t i, unsigned bits, unsigned expbits);

/*
** packi16() -- store a 16-bit int into a char buffer (like htons())
*/ 
void packi16(unsigned char *buf, unsigned int i);

/*
** packi32() -- store a 32-bit int into a char buffer (like htonl())
*/ 
void packi32(unsigned char *buf, unsigned long int i);

/*
** packi64() -- store a 64-bit int into a char buffer (like htonl())
*/ 
void packi64(unsigned char *buf, unsigned long long int i);

/*
** unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
*/ 
int unpacki16(const unsigned char *buf);

/*
** unpacku16() -- unpack a 16-bit unsigned from a char buffer (like ntohs())
*/ 
uint16_t unpacku16(unsigned char *buf);

/*
** unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
*/ 
int32_t unpacki32(const unsigned char *buf);

/*
** unpacku32() -- unpack a 32-bit unsigned from a char buffer (like ntohl())
*/ 
uint32_t unpacku32(const unsigned char *buf);

/*
** unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
*/ 
int64_t unpacki64(const unsigned char *buf);

/*
** unpacku64() -- unpack a 64-bit unsigned from a char buffer (like ntohl())
*/ 
uint64_t unpacku64(const unsigned char *buf);

/*
** pack() -- store data dictated by the format string in the buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C         
**     16 |   h        H         f
**     32 |   l        L         d
**     64 |   q        Q         g
**      - |                               s
**
**  (16-bit unsigned length is automatically prepended to strings)
*/ 

unsigned int pack(unsigned char *buf, char *format, ...);

/*
** unpack() -- unpack data dictated by the format string into the buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C         
**     16 |   h        H         f
**     32 |   l        L         d
**     64 |   q        Q         g
**      - |                               s
**
**  (string is extracted based on its stored length, but 's' can be
**  prepended with a max length)
*/
void unpack(unsigned char *buf, char *format, ...);

#endif // PACK_H