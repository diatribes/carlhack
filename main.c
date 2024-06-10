#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "entity.h"
#include "rune.h"

#define MAP_COLS 64
#define MAP_ROWS 32

#define map_param struct rune map[MAP_ROWS][MAP_COLS]
#define map_rune_char(r,c) (map[(r)][(c)].ch[map[(r)][(c)].i])

struct entity_list entity_list;

static void die(int status)
{
    endwin();
    exit(status);
}

static void handle_signal(int signum)
{
    switch (signum) {
    case SIGUSR1:
    case SIGINT:
    case SIGTERM:
        die(0);
    break;
    default:
        fprintf(stderr, "unhandled signal: %d", signum);
        break;
    }
}

static int randrange (int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

static void entity_add(int r, int c, enum rune_type_enum type)
{
    if (entity_list.count < ENTITY_MAX) {
        int i = entity_list.count;
        e(i).id = type == rune_type_player ? 0 : 0xffff - i;
        e(i).r = r;
        e(i).c = c;
        e(i).active = 1;
        e(i).rune = rune_get_default(type);
        entity_list.count++;
    }
}

static void new_cell(map_param, int r, int c, int level)
{
    int roll = 0;
    if (r > 0 && r < (MAP_ROWS - 1) && c > 0 && c < (MAP_COLS - 1)) {
        roll = randrange(1, 1000);
        switch(roll) {
            case 999:
                entity_add(r, c, rune_type_spider);
                break;
            case 888:
                entity_add(r, c, rune_type_bandit);
                break;
            default:
                break;
        }
        map[r][c] = rune_get_default(rune_type_floor);
    } else {
        map[r][c] = rune_get_default(rune_type_wall);
    }
}

static void new_level(map_param, int level)
{
    int i;
    int r;
    int c;

    entity_list.count = 0;
    entity_add(0, 0, rune_type_player);

    for (r = 0; r < MAP_ROWS; r++) {
        for (c = 0; c < MAP_COLS; c++) {
            new_cell(map, r, c, level);
        }
    }

    r = 0;
    c = 0;
    r = randrange(r+3, MAP_ROWS - 3);
    for (i = 1; i < MAP_COLS; i++) {
        map[r][i] = rune_get_default(rune_type_wall);
    }
    c = randrange(c+3, MAP_COLS - 3);
    for (i = 1; i < MAP_ROWS; i++) {
        map[i][c] = rune_get_default(rune_type_wall);
    }

    int door1 = randrange(1, c-1);
    int door2 = randrange(c+1, MAP_COLS - 1);
    map[r][door1] = rune_get_default(rune_type_door);
    map[r][door2] = rune_get_default(rune_type_door);
    
    int door3 = randrange(1, r-1);
    int door4 = randrange(r+1, MAP_ROWS - 1);
    map[door3][c] = rune_get_default(rune_type_door);
    map[door4][c] = rune_get_default(rune_type_door);

    do {
        r = randrange(1, MAP_ROWS);
        c = randrange(1, MAP_COLS);
    } while (map[r][c].type != rune_type_floor);
    map[r][c] = rune_get_default(rune_type_stairs);

    while (map[r][c].type != rune_type_floor) {
        r = randrange(1, MAP_ROWS);
        c = randrange(1, MAP_COLS);
    };
    e(0).r = r;
    e(0).c = c;
    e(0).rune = rune_get_default(rune_type_player);
}

static void player_tick(map_param, struct entity *e, int level)
{
    int r = e(0).r;
    int c = e(0).c;
    int new_r = r;
    int new_c = c;
    (void)e;

    int k = getch();
    switch (k) {
    case 'j':
    case 258:
        new_r = r + 1;
        break;
    case 'k':
    case 259:
        new_r = r - 1;
        break;
    case 'h':
    case 260:
        new_c = c - 1;
        break;
    case 'l':
    case 261:
        new_c = c + 1;
        break;
    case 'q':
        die(0);
        break;
    }

    if (r != new_r || c != new_c) {
        if (!map[new_r][new_c].blocking) {
            e(0).r = new_r;
            e(0).c = new_c;
            r = new_r;
            c = new_c;
        }
    }

    if (map[r][c].type == rune_type_stairs) {
        new_level(map, level);
    }
}
static void enemy_tick(map_param, struct entity *e, int level)
{
    int i;
    int r = e(0).r;
    int c = e(0).c;

    int er = e->r;
    int ec = e->c;

    if (er > r) {
        e->r--;
    } else if (er < r) {
        e->r++;
    }

    if (ec > c) {
        e->c--;
    }
    else if (ec < c) {
        e->c++;
    }

    if (map[e->r][ec].blocking) {
        e->r = er;
    }

    if (map[er][e->c].blocking) {
        e->c = ec;
    }

    for (i = 0; i < entity_list.count; i++) {
        if (e(i).id == e->id) continue;

        if (e(i).r == e->r && e(i).c == e->c) {
            e->r = er;
            e->c = ec;
        }
    }

    if (e->r == r && e->c == c) {
        e->active = 0;
    }
}

static void entity_tick(map_param, struct entity *e, int level)
{
    switch(e->rune.type) {
    case rune_type_player:
        break;
    case rune_type_spider:
    case rune_type_bandit:
        enemy_tick(map, e, level);
        break;
    case rune_type_floor:
    case rune_type_door:
    case rune_type_wall:
    case rune_type_stairs:
    case rune_type_count:
        break;
    }
}

static void main_loop_body(map_param, int level)
{
    int i, r, c;

    for (i = 0; i < MAP_ROWS*MAP_COLS; i++) {
        r = i / MAP_COLS;
        c = i % MAP_COLS;
        mvaddch(r, c, map_rune_char(r, c));
    }
    for(i = 0; i < entity_list.count; i++) {
        entity_tick(map, &e(i), level);
        if (e(i).active) {
            mvaddch(e(i).r, e(i).c, entity_char(i));
        }
    }
    player_tick(map, &e(0), level);
    standout();
    mvaddch(e(0).r, e(0).c, entity_char(0));
    standend();
}

int main()
{
    int level = 1;
    struct rune map[MAP_ROWS][MAP_COLS];

    (void)signal(SIGINT, handle_signal);
    (void)signal(SIGTERM, handle_signal);
    (void)signal(SIGPIPE, SIG_IGN);

    initscr();
    cbreak();
    srand(time(0));
    keypad(stdscr, 1);
    curs_set(0);
    mvaddnstr(0,0,"test123", -1);
    
    new_level(map, level);

    while (1) {
        main_loop_body(map, level);
    }

    die(0);
}
