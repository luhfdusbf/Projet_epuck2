#include <main.h>
#include <moteur.h>
#include <distance.h>


#define DISTANCE_BETWEEN_WEELS 52 //measured on the robot
#define DISTANCE_PER_STEP 0.13 //find in the wiki
#define FREQUENCY 0.5 //in Hz
#define TURN_SPEED ((DISTANCE_BETWEEN_WEELS*3.14/4)/DISTANCE_PER_STEP*FREQUENCY) //in step per second
#define FOREWARD_SPEED (CASE_SIZE/DISTANCE_PER_STEP*FREQUENCY) //in step per second

static bool foreward = 0;

static THD_WORKING_AREA(waStop_detection, 256);
static THD_FUNCTION(Stop_detection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    while(1){
    	if((get_distance() < DISTANCE_TRESHOLD)&&(foreward)){ //if there is a wall and the robot is going foreward
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

