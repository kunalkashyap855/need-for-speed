#include "gyroDriver.h"
#include <mbed.h>

using namespace L3GD20Gyro;

L3GD20::L3GD20(PinName mosi, PinName miso, PinName sclk, PinName ssel) {
    spi_ptr = new SPI(mosi, miso, sclk);

    spi_ptr->frequency(1000000);
    spi_ptr->format(8, 3);

    _spi_gyro_cs = new DigitalOut(ssel, 1);

    // Setting L3GD20 to 'Normal' mode
    _spi_gyro_cs->write(0);
    spi_ptr->write(CTRL_REG1_ADDR);
    spi_ptr->write(0xF);
    _spi_gyro_cs->write(1);

    // Setting L3GD20 Scale to +- 2000
    // _spi_gyro_cs->write(0);
    // spi_ptr->write(CTRL_REG4_ADDR);
    // spi_ptr->write(0x20);
    // _spi_gyro_cs->write(1);
}

int8_t L3GD20::read_register(int8_t reg) {
    reg = reg | 0x80;
    _spi_gyro_cs->write(0);

    spi_ptr->write(reg);
    int8_t curr_data = spi_ptr->write(0x00); 
    _spi_gyro_cs->write(1);

    return curr_data;
}

void L3GD20::write_register(int8_t reg, int8_t data) {
    reg = reg | 0x7F;
    _spi_gyro_cs->write(0);

    spi_ptr->write(reg);
    spi_ptr->write(data); 

    _spi_gyro_cs->write(1);
}