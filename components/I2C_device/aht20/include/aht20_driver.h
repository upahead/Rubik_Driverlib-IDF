#ifndef __AHT20_DRIVER_H__
#define __AHT20_DRIVER_H__

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../i2c_master/include/i2c_master.h"

#define	AHT20_STATUS_REG		        (0x71)
#define	AHT20_INIT_REG			        (0xBE)
#define	AHT20_SoftReset			        (0xBA)
#define	AHT20_TrigMeasure_REG	      (0xAC)

typedef struct{
    I2cMaster_handle_t i2c_handle;
    uint8_t i2c_addr;
}AHT20_t;
typedef AHT20_t *AHT20_handle_t;



#endif /* __AHT20_DRIVER__ */