#include <distance.h>
#include <color.h>
#include <moteur.h>
#include "ch.h"
#include <leds.h>

#define TURN_SPEED 175
#define FOREWARD_SPEED 350
#define LEFT 0
#define RIGHT 1
#define DISTANCE_TRESHOLD 40

static bool wall_detected = 0;

static THD_WORKING_AREA(waStop_detection, 256);
static THD_FUNCTION(Stop_detection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    while(1){
    	if(get_distance() < DISTANCE_TRESHOLD){
    		wall_detected=TRUE;
    	}
    	else{
    		wall_detected=FALSE;
    	}
    	chThdSleepMilliseconds(10);
    }
}

void stop_detection_start(void){
	chThdCreateStatic(waStop_detection, sizeof(waStop_detection), NORMALPRIO+1, Stop_detection, NULL);
}

void go_foreward(){
	if(!wall_detected){
		left_motor_set_speed(FOREWARD_SPEED);
		right_motor_set_speed(FOREWARD_SPEED);
	}
	else{
		stop();
	}
}

void stop(){
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void turn(int8_t DIRECTION){
	if(DIRECTION == RIGHT){
		left_motor_set_speed(-TURN_SPEED);
		right_motor_set_speed(TURN_SPEED); }
	else if(DIRECTION == LEFT){
		left_motor_set_speed(TURN_SPEED);
		right_motor_set_speed(-TURN_SPEED); }
	else{
		stop();
	}
}

