#pragma once 

#ifdef __x86_64__
#define HAS_PAUSE 1
#endif

void sync_enter(void);

void sync_leave(void);

void sync_pause(void);

