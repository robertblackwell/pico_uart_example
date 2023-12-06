#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include <pico/types.h>
#include <pico/unique_id.h>
#include <pico/time.h>
#include "hardware/gpio.h"

void stats_state_machine(statistics_t* stats, char ch) {
    switch(stats->state) {
        case STATS_BEGIN_IDLE:
            stats->recv_start_time = to_us_since_boot(get_absolute_time());
            stats->recv_total_chars++;
            stats->state = STATS_LINE;
            break;
        case STATS_LINE:
            stats->recv_total_chars++;
            if(ch == '\n') {
                stats->recv_line_count++;
                if(stats->recv_line_count >= stats->recv_max_line_count) {
                    stats->recv_end_time = to_us_since_boot(get_absolute_time());
                    stats->state = STATS_END;
                } else {
                    stats->state = STATS_INTERLINE_IDLE;
                }
            } else {
                ;
            }
            break;
        case STATS_INTERLINE_IDLE:
            stats->recv_total_chars++;
            stats->state = STATS_LINE;
            break;
        case STATS_END:

            break;
    }
}
bool stats_is_ended(statistics_t* stats)
{
    return stats->state == STATS_END;
}

void stats_init(statistics_t* stats, int max_line_count) {
    stats->state = STATS_BEGIN_IDLE;
    stats->count = 0;
    stats->recv_max_line_count = max_line_count;
    stats->recv_line_count = 0;
    stats->recv_first_char = true;
    stats->recv_interval_total = 0;
    stats->recv_buffer_len = 0;
    stats->recv_total_chars = 0;
    stats->recv_interval_line = 0;
    stats->recv_interval_total = 0;

    stats->recv_start_time = 0;
    stats->recv_end_time = 0;
}

void stats_report(statistics_t* stats) {
    float elapsed = (double)(stats->recv_end_time) - (double)(stats->recv_start_time);
    printf("Statistics line_count: %d\n", stats->recv_line_count);
    printf("   Start time                  : %f\n", (float)(stats->recv_start_time));
    printf("   End time                    : %f\n", (float)(stats->recv_end_time));
    printf("   Total interval in micro secs: %f\n", elapsed);
    printf("   Total interval in secs      : %f\n", elapsed / 1000000.0);
    printf("   Total characters received   : %f\n", (float)(stats->recv_total_chars));
    printf("   Overall data rate in ch/s   : %f\n", ((float)(stats->recv_total_chars)/(elapsed/1000000.0)));
    while(1){;}
}

