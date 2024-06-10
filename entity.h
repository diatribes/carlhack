#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "rune.h"

#define ENTITY_MAX 16
#define e(i) entity_list.entity[(i)]
#define entity_char(n) entity_list.entity[(n)].rune.ch[entity_list.entity[(n)].rune.i]

struct entity {
    int id;
    int r;
    int c;
    int active;
    struct rune rune;
};

struct entity_list {
    struct entity entity[ENTITY_MAX];
    int count;
};

#endif
