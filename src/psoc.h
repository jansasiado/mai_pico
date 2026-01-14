/*
PSoC 4-Based touch controller receiver
emulates MPR121
*/
#include <stdbool.h>
#include <stdint.h>
#include "board_defs.h"

#define PSOC_BASE_ADDR 0x5A
#define IO_TIMEOUT_US 2000

#define PSOC_TOUCH_STATUS_REG 0x0
#define PSOC_RAW_VALUE_REG_START 0x02
#define PSOC_BASELINE_REG_START 0x1A

#define PSOC_FINGER_THRESHOLD_REG 0x32
#define PSOC_NOISE_THRESHOLD_REG 0x34
#define PSOC_NEG_NOISE_THRESHOLD_REG 0x36   
#define PSOC_LOW_BASELINE_RESET_REG 0x38
#define PSOC_HYSTERESIS_REG 0x39
#define PSOC_ON_DEBOUNCE_REG 0x40
#define PSOC_IDAC_COMP_N_REG 0x41
#define PSOC_IDAC_COMP_VAL_REG 0x42
#define PSOC_READ_CMD_REG 0x4E
#define PSOC_CMD_REG 0x4F


void write_reg(uint8_t addr, uint8_t reg, uint8_t val);
void write_reg_16(uint8_t addr, uint8_t reg, uint16_t val);
uint8_t read_reg(uint8_t addr, uint8_t reg);
uint16_t read_reg_16(uint8_t addr, uint8_t reg);


void psoc_init(uint8_t i2c_addr);
uint16_t psoc_touched(uint8_t addr);
bool psoc_raw(uint8_t addr, uint16_t *buf, uint8_t num);
bool psoc_baseline(uint8_t addr, uint16_t *buf, uint8_t num);
uint8_t psoc_get_idac(uint8_t addr, uint8_t sns);
void psoc_set_idac(uint8_t addr, uint8_t sns, uint8_t idac);
void psoc_set_finger_threshold(uint8_t i2c_addr, uint16_t ft);
void psoc_set_noise_threshold(uint8_t i2c_addr, uint16_t nt);
void psoc_set_neg_noise_threshold(uint8_t i2c_addr, uint16_t nnt);
void psoc_set_low_baseline_reset(uint8_t i2c_addr, uint8_t lbr);
void psoc_set_hysteresis(uint8_t i2c_addr, uint8_t h);
void psoc_set_on_debounce(uint8_t i2c_addr, uint8_t od);


