#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "i2c_master.h"
#include "paj7620u2_driver.h"

#define PAJ_ADDR     0x73

void app_main(void)
{
    uint16_t gesture_val;

    I2cMaster_handle_t i2c_0 = I2cMaster_Init(I2C_NUM_0, 22, 21, 400000);
    PAJ7620U2_handle_t paj = PAJ7620U2_Init(i2c_0, PAJ_ADDR);

    PAJ7620U2_GestureInit(paj);

    while(1){
        PAJ7620U2_GestureGetState(paj, &gesture_val);
        switch(gesture_val){
            case PAJ7620U2_GESTURE_UP:                   
                printf("Up\r\n");
                break; 
			case PAJ7620U2_GESTURE_DOWM:                  
                printf("Dowm\r\n");
                break; 
			case PAJ7620U2_GESTURE_LEFT:               
  				printf("Left\r\n");
                break; 
			case PAJ7620U2_GESTURE_RIGHT:            
              printf("Right\r\n");
                break; 
			case PAJ7620U2_GESTURE_FORWARD:        
     			printf("Forward\r\n");
                break; 
	    	case PAJ7620U2_GESTURE_BACKWARD:     
            	printf("Backward\r\n");
                  break; 
	    	case PAJ7620U2_GESTURE_CLOCKWISE:  
                printf("Clockwise\r\n");
                break; 
			case PAJ7620U2_GESTURE_COUNT_CLOCKWISE: 
                printf("AntiClockwise\r\n");
                break; 
	    	case PAJ7620U2_GESTURE_WAVE: 
			    printf("Wave\r\n");
                break; 
			default:
                break; 
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
