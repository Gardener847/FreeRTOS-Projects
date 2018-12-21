#include "disp.h"
#include "uart0_min.h"

void Disp::line(uint8_t times){
	for(uint8_t i = 0; i < times; ++i){
		uart0_puts("1 1 0 0");
	}
}

void Disp::dot(uint8_t times){
	for(uint8_t i = 0; i < times; ++i){
		uart0_puts("1 0 0 0");
	}
}

void Disp::end(){
	uart0_puts("0 0 0 1");
}

void Disp::none(){
	uart0_puts("0 0 0 0");
}
