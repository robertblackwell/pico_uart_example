# Pico Uart Example

This project connects two raspberrypi pico board by their uart ports and transfers data between them.

The goal is to get a measurement of the data throughput.

One pico acts as a sender of data and the other acts as receiver of data.

The sender transmit lines of text (each line is different) as fast as possible (that is no delay() or sleep_ms() calls) and writes 
dots to its stdout port as a progress report; one dot per line of text sent.

The reader reads the lines as fast as possible (by reading characters one at a time whenever characters area available)
and also writes dots as a progress report.

The reader reads 20,000 lines and then prints a report detailing the total elapsed time of the 
reading process and the total number of characters received. The number of lines per experiment
can be changed in `main.c`.

## The pico program

Both picos receive the same code.

The role of sender or receiver is determined by the `board id`.

The common code path in the main() function calls the pico-sdk function `pico_get_unique_board_id_string()`
which returns the boards unique id as a string (char*).

By testing this board id value the code paths fork into sender or receiver.

Hence to use this project you will have to determine the unique id of one of your boards and replace the hard coded
unique id string in the main function.

During the startup phase the `main()` function prints out the boards id.

## Running this example

You need 

-	2 raspberrypi pico boards,
-	each connected via a usb cable to your development machine
-	connected to each other
	- the uarts rx/tx pins have to be crossed; that is rx pin on one board connects to the tx pin on the other.
	- the GND of both boards need to be connected together.
-	you have to use the same uart and the same RX and TX pins on each board, thats the way the code is written.
-	arrange to read the sdtout of each board by connecting a copy of minicom or some other terminal emulator to the ports representing the pico boards USB connector. On Ubuntu the ports will be /dev/ttyACM0 and /dev/ttyACM1
-	put the sender board into data storage mode (reconnect power with the bootsel switch held) and copy the pico_uart_example.uf2 to the sender board. 
-	The sender board must be running first as it waits for a transmission from the receiver
	as a signal to start the test.

-	copy the pico_uart_example.uf2 to the receiver board. 
-	Once the receiver completes setup it will signal the sender to start blasting.

After this you should see dots on each minicom display.

## The protocol

There is a very simple protocol in effect between the two pico chips.

-	The receiver must send the single character 'A' to the sender before the sender will transmit 
-	once the sender sees the 'A' it will transmit an entire line ending in a '\n' character.
-	the receiver reads all characters up to and including the '\n'.
-   after receiving and processing the line of text the receiver sends another 'A' to allow the transmission of the next line.
-   This continues until the receiver has received a speified number of lines. 
-   It then stops sending the 'A' after receiving the last '\n'. This prevents the sender from sending more data.

## Here are some sample statistics 

```bash
Statistics line_count: 5000
   Start time                  : 7003321.000000
   End time                    : 31093782.000000
   Total interval in micro secs: 24090460.000000
   Total interval in secs      : 24.090460
   Total characters received   : 268893.000000
   Overall data rate in ch/s   : 11161.804299

```

This experiment achieves approximately 11,000 characters per second throughput in one direction.