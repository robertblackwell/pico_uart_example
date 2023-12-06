#ifndef H_stats_h
#define H_stats_h
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include <pico/types.h>
#include <pico/unique_id.h>
#include <pico/time.h>
#include "hardware/gpio.h"


typedef struct statistic_s {
    int count;
    int         recv_max_line_count;
    int         state;
    int         recv_line_count;
    bool        recv_first_char;
    int         recv_buffer_len;
    long        recv_total_chars;
    long        recv_interval_line;
    uint64_t    recv_interval_total;
    uint64_t    recv_start_time;
    uint64_t    recv_end_time;

} statistics_t;

#define STATS_BEGIN_IDLE 1
#define STATS_INTERLINE_IDLE 2
#define STATS_LINE 3
#define STATS_END 4

void stats_init(statistics_t* stats, int max_line_count);
void stats_state_machine(statistics_t* stats, char ch);
bool stats_is_ended(statistics_t* stats);
void stats_report(statistics_t* stats);

#endif