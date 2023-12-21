
// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

//***********************************Includes**************************************

#include "./G8RTOS/G8RTOS.h"

//***********************************Includes**************************************

//************************************Defines**************************************

#define IMU_FIFO      0

void print_ball(uint8_t x, uint8_t y);
void delete_ball(uint8_t x, uint8_t y);

//************************************Defines**************************************

//**********************************Semaphores*************************************

semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;

//**********************************Semaphores*************************************

//**********************************Structures*************************************
//**********************************Structures*************************************


//******************************Background Threads*********************************
void Idle_Thread(void);
void read_imu(void);
void player(void);
void board0(void);
void board1(void);
void board2(void);
void board3(void);
void Read_Button(void);

//******************************Background Threads*********************************

//*******************************Periodic Threads**********************************
void draw_canvas(void);


//*******************************Periodic Threads**********************************

//******************************Aperiodic Threads**********************************
void win_condition(void);
void lose_condition(void);
void start_game(void);


//******************************Aperiodic Threads**********************************


#endif  /* THREADS_H_ */


/*
// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

***********************************Includes**************************************

#include "./G8RTOS/G8RTOS_Semaphores.h"
#include "./G8RTOS/G8RTOS_Scheduler.h"

***********************************Includes**************************************

**********************************Semaphores*************************************

semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPIA;

**********************************Semaphores*************************************


*******************************Thread Functions**********************************
void Timer_Init();
void Idle(void);
void Thread0(void);
void Thread1(void);
void periodic_test(void);
void aperiodic_test(void);
*/

/********************************Thread Functions***********************************/

