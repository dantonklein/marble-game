# marble-game
This repository contains the Marble game I made for the TM4C123G LaunchPad evaluation kit for my Microprocessor Applications 2 course. This game runs on an real-time operating system that we developed in class that utilizes a priority based round-robin scheduler. The RTOS has blocking semaphores for resource sharing and for interprocess communication. It also has FIFOs to allow for constant data streaming between threads. 

The game utilizes an accelerometer on an external sensorpack to navigate the marble through one of four different levels that the user can choose from. The data from the accelerometer is streamed from the thread that reads the accelerometer to the thread that handles the marble with a FIFO. 

The project was coded in C on Texas Instrument's Code Composer.

Youtube video that showcases the system design of the game:
https://youtu.be/P0a6EwH9EGg

Demo of the game:
https://youtu.be/a4KbEc8K5RY


