#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include <pico/types.h>
#include <pico/unique_id.h>
#include <pico/time.h>
#include "hardware/gpio.h"
#include "stats.h"
#include "progress.h"

#if 1
#define UART_ID uart0
#define UART_BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#endif
#if 0
#define UART_ID uart1
#define UART_BAUD_RATE 115200
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#endif
#if 0
#define UART_ID uart0
#define UART_BAUD_RATE 115200
#define UART_TX_PIN 16
#define UART_RX_PIN 17
#endif
#undef RECEIVER_VERBOSE
#undef RECV_STATS
#if 0
typedef struct statistic_s {
    int count;
    int         recv_max_line_count;
    int         recv_dots_per_line;
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

statistics_t my_stats;
void stats_init(statistics_t* stats, int max_line_count, int dots_per_line) {
    stats->state = STATS_BEGIN_IDLE;
    stats->count = 0;
    stats->recv_dots_per_line = dots_per_line;
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

// void stats_begin(statistics_t * stats) 
// {
//     if(stats->recv_first_char) {
//         stats->recv_start_time = to_us_since_boot(get_absolute_time());
//         stats->recv_first_char = false;
//     }
// }
// void stats_begin_line(statistics_t * stats) 
// {
//     if(stats->recv_first_char) {
//         stats->recv_start_time = to_us_since_boot(get_absolute_time());
//         stats->recv_first_char = false;
//     }
// }

// void stats_character(statistics_t* stats, char ch) 
// {

// }

// void stats_end_line(statistics_t* stats, int line_length, uint64_t time_us_since_boot) 
// {
//     stats->recv_total_chars += line_length;   
// }
// void stats_end(statistics_t* stats, int line_length, uint64_t time_us_since_boot) 
// {
//     stats->recv_end_time = to_us_since_boot(get_absolute_time());
//     stats->state = STATS_END;
// }
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
#endif
statistics_t my_stats;
progress_t my_progress;

int count;

int  recv_line_count = 0;
bool recv_first_char = true;
int  recv_buffer_len;
long recv_total_chars;
long recv_interval_line;
uint64_t recv_interval_total = 0;
// absolute_time_t recv_start_time;
uint64_t recv_start_time;
// absolute_time_t recv_end_time;
uint64_t recv_end_time;
#ifdef RECV_STATS
#endif

const int LED_PIN = 25;
char unique_id[8];
#define BUFFER_MAX_LENGTH 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 2


char out_buffer[512];
int out_buffer_len;
char in_buffer[512];
int in_buffer_len;
bool iam_sender;
void printf_unique_id(char* buffer, uint8_t* uid, int len) 
{
    int blen = 0;
    for(int ix = 0; ix < len; ix++){
        blen = blen + sprintf(buffer, "%2x", uid[ix]);
    } 
    buffer[blen] = '\0';

}
#undef SENDER_VERBOSE

void stdio_putc(char ch) {
    printf("%c", ch);
}

void wait_for_ack(){
    while(1) {
        if(uart_is_readable(UART_ID)) {
            char ch = uart_getc(UART_ID);
            if(ch == 'A') {
                break;
            } else {
                printf("wait_for_ack got %c %d\n", (char)ch, (int)ch);
            }
        } else {
            ;
        }
    }
}

void write_string(char* buffer) {
    char* p = buffer;
    while(*p != '\0'){
        if(uart_is_writable(UART_ID)) {
            uart_putc(UART_ID, *p);
            p++;
        }
    }
}
void send_ack(){
    write_string("A");
}
void sender(char* uid_buffer)
{
    count++;
    int len = sprintf(out_buffer, "From sender count: %d my board_id %s \n", count, uid_buffer);
    out_buffer[len] = '\0';
    write_string(out_buffer);
    progress_update(&my_progress);
    // #if defined SENDER_VERBOSE || 0
    // printf("Sender msg len: %d count: %d my board_id %s \n", len, count, uid_buffer);
    // sleep_ms(10);
    // #else
    // printf(".");
    // if(count % 30 == 0) {
    //     printf("\n");
    // }
    // #endif
}
#if 0
void receiver(char* uid_buffer)
{
    send_ack();
    while(1) {
        in_buffer_len = 0;
        while(1) {
            while(uart_is_readable(UART_ID)) {
                #if defined RECV_STATS || 1
                if(recv_first_char) {
                    recv_start_time = to_us_since_boot(get_absolute_time());
                    recv_first_char = false;
                }
                #endif
                char ch = uart_getc(UART_ID);
                // printf("got a character %d\n", (int)ch);
                if(ch == '\n') {
                    recv_line_count++;
                    #if defined  RECV_STATS || 1
                    
                    recv_buffer_len = in_buffer_len+1;
                    recv_total_chars = recv_total_chars + in_buffer_len+1;
                    // printf("get_absolute_time\n");
                    absolute_time_t stop_time = get_absolute_time();
                    // printf("before now\n");
                    uint64_t now_us = to_us_since_boot(get_absolute_time());
                    recv_end_time = now_us;
                    // printf("now %llu\n", now);
                    uint64_t then = recv_start_time;
                    // printf("then %llu\n", now);
                    uint64_t d = to_us_since_boot(get_absolute_time()) - recv_start_time;
                    recv_interval_total = recv_interval_total + d;
                    #endif
                    #if defined RECIEVER_VERBOSE || 0
                    printf("Receiver got a line [%s]\n", in_buffer);
                    #else
                        if(recv_line_count % 30 == 0) 
                            printf(".\n");
                        else
                            printf(".");
                    #endif
                    if(recv_line_count > 20000) {
                    #if defined RECV_STATS || 1
                         {
                            float elapsed = (double)(recv_end_time) - (double)(recv_start_time);
                            printf("Statistics line_count: %d\n", recv_line_count);
                            printf("   Start time                  : %f\n", (float)(recv_start_time));
                            printf("   End time                    : %f\n", (float)(recv_end_time));
                            printf("   Total interval in micro secs: %f\n", elapsed);
                            printf("   Total interval in secs      : %f\n", elapsed / 1000000.0);
                            printf("   Total characters received   : %f\n", (float)(recv_total_chars));
                            printf("   Overall data rate in ch/s   : %f\n", ((float)(recv_total_chars)/(elapsed/1000000.0)));
                            while(1){;}
                        }
                    #else
                        while(1) {
                            printf("recv line count");
                            sleep_ms(2000);
                        }
                    #endif
                    }
                    in_buffer_len = 0;
                    // ready for the next line - send 'A' as ack    
                    send_ack();
                    #ifdef RECV_STATS
                    recv_start_time = get_absolute_time();
                    #endif
                } else if(ch == '\r') {
                    ;
                } else {
                    in_buffer[in_buffer_len] = ch;
                    in_buffer[in_buffer_len + 1] = '\0';
                    in_buffer_len++;
                }
            }
        }
    }
}
#endif
void receive_line(char* uid_buffer)
{
    in_buffer_len = 0;
    while(1) {
        while(uart_is_readable(UART_ID)) {
            char ch = uart_getc(UART_ID);
            in_buffer[in_buffer_len] = ch;
            in_buffer_len++;
            stats_state_machine(&my_stats, ch);
            // printf("got a character %d\n", (int)ch);
            if(ch == '\n') {
                in_buffer[in_buffer_len] = '\0';
                progress_update(&my_progress);
                // #if defined RECV_VERBOSE
                // printf("line recv'd: [%s]\n", in_buffer);
                // #else
                // printf("%s", ((my_stats.recv_line_count %my_stats.recv_dots_per_line)==0)?".\n": ".");
                // #endif
                return;
            }
        }
    }
}
int main()
{
    count = 0;
    char uid_buffer[BUFFER_MAX_LENGTH];
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    stdio_init_all();
    sleep_ms(2000);
    uart_init(UART_ID, UART_BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    pico_get_unique_board_id_string(uid_buffer, BUFFER_MAX_LENGTH);
    iam_sender = (strcmp(uid_buffer, "E6614104036E8D32") == 0);
    printf("uid_buffer is : [%s] iam_%s \n", uid_buffer, (iam_sender)? " sender ": " receiver ");
    
    stats_init(&my_stats, 5000);
    progress_init(&my_progress, stdio_putc, 100);
    sleep_ms(5000);
    printf("after sleep uid_buffer is : [%s] iam_sender: %d \n", uid_buffer, iam_sender);
    while(1) {
        #if 0
        sleep_ms(1000);
        printf("Looping\n");
        #else
        if(iam_sender) {
            wait_for_ack();
            sender(uid_buffer);
        } else {
            send_ack();
            receive_line(uid_buffer);
            if(stats_is_ended(&my_stats)) {
                break;
            }
        }
        #endif
    }
    stats_report(&my_stats);
}