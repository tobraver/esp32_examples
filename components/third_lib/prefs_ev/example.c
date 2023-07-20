#include "prefs_ev.h"

void example1(void)
{
    #define MAX_EV_COUNT   50

    prefs_ev_t hprefs_ev = {
        .hprefs.part_name = "event",
        .hprefs.namespace = "group1",
        .max_count = MAX_EV_COUNT,
        .idx.read = 0,
        .idx.write = 0,
        .hmutex = NULL,
    };

    prefs_ev_init(&hprefs_ev);

    uint64_t buff[MAX_EV_COUNT] = {0};
    
    for(int32_t i=0; i<MAX_EV_COUNT; i++)
    {
        buff[i] = i+1;
        prefs_ev_write_u64(&hprefs_ev, buff[i]);
    }
    
    delay_ms(100);
    
    uint64_t v[MAX_EV_COUNT] = {};
    uint32_t l=0;
    prefs_ev_peek_u64_muti(&hprefs_ev, 0, MAX_EV_COUNT, v, &l);
    printf("length: %d\n", l);
    for(int i=0; i<l; i++)
    {
        printf("value [%d]: %lld\n", i, v[i]);
    }
    printf("---------###-------------\n");

    for(int32_t i=0; i<MAX_EV_COUNT; i++)
    {
        uint64_t value = 0;
        prefs_ev_peek_u64(&hprefs_ev, &value);
        prefs_ev_move(&hprefs_ev);
        printf("value : %lld\n", value);
    }
    printf("---------###-------------\n");

    delay_ms(100);

    for(int32_t i=0; i<MAX_EV_COUNT; i++)
    {
        uint64_t value = 0;
        bool r = prefs_ev_peek_u64(&hprefs_ev, &value);
        prefs_ev_move(&hprefs_ev);
        if(r)
        {
            printf("value : %lld\n", value);
        }
        else {
            printf("peek failed!, [%i]\n", i);
        }
    }
    printf("---------###-------------\n");
}



void example2(void)
{
    #define MAX_EV_COUNT   3

    typedef struct
    {
        uint32_t age;
        uint32_t score;
    } student_t;

    prefs_ev_t hprefs_ev = {
        .hprefs.part_name = "event",
        .hprefs.namespace = "group2",
        .max_count = MAX_EV_COUNT,
        .idx.read = 0,
        .idx.write = 0,
        .hmutex = NULL,
    };

    prefs_ev_init(&hprefs_ev);

    student_t buff[MAX_EV_COUNT] = {0};
    
    for(int32_t i=0; i<MAX_EV_COUNT; i++)
    {
        buff[i].age = i+1;
        buff[i].score = i+1;
        prefs_ev_write_block(&hprefs_ev, &buff[i], 8);
    }
    
    delay_ms(100);
    
    student_t v[MAX_EV_COUNT] = {};
    uint32_t l=0;
    prefs_ev_peek_block_muti(&hprefs_ev, 0, MAX_EV_COUNT, v, 8, &l);
    printf("length: %d\n", l);
    for(int i=0; i<l; i++)
    {
        printf("value age   [%d]: %d\n", i, v[i].age);
        printf("value score [%d]: %d\n", i, v[i].score);
    }
    printf("---------###-------------\n");

    for(int32_t i=0; i<MAX_EV_COUNT; i++)
    {
        student_t st = { 0 };
        prefs_ev_peek_block(&hprefs_ev, &st, sizeof(st));
        prefs_ev_move(&hprefs_ev);
        printf("value age   [%d]: %d\n", i, st.age);
        printf("value score [%d]: %d\n", i, st.score);
    }
    printf("---------###-------------\n");

    delay_ms(100);

    for(int32_t i=0; i<MAX_EV_COUNT; i++)
    {
        student_t st = { 0 };
        bool r = prefs_ev_peek_block(&hprefs_ev, &st, sizeof(st));
        prefs_ev_move(&hprefs_ev);
        if(r)
        {
            printf("value age   [%d]: %d\n", i, st.age);
            printf("value score [%d]: %d\n", i, st.score);
        }
        else {
            printf("peek failed!, [%i]\n", i);
        }
    }
    printf("---------###-------------\n");
}



