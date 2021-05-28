#ifndef __MEM_CTL_TABLE_H
#define __MEM_CTL_TABLE_H
#include "stm8s.h"


#define MEM_CTL_TABLE_SIZE           66
#define MEM_CTL_TABLE_ROM_END_ADDR   33
#define MEM_CTL_TABLE_RAM_START_ADDR 40
#define SERVO_REG_BUF_SIZE           30

#define YEAR_OF_MANUFACTURE      0x00
#define MONTH_OF_MANUFACTURE     0x01
#define DAY_OF_MANUFACTURE       0x02
#define SOFTWARE_VERSION_2BYTES  0x03 
#define ID                       0x05
                                 
#define MIN_ANGLE_LIMIT_2BYTES   0x09
#define MAX_ANGLE_LIMIT_2BYTES   0x0B // 11
#define MID_ANGLE_2BYTES         0x14 // 20
#define MOTOR_RUN_DIRECTION      0x16 // 22

#define MAX_CURRENT_LIMIT_2BYTES 0x10 // 16

#define TOP_RESOLUTION_2BYTES    0x17 // 23
#define BOTTOM_RESOLUTION_2BYTES 0x1F // 31

#define MIN_ADC_LIMIT_2BYTES     0x19 // 25

#define MAX_ADC_LIMIT_2BYTES     0x1B // 27

#define MIDDLE_ADC_VALUE_2BYTES  0x1D // 29

#define MOTOR_INSTALL_DIRECTION  0x21 // 


#define MOTOR_STOP               0x28 // 40
#define MOTOR_BREAK              0x29 // 41

#define TARGET_POSITION_2BYTES   0x2A
#define RUN_TIME_2BYTES          0x2C
#define THE_CURRENT_2BYTES       0x2E

#define CURRENT_POSITION_2BYTES  0x38

#define CURRENT_SPEED_2BYTES     0x3A


extern uint8_t mem_ctl_table[MEM_CTL_TABLE_SIZE];
extern uint8_t mem_ctl_table_cache[MEM_CTL_TABLE_SIZE];

void servo_mem_ctl_table_init(void);

#endif