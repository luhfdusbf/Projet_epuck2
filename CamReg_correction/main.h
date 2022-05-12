#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/***includes used in all the project***/
#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"
#include <camera/po8030.h>
#include "ch.h"
#include "hal.h"
#include <stdio.h>
#include <motors.h>
#include <i2c_bus.h>
#include "sensors\VL53L0X\VL53L0X.h"

/***constants for the differents parts of the project***/
#define COLOR_MARGIN	2000	//margin over 19200
#define DISTANCE_TRESHOLD 	40
#define NB_STEP_EACH_TURN	4

/***conventions for all the project***/
#define BLACK		0		//no significant color
#define RED			1
#define GREEN		2
#define BLUE		3
#define RIGHT 		0
#define LEFT		1

/** Robot wide IPC bus. */
extern messagebus_t bus;

extern parameter_namespace_t parameter_root;

void SendUint8ToComputer(uint8_t* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif
