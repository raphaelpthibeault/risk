#ifndef SYS_LIST_H
#define SYS_LIST_H

#include <stdbool.h>

/* MUST BE PLACED AT THE TOP OF A STRUCT */
typedef struct list_entry {
    struct list_entry *prev;
    struct list_entry *next;
} list_entry_t;

typedef struct {
    list_entry_t *head;
} list_t;

#endif /* ifndef SYS_LIST_H */
