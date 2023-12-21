
// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

//***********************************Includes**************************************

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Change this to change the number of points that make up each line of a cube.
// Note that if you set this too high you will have a stack overflow!
//#define Num_Interpolated_Points 10

// sizeof(float) * num_lines * (Num_Interpolated_Points + 2) = ?

//#define MAX_NUM_CUBES           (MAX_THREADS - 3)


//********************************Global Variables*********************************
/*Quat_t world_camera_pos = {0, 0, 0, 100};
Quat_t world_camera_frame_offset = {0, 0, 0, 50};
Quat_t world_camera_frame_rot_offset;
Quat_t world_view_rot = {1, 0, 0, 0};
Quat_t world_view_rot_inverse = {1, 0, 0, 0};*/

// How many cubes?
//uint8_t num_cubes = 0;

// y-axis controls z or y
//uint8_t joystick_y = 1;

// Kill a cube?
//uint8_t kill_cube = 0;

//********************************Global Variables*********************************
uint8_t gameboard[100][200] = {0};
//0 is death plane/darkness, 1 game stage, 2 is ball, 3 is ball core, 4 is win zone
//5 is ball with stage underneath, 6 is ball with win zone underneath,
uint8_t level = 0;
float x_coord = 20;
float y_coord = 40;
float x_coord_old = 20;
float y_coord_old = 40;
float x_velocity_initial = 0;
float y_velocity_initial = 0;
float x_velocity = 0;
float y_velocity = 0;

bool game_hasnt_started = 1;
uint8_t game_state = 0;
bool level_loaded = 0;
bool restart_game = 0;
//0 is alive, 1 is loss, 2 is win
//************************************Threads**************************************

void print_ball(uint8_t x, uint8_t y){
    for(int i = -2; i < 3; i++){
        for(int j = -1; j < 2; j++){
            if(gameboard[y+i][x+j] == 0){
                gameboard[y+i][x+j] = 2;
            } else if (gameboard[y+i][x+j] == 1){
                gameboard[y+i][x+j] = 5;
            } else if (gameboard[y+i][x+j] == 4){
                gameboard[y+i][x+j] = 6;
            }
        }
        if(i > -2 && i < 2){

            if(gameboard[y+i][x-2] == 0){
                gameboard[y+i][x-2] = 2;
            } else if (gameboard[y+i][x-2] == 1){
                gameboard[y+i][x-2] = 5;
            } else if (gameboard[y+i][x-2] == 4){
                gameboard[y+i][x-2] = 6;
            }

            if(gameboard[y+i][x+2] == 0){
                gameboard[y+i][x+2] = 2;
            } else if (gameboard[y+i][x+2] == 1){
                gameboard[y+i][x+2] = 5;
            } else if (gameboard[y+i][x+2] == 4){
                gameboard[y+i][x+2] = 6;
            }

        }
    }
    //this will be fixed when i add levels
    uint8_t next_core_location = gameboard[y][x];
    if((next_core_location == 2 && level_loaded == 1) ||
       (next_core_location == 0 && level_loaded == 1)){
        game_state = 1;
        restart_game = 1;
    } else if((next_core_location == 4 && level_loaded == 1) ||
              (next_core_location == 6 && level_loaded == 1)){
        game_state = 2;
        restart_game = 1;
    } else {
    gameboard[y][x] = 3;
    }
}

void delete_ball(uint8_t x, uint8_t y){
    for(int i = -2; i < 3; i++){
            for(int j = -1; j < 2; j++){
                if(gameboard[y+i][x+j] == 2){
                    gameboard[y+i][x+j] = 0;
                } else if (gameboard[y+i][x+j] == 5){
                    gameboard[y+i][x+j] = 1;
                } else if (gameboard[y+i][x+j] == 6){
                    gameboard[y+i][x+j] = 4;
                } else if (gameboard[y+i][x+j] == 3){
                    //Will change for final submission
                    gameboard[y+i][x+j] = 1;
                }
            }
            if(i > -2 && i < 2){

                if(gameboard[y+i][x-2] == 2){
                    gameboard[y+i][x-2] = 0;
                } else if (gameboard[y+i][x-2] == 5){
                    gameboard[y+i][x-2] = 1;
                } else if (gameboard[y+i][x-2] == 6){
                    gameboard[y+i][x-2] = 4;
                }

                if(gameboard[y+i][x+2] == 2){
                    gameboard[y+i][x+2] = 0;
                } else if (gameboard[y+i][x+2] == 5){
                    gameboard[y+i][x+2] = 1;
                } else if (gameboard[y+i][x+2] == 6){
                    gameboard[y+i][x+2] = 4;
                }

            }
        }
}

void Idle_Thread(void) {
    while(1);
}


void player(void){
    int16_t x_imu_data = 0;
    int16_t y_imu_data = 0;

    print_ball((int)x_coord, (int)y_coord);
    while(1){
        //Time is always 11ms
        x_coord_old = x_coord;
        y_coord_old = y_coord;
        x_velocity_initial = x_velocity;
        y_velocity_initial = y_velocity;
        x_imu_data = (G8RTOS_ReadFIFO(IMU_FIFO) - 450) / 1200; //scaling
        y_imu_data = (G8RTOS_ReadFIFO(IMU_FIFO) + 550) / 1200;
        IBit_State = StartCriticalSection();
        x_velocity = (x_velocity_initial + (float)x_imu_data * 0.011 * 200);
        y_velocity = (y_velocity_initial + (float)y_imu_data * 0.011 * 200);
        delete_ball((int)x_coord,(int)y_coord);
        x_coord = x_coord_old + (x_velocity_initial * 0.011 + 0.5 * x_imu_data * (0.011) * (0.011));
        y_coord = y_coord_old + (y_velocity_initial * 0.011 + 0.5 * y_imu_data * (0.011) * (0.011));
        //x_coord += x_imu_data;
        //y_coord += y_imu_data;
        if(x_coord < 2 || x_coord > 220){
            x_coord = 2;
        }
        else if(x_coord > 197){
            x_coord = 197;
        }
        if(y_coord < 2 || y_coord > 120){
            y_coord = 2;
        }
        else if(y_coord > 97){
            y_coord = 97;
        }
        print_ball((int)x_coord, (int)y_coord);
        EndCriticalSection(IBit_State);
        if(game_state>0){
            G8RTOS_KillSelf();
        }
        sleep(11);

    }
}
void read_imu(void) {
    int16_t x_data;
    int16_t y_data;
    while(1){
        if(restart_game){
            restart_game = 0;
            G8RTOS_KillSelf();
        }
        G8RTOS_WaitSemaphore(&sem_I2CA);
        x_data = BMI160_AccelXGetResult();
        y_data = BMI160_AccelYGetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);
        G8RTOS_WriteFIFO(IMU_FIFO, x_data);
        G8RTOS_WriteFIFO(IMU_FIFO, y_data);
        sleep(13);
    }
}

void board1(void){
    for(int i = 10; i < 80; i++){
        for(int j = 30; j < 50; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 60; i < 80; i++){
            for(int j = 30; j < 90; j++){
                gameboard[j][i] = 1;
            }
        }
    for(int i = 80; i < 160; i++){
                for(int j = 70; j < 90; j++){
                    gameboard[j][i] = 1;
                }
            }
    for(int i = 160; i < 190; i++){
            for(int j = 70; j < 90; j++){
                gameboard[j][i] = 4;
            }
        }
    IBit_State = StartCriticalSection();
    uint16_t printed_color = ST7789_BLACK;
    for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 200; j++) {
                if(gameboard[i][j]==0){
                    printed_color = ST7789_BLACK;
                } else if(gameboard[i][j]==1){
                    printed_color = 0xFFFF;
                } else if(gameboard[i][j]==4){
                    printed_color = ST7789_GREEN;
                } else{
                    printed_color = ST7789_RED;
                }
                ST7789_DrawPixel(20 + j,110 + i, printed_color);
            }
    }
    EndCriticalSection(IBit_State);
    level_loaded = 1;
    x_coord = 20;
    y_coord = 40;
    x_coord_old = 20;
    y_coord_old = 40;
    y_velocity_initial = 0;
    y_velocity_initial = 0;
    x_velocity = 0;
    y_velocity = 0;
    G8RTOS_KillSelf();
}

void board0(void){
    for(int i = 10; i < 160; i++){
        for(int j = 10; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 160; i < 190; i++){
            for(int j = 10; j < 90; j++){
                gameboard[j][i] = 4;
            }
        }
    IBit_State = StartCriticalSection();
    uint16_t printed_color = ST7789_BLACK;
    for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 200; j++) {
                if(gameboard[i][j]==0){
                    printed_color = ST7789_BLACK;
                } else if(gameboard[i][j]==1){
                    printed_color = 0xFFFF;
                } else if(gameboard[i][j]==4){
                    printed_color = ST7789_GREEN;
                } else{
                    printed_color = ST7789_RED;
                }
                ST7789_DrawPixel(20 + j,110 + i, printed_color);
            }
    }
    EndCriticalSection(IBit_State);
    level_loaded = 1;
    x_coord = 20;
    y_coord = 50;
    x_coord_old = 20;
    y_coord_old = 50;
    y_velocity_initial = 0;
    y_velocity_initial = 0;
    x_velocity = 0;
    y_velocity = 0;
    G8RTOS_KillSelf();
}

void board3(void){

    for(int i = 170; i < 190; i++){
        for(int j = 10; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 140; i < 170; i++){
        for(int j = 70; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 140; i < 160; i++){
        for(int j = 10; j < 70; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 110; i < 140; i++){
        for(int j = 10; j < 30; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 110; i < 130; i++){
        for(int j = 30; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 80; i < 110; i++){
        for(int j = 70; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 80; i < 100; i++){
        for(int j = 10; j < 70; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 50; i < 80; i++){
        for(int j = 10; j < 30; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 50; i < 70; i++){
        for(int j = 30; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 50; i < 70; i++){
        for(int j = 30; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 20; i < 50; i++){
        for(int j = 70; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 20; i < 40; i++){
        for(int j = 30; j < 70; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 20; i < 40; i++){
        for(int j = 10; j < 30; j++){
            gameboard[j][i] = 4;
        }
    }
    IBit_State = StartCriticalSection();
    uint16_t printed_color = ST7789_BLACK;
    for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 200; j++) {
                if(gameboard[i][j]==0){
                    printed_color = ST7789_BLACK;
                } else if(gameboard[i][j]==1){
                    printed_color = 0xFFFF;
                } else if(gameboard[i][j]==4){
                    printed_color = ST7789_GREEN;
                } else{
                    printed_color = ST7789_RED;
                }
                ST7789_DrawPixel(20 + j,110 + i, printed_color);
            }
    }
    EndCriticalSection(IBit_State);
    level_loaded = 1;
    x_coord = 180;
    y_coord = 20;
    x_coord_old = 180;
    y_coord_old = 20;
    y_velocity_initial = 0;
    y_velocity_initial = 0;
    x_velocity = 0;
    y_velocity = 0;
    G8RTOS_KillSelf();
}

void board2(void){
    for(int i = 10; i < 30; i++){
        for(int j = 10; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 30; i < 190; i++){
        for(int j = 70; j < 90; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 170; i < 190; i++){
        for(int j = 10; j < 70; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 40; i < 170; i++){
        for(int j = 10; j < 30; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 40; i < 60; i++){
        for(int j = 30; j < 60; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 60; i < 140; i++){
        for(int j = 40; j < 60; j++){
            gameboard[j][i] = 1;
        }
    }
    for(int i = 140; i < 160; i++){
        for(int j = 40; j < 60; j++){
            gameboard[j][i] = 4;
        }
    }


    IBit_State = StartCriticalSection();
    uint16_t printed_color = ST7789_BLACK;
    for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 200; j++) {
                if(gameboard[i][j]==0){
                    printed_color = ST7789_BLACK;
                } else if(gameboard[i][j]==1){
                    printed_color = 0xFFFF;
                } else if(gameboard[i][j]==4){
                    printed_color = ST7789_GREEN;
                } else{
                    printed_color = ST7789_RED;
                }
                ST7789_DrawPixel(20 + j,110 + i, printed_color);
            }
    }
    EndCriticalSection(IBit_State);
    level_loaded = 1;
    x_coord = 20;
    y_coord = 20;
    x_coord_old = 20;
    y_coord_old = 20;
    y_velocity_initial = 0;
    y_velocity_initial = 0;
    x_velocity = 0;
    y_velocity = 0;
    G8RTOS_KillSelf();
}

void Read_Button(){
    uint8_t button_outputs;
    while(1){
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
        sleep(10);
        //G8RTOS_WaitSemaphore(&sem_I2CA);
        button_outputs = MultimodButtons_Get();
        //G8RTOS_SignalSemaphore(&sem_I2CA);
        if((button_outputs & SW1) != SW1){
            if(game_hasnt_started){
                G8RTOS_AddThread(board0, 250, "board");
                G8RTOS_AddThread(player, 251, "player");
                G8RTOS_AddThread(read_imu, 252, "read_imu");

                game_hasnt_started = 0;
            }
        }
        else if((button_outputs & SW2) != SW2){
            if(game_hasnt_started){
                G8RTOS_AddThread(board1, 250, "board");
                G8RTOS_AddThread(player, 251, "player");
                G8RTOS_AddThread(read_imu, 252, "read_imu");
                game_hasnt_started = 0;
            }
        }
        else if((button_outputs & SW3) != SW3){
            if(game_hasnt_started){
                G8RTOS_AddThread(board2, 250, "board");
                G8RTOS_AddThread(player, 251, "player");
                G8RTOS_AddThread(read_imu, 252, "read_imu");
                game_hasnt_started = 0;
            }
        }
        else if((button_outputs & SW4) != SW4){
            if(game_hasnt_started){
                G8RTOS_AddThread(board3, 250, "board");
                G8RTOS_AddThread(player, 251, "player");
                G8RTOS_AddThread(read_imu, 252, "read_imu");
                game_hasnt_started = 0;
            }
        }
        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    }
}
//*******************************Periodic Threads**********************************
void draw_canvas(){
    if(game_state == 0){
    //Updates screen
    uint16_t printed_color = ST7789_BLACK;
   /* for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 200; j++) {
            if(gameboard[i][j]==0){
                printed_color = ST7789_BLACK;
            } else if(gameboard[i][j]==1){
                printed_color = 0xFFFF;
            } else if(gameboard[i][j]==4){
                printed_color = ST7789_GREEN;
            } else{
                printed_color = ST7789_RED;
            }
            ST7789_DrawPixel(20 + j,120 + i, printed_color);
        }
    }*/
    if(level_loaded){
    for (int i = y_coord_old-10; i < y_coord_old+10; i++) {
            for (int j = x_coord_old-10; j < x_coord_old+10; j++) {
                if(gameboard[i][j]==0){
                    printed_color = ST7789_BLACK;
                } else if(gameboard[i][j]==1){
                    printed_color = 0xFFFF;
                } else if(gameboard[i][j]==4){
                    printed_color = ST7789_GREEN;
                } else{
                    printed_color = ST7789_RED;
                }
                ST7789_DrawPixel(20 + j,110 + i, printed_color);
            }
        }
    for (int i = y_coord-2; i < y_coord+3; i++) {
            for (int j = x_coord-2; j < x_coord+3; j++) {
                if(gameboard[i][j]==0){
                        printed_color = ST7789_BLACK;
                } else if(gameboard[i][j]==1){
                    printed_color = 0xFFFF;
                } else if(gameboard[i][j]==4){
                    printed_color = ST7789_GREEN;
                } else{
                    printed_color = ST7789_RED;
                }
                ST7789_DrawPixel(20 + j,110 + i, printed_color);
            }
        }
    }
    }
    else if(game_state == 1){
        ST7789_Fill(ST7789_RED);
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 200; j++) {
                gameboard[i][j] = 0;
            }
        }
        SysCtlDelay(8000000);
        ST7789_Fill(0b1111110100100000);
        game_state = 0;
        level_loaded = 0;
        game_hasnt_started = 1;
    }
    else if(game_state == 2){
        ST7789_Fill(ST7789_GREEN);
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 200; j++) {
                gameboard[i][j] = 0;
            }
        }
        SysCtlDelay(8000000);
        ST7789_Fill(0b1111110100100000);
        game_state = 0;
        level_loaded = 0;
        game_hasnt_started = 1;
    }
}
//******************************Aperiodic Threads**********************************


void start_game(){
    GPIOIntDisable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}


