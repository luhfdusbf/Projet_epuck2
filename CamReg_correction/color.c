#include <main.h>
#include <color.h>

#define NB_POINTS		300

static uint8_t color = BLACK;

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes 1/4 pixels at RGB565, in a band of 20 by 240 pixels
    po8030_advanced_config(FORMAT_RGB565,310,0, 20,240, SUBSAMPLING_X4, SUBSAMPLING_X4);
    //disable auto white balance adjustment
    po8030_set_awb(0);
    //change gains for every color to have equal measurement with white light
    po8030_set_rgb_gain(0x4A, 0x45, 0x5D);

    //preparations to take one picture
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();
    while(TRUE){
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
			//5 first bits (>>3)*2,makes RGB values comparable on 6 bits
			red[i/2] = ((img_buff_ptr[i]&0b11111000) >> 2);
			//3 bits on the first ptr byte and 3 on the second one
			green[i/2] = ((img_buff_ptr[i]&0b00000111)<<3)+(((img_buff_ptr[i+1]&0b11100000)>>5));
			//5 last bits
			blue[i/2] = (img_buff_ptr[i+1]&0b00011111) * 2;
		}
		//sum on every pixels
		uint32_t sum_red = 0;
		uint32_t sum_green = 0;
		uint32_t sum_blue = 0;
		for(uint16_t i = 0 ; i < (NB_POINTS) ; i++){
				sum_red += red[i];
				sum_green += green[i];
				sum_blue +=  blue[i];
		}
		//identification of the most significant color, with an COLOR_MARGIN margin
		if((sum_red>(sum_blue+COLOR_MARGIN))&&((sum_green+COLOR_MARGIN)<sum_red)){
			color = RED;
		}
		else if(((sum_red+COLOR_MARGIN)<sum_green)&&(sum_green>(sum_blue+COLOR_MARGIN))){
			color = GREEN;
		}
		else if(((sum_red+COLOR_MARGIN)<sum_blue)&&((sum_green+COLOR_MARGIN)<sum_blue)){
			color = BLUE;
		}
		else{
			color = BLACK;
		}
    }
}

void color_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}

uint8_t get_color(void){
	return color;
}
