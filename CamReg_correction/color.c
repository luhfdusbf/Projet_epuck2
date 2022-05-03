#include "ch.h"
#include "hal.h"
//#include <chprintf.h>
//#include <usbcfg.h>

#include <main.h>
#include <camera/po8030.h>

#include <color.h>


#define EPSILON			2000
#define NB_POINTS		300
#define BLACK		0
#define RED			1
#define GREEN		2
#define BLUE		3

static uint8_t color = BLACK;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);


static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	//po8030_advanced_config(FORMAT_RGB565, 0, 10, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
    po8030_advanced_config(FORMAT_RGB565,310,0, 20,240, SUBSAMPLING_X4, SUBSAMPLING_X4);
    po8030_set_awb(0);
    po8030_set_rgb_gain(0x4A, 0x45, 0x5D); //ajuste les gains pour valeur nominale RGB egales
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}


static THD_WORKING_AREA(waProcessImage, 2048);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t red[NB_POINTS] = {0};
	uint8_t green[NB_POINTS] = {0};
	uint8_t blue[NB_POINTS] = {0};

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565    
		img_buff_ptr = dcmi_get_last_image_ptr();


		//Extracts pixels
		for(uint16_t i = 0 ; i < (2 * NB_POINTS) ; i+=2){
			red[i/2] = ((img_buff_ptr[i]&0b11111000) >> 2);//5premiers bit >>3 *2
			green[i/2] = ((img_buff_ptr[i]&0b00000111)<<3)+(((img_buff_ptr[i+1]&0b11100000)>>5));//3bits sur premier byte et 3 sur le 2e
			blue[i/2] = (img_buff_ptr[i+1]&0b00011111) * 2;//5 derniers bits
		}

		uint32_t sum_red = 0;
		uint32_t sum_green = 0;
		uint32_t sum_blue = 0;
		for(uint16_t i = 0 ; i < (NB_POINTS) ; i++){
				sum_red +=(uint32_t) red[i];
				sum_green += (uint32_t)green[i];
				sum_blue += (uint32_t)blue[i];
		}
		//Moyenne pas necessaire

		if((sum_red>(sum_blue+EPSILON))&&((sum_green+EPSILON)<sum_red)){		//vert
			color = RED;
			/*palClearPad(GPIOD, GPIOD_LED1);
			palSetPad(GPIOD, GPIOD_LED3);
			palSetPad(GPIOD, GPIOD_LED5);
			palSetPad(GPIOD, GPIOD_LED7);*/
		}
		else if(((sum_red+EPSILON)<sum_green)&&(sum_green>(sum_blue+EPSILON))){		//rouge
			color = GREEN;
			/*palSetPad(GPIOD, GPIOD_LED1);
			palClearPad(GPIOD, GPIOD_LED3);
			palSetPad(GPIOD, GPIOD_LED5);
			palSetPad(GPIOD, GPIOD_LED7);*/
		}
		else if(((sum_red+EPSILON)<sum_blue)&&((sum_green+EPSILON)<sum_blue)){		//bleu
			color = BLUE;
			/*palSetPad(GPIOD, GPIOD_LED1);
			palSetPad(GPIOD, GPIOD_LED3);
			palClearPad(GPIOD, GPIOD_LED5);
			palSetPad(GPIOD, GPIOD_LED7);*/
		}
		else{		//noir
			color = BLACK;
			/*palSetPad(GPIOD, GPIOD_LED1);
			palSetPad(GPIOD, GPIOD_LED3);
			palSetPad(GPIOD, GPIOD_LED5);
			palClearPad(GPIOD, GPIOD_LED7);*/
		}
		//chprintf((BaseSequentialStream *)&SD3, "%R=%d G=%d B=%d \r\n\n",
        //        sum_red, sum_green, sum_blue);
    }
}
uint8_t get_color(void){
	return color;
}


void color_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}
