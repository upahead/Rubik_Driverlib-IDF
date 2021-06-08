/*****************************************************************************
 *                                                                           *
 *  Copyright 2021 upahead PTE LTD                                           *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *      http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/
/**
  * @file           ads1115_reg.h
  * @version        1.0
  * @date           2021-6-8
  */

#ifndef __ADS1115_REG_H__
#define __ADS1115_REG_H__  

// Address pointer register(W)
#define ADS1115_POINTER_CONVERT_REG           (0x00) // Conversion register(R)
#define ADS1115_POINTER_CONFIG_REG            (0x01) // Configuration register(RW)
#define ADS1115_POINTER_LOTHRESH_REG          (0x02) // Threshold comparator low byte(RW)
#define ADS1115_POINTER_HITHRESH_REG          (0x03) // Threshold comparator high byte(RW)

// Configuration register(0x01)：
/* CRH[15:8]
   BIT      15      14      13      12      11      10      9       8
   NAME     OS      MUX2    MUX1    MUX0    PGA2    PGA1    PGA0    MODE
   CRL[7:0]
   BIT      7       6       5       4       3       2       1       0
   NAME    DR0     DR1     DR0   COMP_M  COMP_P  COMP_L  COMP_Q1  COMP_Q0
*/
// OS CRH[7]
// CONFIG register OS Device status read and write.
typedef enum{
    ADS1115_REG_CONFIG_OS_W_SINGLE_START    = (0x01U<<7), // Single conversion start. 
}ADS1115_RegConfigOsW_t;
typedef enum{
    ADS1115_REG_CONFIG_OS_R_CONVERT         = (0x00U<<7), // Conversion in progress.
    ADS1115_REG_CONFIG_OS_R_NOTCONVERT      = (0x01U<<7), // No conversion in progress.(default)
}ADS1115_RegConfigOsR_t;

// MUX CRH[6:4]
// CONFIG register MUX Device channel selection.
typedef enum{
    ADS1115_REG_CONFIG_MUX_DIFF_01          = (0x00U<<4), // Channel 0 and channel 1 differential input.(default)
    ADS1115_REG_CONFIG_MUX_DIFF_03          = (0x01U<<4), // Channel 0 and channel 3 differential input.
    ADS1115_REG_CONFIG_MUX_DIFF_13          = (0x02U<<4), // Channel 1 and channel 3 differential input.
    ADS1115_REG_CONFIG_MUX_DIFF_23          = (0x03U<<4), // Channel 2 and channel 3 differential input.
    ADS1115_REG_CONFIG_MUX_SING_0           = (0x04U<<4), // Channel 0 independent input.
    ADS1115_REG_CONFIG_MUX_SING_1           = (0x05U<<4), // Channel 1 independent input.
    ADS1115_REG_CONFIG_MUX_SING_2           = (0x06U<<4), // Channel 2 independent input.
    ADS1115_REG_CONFIG_MUX_SING_3           = (0x07U<<4), // Channel 3 independent input.
}ADS1115_RegConfigMux_t;

// PGA CRH[3:1]
// CONFIG register PGA Programmable gain amplifier configuration (range selection).
typedef enum{
    ADS1115_REG_CONFIG_PGA_FSR_6114         = (0x00U<<1), // ±6.144 V  
    ADS1115_REG_CONFIG_PGA_FSR_4096         = (0x01U<<1), // ±4.096 V 
    ADS1115_REG_CONFIG_PGA_FSR_2048         = (0x02U<<1), // ±2.048 V(default)
    ADS1115_REG_CONFIG_PGA_FSR_1024         = (0x03U<<1), // ±1.024 V 
    ADS1115_REG_CONFIG_PGA_FSR_0512         = (0x04U<<1), // ±0.512 V 
    ADS1115_REG_CONFIG_PGA_FSR_0256         = (0x05U<<1), // ±0.256 V  
}ADS1115_RegConfigPga_t;

// MODE CRH[0]
// CONFIG register MODE Operation mode selection.
typedef enum{
    ADS1115_REG_CONFIG_MODE_CONT            = (0x00U<<0), // Continuous conversion mode.
    ADS1115_REG_CONFIG_MODE_SING            = (0x01U<<0), // Single conversion mode.(default) 
}ADS1115_RegConfigMode_t;

// DR CRL[7:5]
// CONFIG register DR Conversion rate configuration.
typedef enum{
    ADS1115_REG_CONFIG_DR_SPS_8             = (0x00U<<5), // 8 SPS
    ADS1115_REG_CONFIG_DR_SPS_16            = (0x01U<<5), // 16 SPS
    ADS1115_REG_CONFIG_DR_SPS_32            = (0x02U<<5), // 32 SPS
    ADS1115_REG_CONFIG_DR_SPS_64            = (0x03U<<5), // 64 SPS
    ADS1115_REG_CONFIG_DR_SPS_128           = (0x04U<<5), // 128 SPS(default)
    ADS1115_REG_CONFIG_DR_SPS_250           = (0x05U<<5), // 250 SPS
    ADS1115_REG_CONFIG_DR_SPS_475           = (0x06U<<5), // 475 SPS
    ADS1115_REG_CONFIG_DR_SPS_860           = (0x07U<<5), // 860 SPS
}ADS1115_RegConfigDr_t;

// COMP_MODE CRL[4]
// CONFIG register COMP_MODE Comparator mode configuration.
typedef enum{
    ADS1115_REG_CONFIG_COMPM_TRADITION      = (0x00U<<4), // Traditional comparator.(default)
    ADS1115_REG_CONFIG_COMPM_WINDOW         = (0x01U<<4), // Window comparator.
}ADS1115_RegConfigCompm_t;

// COMP_POL CRL[3]
// CONFIG register COMP_POL Comparator polarity configuration. 
typedef enum{
    ADS1115_REG_CONFIG_COMPP_ACTIVE_LOW     = (0x00U<<3), // Active low.(default)
    ADS1115_REG_CONFIG_COMPP_ACTIVE_HIG     = (0x01U<<3), // Active high.
}ADS1115_RegConfigCompp_t;

// COMP_LAT CRL[2]
// CONFIG register COMP_LAT Lock the comparator configuration.
typedef enum{
    ADS1115_REG_CONFIG_COMPL_NONLATCHING    = (0x00U<<2), // Not locked.(default)
    ADS1115_REG_CONFIG_COMPL_LATCHING       = (0x01U<<2), // locking.
}ADS1115_RegConfigCompl_t;

// COMP_QUE CRL[1:0]
// CONFIG register COMP_QUE Comparator queue and disable.
typedef enum{
    ADS1115_REG_CONFIG_COMPQ_ONE            = (0x00U<<0), // Alarm after one conversion.
    ADS1115_REG_CONFIG_COMPQ_TWO            = (0x01U<<0), // Alarm after two conversions.
    ADS1115_REG_CONFIG_COMPQ_FOUR           = (0x02U<<0), // Alarm after four conversions.
    ADS1115_REG_CONFIG_COMPQ_DISABLE        = (0x03U<<0), // Disable.(default)
}ADS1115_RegConfigCompq_t;

#endif /* __ADS1115_REG_H__ */