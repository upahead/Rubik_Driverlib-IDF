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
  * @file           axp192_reg.c
  * @version        1.0
  * @date           2021-6-7
  */

#ifndef __AXP192_REG_H__
#define __AXP192_REG_H__

// Power Control Class
#define AXP192_INPUT_POWER_STATUS_REG           (0x00)   // Power status register.
#define AXP192_POWER_MODE_CHARGE_STATUS_REG     (0x01)   // Power mode/charge status register.
#define AXP192_EXTEN_DCDC2_OUTPUT_CONTROL_REG   (0x10)   // EXTEN & DC-DC2 switch control register.
#define AXP192_POWER_OUTPUT_REG                 (0x12)   // DC-DC1/3 & LDO2/3 switch control register.
#define AXP192_DCDC3_OUTPUT_VOLUME_SET_REG      (0x27)   // DC-DC3 voltage setting register.
#define AXP192_CHARGE_CONTROL_REG_1             (0x33)   // Charge control register 1.
#define AXP192_BACKUP_CHARGE_CONTROL_REG        (0x35)   // Backup battery charge control register.
#define AXP192_ADC_ENABLE_CONTROL_REG_1         (0x82)   // ADC enable setting register 1.
#define AXP192_ADC_ENABLE_CONTROL_REG_2         (0x83)   // ADC enable setting register 2.

// GPIO control class

// Interrupt control class

// ADC data classes
#define AXP192_ACIN_VAL_ADC_REG                 (0x56)   // ACIN voltage ADC data high 8 bits.
#define AXP192_VBUS_VAL_ADC_REG                 (0x5A)   // VBUS voltage ADC data high 8 bits.
#define AXP192_INTERNAL_TEMPERATURE_ADC_REG     (0x5E)   // AXP192 internal temperature monitoring ADC data high 8 bits.
#define AXP192_BATTERY_VAL_ADC_REG              (0x78)   // Battery voltage high 8 bits.

#endif /* __AXP192_REG_H__ */