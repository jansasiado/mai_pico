/*
PSoC 4-Based touch controller receiver
emulates MPR121
*/
#include <stdint.h>
#include <string.h>
#include "psoc.h"
#include "hardware/i2c.h"

#include "board_defs.h"

void write_reg(uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t buf[] = {reg, val};
    i2c_write_blocking_until(I2C_PORT, addr, buf, 2, false,
                             time_us_64() + IO_TIMEOUT_US);
}

void write_reg_16(uint8_t addr, uint8_t reg, uint16_t val)
{
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (val & 0xFF00) >> 8;
    buf[2] = val & 0xFF;
    i2c_write_blocking_until(I2C_PORT, addr, buf, 3, false,
                             time_us_64() + IO_TIMEOUT_US);
}

uint8_t read_reg(uint8_t addr, uint8_t reg)
{
    uint8_t value;
    i2c_write_blocking_until(I2C_PORT, addr, &reg, 1, true,
                             time_us_64() + IO_TIMEOUT_US);
    i2c_read_blocking_until(I2C_PORT, addr, &value, 1, false,
                            time_us_64() + IO_TIMEOUT_US);
    return value;
}

uint16_t read_reg_16(uint8_t addr, uint8_t reg)
{
    uint8_t value[2];
    i2c_write_blocking_until(I2C_PORT, addr, &reg, 1, true,
                             time_us_64() + IO_TIMEOUT_US);
    i2c_read_blocking_until(I2C_PORT, addr, value, 2, false,
                            time_us_64() + IO_TIMEOUT_US);
    return (uint16_t) value[1]<<8 | value[0];
}

bool psoc_read_many_16(uint8_t addr, uint8_t reg, uint16_t *buf, uint8_t num)
{
    uint8_t vals[24] = {0};
    uint16_t val;
    i2c_write_blocking_until(I2C_PORT, addr, &reg, 1, true,
                             time_us_64() + IO_TIMEOUT_US);
    uint8_t b = i2c_read_blocking_until(I2C_PORT, addr, vals, 2*num, false,
                            time_us_64() + IO_TIMEOUT_US);
    for(uint8_t i = 0; i < 12; i++){
        val = ((vals[2*i] << 8) | vals[2*i+1]);
        buf[i] = val;
    }

    return b==num*2;
}

uint16_t psoc_touched(uint8_t addr){
    return read_reg_16(addr, PSOC_TOUCH_STATUS_REG);
}

bool psoc_raw(uint8_t addr, uint16_t *buf, uint8_t num){
    write_reg(addr, PSOC_READ_CMD_REG, 1<<0);
    return psoc_read_many_16(addr, PSOC_RAW_VALUE_REG_START, buf, num);
}

bool psoc_baseline(uint8_t addr, uint16_t *buf, uint8_t num){
    return psoc_read_many_16(addr, PSOC_BASELINE_REG_START, buf, num);
}

uint8_t psoc_get_idac(uint8_t addr, uint8_t sns){
    uint8_t buf = 0;
    write_reg(addr, PSOC_IDAC_COMP_N_REG, sns);
    write_reg(addr, PSOC_READ_CMD_REG, 1<<2);
    buf = read_reg(addr, PSOC_IDAC_COMP_VAL_REG);
    return buf;
}

void psoc_set_idac(uint8_t addr, uint8_t sns, uint8_t idac){
    if(idac>127){
        idac = 127;
    }
    write_reg(addr, PSOC_IDAC_COMP_N_REG, sns);
    write_reg(addr, PSOC_IDAC_COMP_VAL_REG, idac);
    uint8_t cmd = read_reg(addr, PSOC_CMD_REG);
    write_reg(addr, PSOC_CMD_REG, cmd | 1<<7);
}

void psoc_init(uint8_t i2c_addr){
    write_reg(i2c_addr, PSOC_CMD_REG, 1); // reset
}

void psoc_set_finger_threshold(uint8_t i2c_addr, uint16_t ft){
    write_reg_16(i2c_addr, PSOC_FINGER_THRESHOLD_REG, ft);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<1);
}

void psoc_set_noise_threshold(uint8_t i2c_addr, uint16_t nt){
    write_reg_16(i2c_addr, PSOC_NOISE_THRESHOLD_REG, nt);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<2);
}

void psoc_set_neg_noise_threshold(uint8_t i2c_addr, uint16_t nnt){
    write_reg_16(i2c_addr, PSOC_NEG_NOISE_THRESHOLD_REG, nnt);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<3);
}

void psoc_set_low_baseline_reset(uint8_t i2c_addr, uint8_t lbr){
    write_reg(i2c_addr, PSOC_LOW_BASELINE_RESET_REG, lbr);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<4);
}

void psoc_set_hysteresis(uint8_t i2c_addr, uint8_t h){
    write_reg(i2c_addr, PSOC_HYSTERESIS_REG, h);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<5);
}

void psoc_set_on_debounce(uint8_t i2c_addr, uint8_t od){
    write_reg(i2c_addr, PSOC_ON_DEBOUNCE_REG, od);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<6);
}

void psoc_set_idac_comp(uint8_t i2c_addr, uint8_t n, int8_t val){
    write_reg(i2c_addr, PSOC_IDAC_COMP_N_REG, n);
    write_reg(i2c_addr, PSOC_IDAC_COMP_VAL_REG, val);
    uint8_t cmd = read_reg(i2c_addr, PSOC_CMD_REG);
    write_reg(i2c_addr, PSOC_CMD_REG, cmd | 1<<7);
}
