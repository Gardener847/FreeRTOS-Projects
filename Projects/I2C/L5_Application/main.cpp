#include <i2c_base.hpp>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "io.hpp"
#include "printf_lib.h"
#include "LPC17xx.h"
#include "I2C.h"
#include "i2c2.hpp"

int main(int argc, char const *argv[]) {
    const uint8_t addr = 0x1E;
    SlaveI2C a;
    if (a.init(addr)) {
        u0_dbg_printf("Inside if\n");
        while(1);
    }

    return 0;
}