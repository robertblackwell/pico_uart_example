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

statistics_t my_stats;
progress_t my_progress;

#if 0
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
#endif

const int LED_PIN = 25;
char unique_id[8];
#define BUFFER_MAX_LENGTH 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 2


char out_buffer[512];
int out_buffer_len;
char in_buffer[512];
int in_buffer_len;
bool iam_sender;

int sender_line_count;

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
    sender_line_count++;
    int len = sprintf(out_buffer, "From sender count: %d my board_id %s \n", sender_line_count, uid_buffer);
    out_buffer[len] = '\0';
    write_string(out_buffer);
    progress_update(&my_progress);
}
void receive_line(char* uid_buffer)
{
    in_buffer_len = 0;
    while(1) {
        while(uart_is_readable(UART_ID)) {
            char ch = uart_getc(UART_ID);
            in_buffer[in_buffer_len] = ch;
            in_buffer_len++;
            stats_state_machine(&my_stats, ch);
            if(ch == '\n') {
                in_buffer[in_buffer_len] = '\0';
                progress_update(&my_progress);
                return;
            }
        }
    }
}
int main()
{
    char uid_buffer[BUFFER_MAX_LENGTH];
    sender_line_count = 0;
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
    }
    stats_report(&my_stats);
}