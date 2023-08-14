#ifndef _COMMON_H
#define _COMMON_H

typedef unsigned char   u8_t;
typedef signed char     s8_t;
typedef unsigned short  u16_t;
typedef signed short    s16_t;
typedef unsigned int    u32_t;
typedef signed int      s32_t;

#define true 1
#define false 0

#ifndef NULL
#define NULL ((void*) 0)
#endif

#define REG(addr) (*((volatile u32_t*)(addr)))

#endif /* _COMMON_H */

