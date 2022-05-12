#include <main.h>
#include <distance.h>


static int16_t distance = 100; //millimeters

static THD_WORKING_AREA(waDistance, 256);
static THD_FUNCTION(Distance, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
    	distance = VL53L0X_get_dist_mm();
    	chThdSleepMilliseconds(100);
    }
}

void distance_start(void){
	chThdCreateStatic(waDistance, sizeof(waDistance), NORMALPRIO, Distance, NULL);
}

int16_t get_distance(){
	return distance;
}
