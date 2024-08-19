#ifndef MMU_H
#define MMU_H

#include <memory/pml.h>
#include <stdint.h>

#define KERNEL_HEAP_START 0xFFFFff0000000000UL
#define MMIO_BASE_START   0xffffff1fc0000000UL
#define HIGH_MAP_REGION   0xffffff8000000000UL

#define MODULE_BASE_START 0xffffffff80000000UL

#define USER_SHM_LOW      0x0000400100000000UL
#define USER_SHM_HIGH     0x0000500000000000UL
#define USER_DEVICE_MAP   0x0000400000000000UL

#define MMU_FLAG_KERNEL       0x01
#define MMU_FLAG_WRITABLE     0x02
#define MMU_FLAG_NOCACHE      0x04
#define MMU_FLAG_WRITETHROUGH 0x08
#define MMU_FLAG_SPEC         0x10
#define MMU_FLAG_WC           (MMU_FLAG_NOCACHE | MMU_FLAG_WRITETHROUGH | MMU_FLAG_SPEC)
#define MMU_FLAG_NOEXECUTE    0x20
#define MMU_GET_MAKE 0x01
#define MMU_PTR_NULL  1
#define MMU_PTR_WRITE 2

/* frame management */

/* mapping */

/* paging */

/* misc */


#endif /* ifndef MMU_H */
