#include <main.h>
#include <distance.h>
#include <moteur.h>
#include <color.h>

#include "ch.h"
#include "hal.h"

#include "memory_protection.h"
#include "leds.h"


void leds_on(void){
	set_led(LED3,1);
	set_led(LED5,1);
	set_led(LED7,1);
}

void leds_off(void){
	set_led(LED3,0);
	set_led(LED5,0);
	set_led(LED7,0);
}

int main(void)
{
    halInit();
    chSysInit();
    mpu_init();

    //enable i2c communication
    i2c_start();
    //enable distance detector
    VL53L0X_start();

    //start the camera
    dcmi_start();
	po8030_start();
	//initialyse the motors
	motors_init();
	//stars the threads (distance,color measure, and anti-colision control)
	distance_start();
	color_start();
	stop_detection_start();

	//global variables initialisation
	bool wall_detected=FALSE;
	uint8_t color=BLACK;
	int8_t step_left = NB_STEP_EACH_TURN;

    /* Infinite loop. */
    while (TRUE) {
    	//waits 2 second
    	chThdSleepMilliseconds(2000);

        wall_detected = (get_distance() < DISTANCE_TRESHOLD); //true if there is a wall_detected
        color = get_color();

        if(wall_detected && (color == RED)){
        	leds_off();
        	turn(RIGHT);
        	step_left = NB_STEP_EACH_TURN;
        }
        else if(wall_detected && (color == GREEN)){
        	leds_off();
        	turn(LEFT);
        	step_left = NB_STEP_EACH_TURN;
        }
        else if(wall_detected && (color == BLUE)){
        	//END GAME
        	stop();
        	leds_on();
        	set_body_led(1);
        	while(TRUE){	//infinite loop
        		leds_off();
        		chThdSleepMilliseconds(1000);
        		leds_on();
        		chThdSleepMilliseconds(1000);
        	}
        }
        else if(!wall_detected && (step_left > 0)){
        	//FOREWARD
        	go_foreward();
        	step_left = step_left - 1;
        }
        else{
        	//NEXT PLAYER TURN/BLACK WALL DETECTED
        	step_left = 0;
            stop();
        	leds_on();
        }
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
