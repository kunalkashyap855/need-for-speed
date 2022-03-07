#ifndef GYRO_DRIVER
#define GYRO_DRIVER

#include <mbed.h>

#define GYRO_SENSITIVITY_250      8.75	    // 8.75 mdps/digit
#define GYRO_SENSITIVITY_500	  17.5	    // 17.5 mdps/digit
#define GYRO_SENSITIVITY_2000	  70	    // 70 mdps/digit

namespace L3GD20Gyro
{
    class L3GD20 {
        public:
            // constructor - SPI
            L3GD20(PinName mosi, PinName miso, PinName sclk, PinName ssel);

            enum Registers {
                WHO_AM_I_ADDR = 0x0F,       // device identification register
                CTRL_REG1_ADDR = 0x20,      // Control register 1
                CTRL_REG2_ADDR = 0x21,      // Control register 2
                CTRL_REG3_ADDR = 0x22,      // Control register 3
                CTRL_REG4_ADDR = 0x23,      // Control register 4
                CTRL_REG5_ADDR = 0x24,      // Control register 5
                STATUS_REG_ADDR = 0x27,     // Status register
                OUT_X_L_ADDR = 0x28,        // Output Register X
                OUT_X_H_ADDR = 0x29,        // Output Register X
                OUT_Y_L_ADDR = 0x2A,        // Output Register Y
                OUT_Y_H_ADDR = 0x2B,        // Output Register Y
                OUT_Z_L_ADDR = 0x2C,        // Output Register Z
                OUT_Z_H_ADDR = 0x2D,        // Output Register Z
            };

            int8_t read_register(int8_t reg);
            void write_register(int8_t reg, int8_t data);

        private:
            // The following two pointers are used to communicate through SPI interface
            SPI *spi_ptr;
            DigitalOut *_spi_gyro_cs;
    };
}

using L3GD20Gyro::L3GD20;

#endif // GYRO_DRIVER