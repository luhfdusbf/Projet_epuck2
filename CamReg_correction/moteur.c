#include <main.h>
#include <moteur.h>
#include <distance.h>

#define TURN_SPEED 175
#define FOREWARD_SPEED 350

static bool foreward = 0;

static THD_WORKING_AREA(waStop_detection, 256);
static THD_FUNCTION(Stop_detection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    while(1){
    	if((get_distance() < DISTANCE_TRESHOLD)&&(foreward)){
    		stop();
    	}
    	chThdSleepMilliseconds(100);
    }
}

void stop_detection_start(void){
	chThdCreateStatic(waStop_detection, sizeof(waStop_detection), NORMALPRIO+1, Stop_detection, NULL);
}

void go_foreward(){
	foreward = 1;
	left_motor_set_speed(FOREWARD_SPEED);
	right_motor_set_speed(FOREWARD_SPEED);
}

void stop(){
	foreward = 0;
	left_motor_set_speed(0);
	right_motor_set_speed(0);
}

void turn(int8_t DIRECTION){
	foreward = 0;
	if(DIRECTION == LEFT){
		left_motor_set_speed(-TURN_SPEED);
		right_motor_set_speed(TURN_SPEED); }
	else if(DIRECTION == RIGHT){
		left_motor_set_speed(TURN_SPEED);
		right_motor_set_speed(-TURN_SPEED); }
	else{
		stop();
	}
}

