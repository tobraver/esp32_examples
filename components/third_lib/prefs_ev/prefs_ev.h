#ifndef __PREFS_EV_H__
#define __PREFS_EV_H__

/**
 * @brief 
 * 
 * 1. [0 ~ max_cunter) event storage with loop! [dependce: nvs, prefs, mutex]
 * 
 * 2. better choose xxxx_u64, usr less ram and flash!
 * 
 * 3. better choose peed and move for reading!
 * 
 * 4. the same prefs event can't mix xx_u64 and xx_bolck at the same time!
 * 
 * 5. flash usage: [byte]
 *    xxx_64: 32 * n_cnt
 *    xxx_blcok: (32 * (sizeof(type) + 1)) * n_cnt
 */

#include "stdio.h"
#include "stdint.h"
#include "prefs.h"
#include "mutex.h"

typedef struct 
{
    uint32_t read;
    uint32_t write;
} prefs_idx_t;

typedef struct 
{
    prefs_t     hprefs; /* different prefs event handle, part_name can same, namespace must different. */
    uint32_t    max_count; /* prefs event max counter. */
    prefs_idx_t idx; /* don't care. */
    mutex_t     hmutex; /* don't care. */
} prefs_ev_t;


bool prefs_ev_init(prefs_ev_t* hprefs_ev); /* prefs event init. */

bool prefs_ev_peek_u64(prefs_ev_t* hprefs_ev, uint64_t* value); /* prefs event peek. */
bool prefs_ev_peek_u64_muti(prefs_ev_t* hprefs_ev, uint32_t idx, uint32_t cnt, uint64_t value[], uint32_t* length);
bool prefs_ev_read_u64(prefs_ev_t* hprefs_ev, uint64_t* value); /* prefs event read = peek + remove. */
bool prefs_ev_write_u64(prefs_ev_t* hprefs_ev, uint64_t value); /* prefs event write. */

bool prefs_ev_peek_block(prefs_ev_t* hprefs_ev, void* buff, uint32_t size); /* prefs event peek. */
bool prefs_ev_peek_block_muti(prefs_ev_t* hprefs_ev, uint32_t idx, uint32_t cnt, void* value, uint32_t size, uint32_t* length);
bool prefs_ev_read_block(prefs_ev_t* hprefs_ev, void* buff, uint32_t size); /* prefs event read = peek + remove. */
bool prefs_ev_write_block(prefs_ev_t* hprefs_ev, void* buff, uint32_t size); /* prefs event write. */

bool prefs_ev_move(prefs_ev_t* hprefs_ev); /* prefs event move read, but not erase. */
bool prefs_ev_remove(prefs_ev_t* hprefs_ev);  /* prefs event remove. */
bool prefs_ev_clear(prefs_ev_t* hprefs_ev);  /* prefs event clear all. */

#endif // !__PREFS_EV_H__
