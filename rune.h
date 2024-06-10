#ifndef _RUNE_H_
#define _RUNE_H_

enum rune_type_enum {
   rune_type_floor = 0,
   rune_type_door,
   rune_type_wall,
   rune_type_player,
   rune_type_stairs,
   rune_type_bandit,
   rune_type_spider,

   rune_type_count,
};

struct rune {
    enum rune_type_enum type;
    char ch[4];
    int i;
    int blocking;
};

static struct rune rune_default[rune_type_count] = {
    {
        .type = rune_type_floor,
        .ch = { ' ', '.', ' ', ' ' },
        .i = 0,
        .blocking = 0,
    },
    {
        .type = rune_type_door,
        .ch = { ' ', ' ', ' ', ' ' },
        .i = 0,
        .blocking = 0,
    },
    {
        .type = rune_type_wall,
        .ch = { '#', ':', '#', '#' },
        .i = 0,
        .blocking = 1,
    },
    {
        .type = rune_type_player,
        .ch = { '@', 'v', '<', '>' },
        .i = 0,
        .blocking = 1,
    },
    {
        .type = rune_type_stairs,
        .ch = { '%', '%', '%', '%' },
        .i = 0,
        .blocking = 0,
    },
    {
        .type = rune_type_bandit,
        .ch = { 'B', '!', 'B', '!' },
        .i = 0,
        .blocking = 1,
    },
    {
        .type = rune_type_spider,
        .ch = { 'X', '!', 'B', '!' },
        .i = 0,
        .blocking = 1,
    },

};

struct rune rune_get_default(enum rune_type_enum rt)
{
    return rune_default[rt];
}

#endif

