#ifndef _MEM_LAYOUT_H
#define _MEM_LAYOUT_H


#define KERN_BASE 0x00000000
#define KERN_TOP  0x10000000 /* reserve 256MB to kernel. 
                                0x0FFFFFFF is the highest address reserved to 
                                kernel */
#define USR_BASE  0x10000000
#define USR_TOP   0x20000000 /* 0x1FFFFFFF is the highest address. 
                                256MB user space RAM */

#define UART0 0x44E09000

#endif /* _MEM_LAYOUT_H */
