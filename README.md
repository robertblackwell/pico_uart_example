# Pico Uart Example

This projectconnects two raspberrypi pico board by their uart ports and transfers data between them.

The goal is to get a measurement of the data throughput.

One pico acts as a sender of data and the other acts as receiver of data.

The sender transmit lines of text with each one different as fast (no delay() or sleep_ms() calls) as possible and writes 
dots to its stdout port as a progress report; one dot per line of text sent.

The reader reads the lines as fast as possible (by reading characters one at a time whenever characters area available)
and also writes dots as a progress report.

The reader reads 20,000 lines and then prints a report detailiing the total elapsed time of the 
reading process and the total number of characters received.

## The pico program

Both picos receive the same code.

The role of sender or receiver is determined by the `board id`.

The common code path in the main() function calls the pico-sdk function `pico_get_unique_board_id_string()`
which returns the boards unique id as a string (char*).

By testing this board id value the code paths fork into sender or receiver.

Hence to use this project you will have to determine the unique id of one of your boards and replace the hard coded
unique id string in the main function.

## Running this example

You need 

-	2 raspberrypi pico boards,
-	each connected via a usb cable to your development machine
-	connected to each other via uarts rx/tx crossed GND to GND - thats 3 wires.
-	you have to use the same RX and TX pins on each board, thats the way the code is written.
-	a copy of minicom or some other terminal emulator reading the input from the boards, on Ubuntu the ports will be /dev/ttyACM0 and /dev/ttyACM1
-	put the receive board into data storage mode (reconnect power with the bootsel switch held)
-	copy the pico_uart_example.uf2 to the receiver board
-	put the sender board into data storage mode (reconnect power with the bootsel switch held)
-	copy the pico_uart_example.uf2 to the sender board

After this you should see dots on each minicom display.
