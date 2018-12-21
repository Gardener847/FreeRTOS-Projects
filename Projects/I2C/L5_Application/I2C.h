#pragma once
#ifndef I2C_H
#define I2C_H


class SlaveI2C {
public:
    SlaveI2C();
    ~SlaveI2C();
    static void slaveISR(void);
    bool init (uint8_t);
};

#endif