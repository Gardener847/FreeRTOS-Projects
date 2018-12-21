#ifndef DISP_H
#define DIPS_H
#include <stdint.h>

class Disp{
public:
	void line(uint8_t times);
	void dot(uint8_t times);
	void end();
	void none();
};
#endif
