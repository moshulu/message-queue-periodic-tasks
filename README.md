# message-queue-periodic-tasks
This project is aimed at creating a "sender" process (main.c) and two reciever processes (reciever1.c and reciever2.c).

The recievers listen to the message queue port (aptly named "\my_queue") and print out anything they read.

The main.c sends out the epoch time (the time since Jan. 1, 1970) by increments of 500ms to the message queue.

### Running the programs
You should run ./reciever1 and ./reciever2 before running ./main although it doesn't really matter.

The program was compiled on a Raspbian GNU/Linux 9 (stretch) system, a variant of debian. If you're getting errors in the output (command prompt, probably) you probably aren't running it in a 64 bit operating system. You have 1 of 2 options:

1. Clone this repo and change the output of the epoch time to "long" instead of "unsigned long"
2. Change your OS image to meet this repo

Pick your poison.
