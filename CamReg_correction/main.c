#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>

#include <color.h>

#include <distance.h>
#include <moteur.h>
#include <i2c_bus.h>
#include "msgbus\messagebus.h"
#include "motors.h"
#include "sensors\proximity.h"
#include "sensors\VL53L0X\VL53L0X.h"
#include <chmtx.h>
#include "leds.h"

#define BLACK		0
#define RED			1
#define GREEN		2
#define BLUE		3
#define DISTANCE_TRESHOLD 40
#define NB_STEP_EACH_TURN 4


         void set_leds(){
         set_led(LED3,1);
         set_led(LED5,1);
         set_led(LED7,1); }

         void clearr_leds(){
         set_led(LED3,0);
         set_led(LED5,0);
         set_led(LED7,0); }
          #define RIGHT 0
          #define LEFT 1

/*void SendUint8ToComputer(uint8_t* data, uint16_t size)
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg);
}*/// UART3.

int main(void)
{
    halInit();
    chSysInit();
    mpu_init();

    i2c_start();
    VL53L0X_start();
    //starts the serial communication
    //serial_start();
    //start the USB communication
    // usb_start();
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();

	distance_start();


	//stars the threads for the pi regulator and the processing of the image
	color_start();

	bool wall;
	uint8_t color;
	int8_t step_left = NB_STEP_EACH_TURN;
    /* Infinite loop. */
    while (1) {
    	//waits 2 second
    	 chThdSleepMilliseconds(2000);
        wall = (get_distance() < DISTANCE_TRESHOLD); //true if there is a wall
        color = get_color();

                  if(wall && color == GREEN){
                  		clearr_leds();
                  		turn(RIGHT);
                  		step_left = NB_STEP_EACH_TURN;
                  	}
                  else if(wall && color == RED){
                  		clearr_leds();
                  		turn(LEFT);
                  		step_left = NB_STEP_EACH_TURN;
                  	}
                  else if(wall && color == BLUE){
                  		//END GAME
                  		stop();
                  		set_leds();
                  		set_body_led(1);
                  		while(1){
                  			 chThdSleepMilliseconds(1000);
                  		}
                  	}
                  else if(!wall && step_left > 0){
                  		go_foreward();
                  		step_left = step_left - 1;
                  	}
                  else if(step_left == 0){
                  		//NEXT PLAYER TURN
                	    stop();
                  		set_leds();
                  	}
                 //set_body_led(2);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
