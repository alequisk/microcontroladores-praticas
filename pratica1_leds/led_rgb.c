#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

typedef struct{
	uint32_t PCR[32];
} PORTRegs_t;

typedef struct{
	 uint32_t PDOR;
	 uint32_t PSOR;
	 uint32_t PCOR;
	 uint32_t PTOR;
	 uint32_t PDIR;
	 uint32_t PDDR;
} GPIORegs_t;

#define PORT_B ((PORTRegs_t *) 0x4004A000)
#define PORT_D ((PORTRegs_t *) 0x4004C000)

#define GPIO_B ((GPIORegs_t *) 0x400FF040)
#define GPIO_D ((GPIORegs_t *) 0x400FF0C0)

/**
 * Delay function (haven't precision)
 */
void sleep(int n) {
	for (int i =  0; i < n; ++i)
	for (int j = 0; j < 2400; ++j);
}

/**
 * Turn led by mask params.
 * Bits leads by RGB mask;
 */
void LedSetByMask(int mask) {
	if (mask & 0b001) {
		GPIO_D->PCOR = (1 << 1);
	} else {
		GPIO_D->PSOR = (1 << 1);
	}

	if (mask & 0b010) {
		GPIO_B->PCOR = (1 << 19);
	} else {
		GPIO_B->PSOR = (1 << 19);
	}

	if (mask & 0b100) {
		GPIO_B->PCOR = (1 << 18);
	} else {
		GPIO_B->PSOR = (1 << 18);
	}
}

/**
 * The board FDRM-KL25Z turn on led with 0 signal,
 */

int main(void) {
	/**
	 * Led setup
	 * */
	SIM->SCGC5 = (1 << 10) | (1 << 12); // Enable PORT B and D clock.

	PORT_B->PCR[18] = 1 << 8;  // Set port B, pin 18 as GPIO (red led).
	PORT_B->PCR[19] = 1 << 8;  // Set port B, pin 19 as GPIO (green led).
	PORT_D->PCR[1]  = 1 << 8;  // Set port D, pin 1 as GPIO (blue led).


	GPIO_B->PDDR = (1 << 18);  // GPIO_B pin 18 as output.
	GPIO_B->PSOR = (1 << 18);  // turn off red led

	GPIO_B->PDDR |= (1 << 19); // GPIO_B pin 19 as output.
	GPIO_B->PSOR =  (1 << 19); // turn off green led.

	GPIO_D->PDDR = (1 << 1);   // GPIO_D pin 1 as output.
	GPIO_D->PSOR = (1 << 1);   // turn off blue led.

  /** Code loop, behave */
	while (1) {
		LedSetByMask(0b100);      // red
		sleep(1000);
		LedSetByMask(0b010);      // green
		sleep(1000);
		LedSetByMask(0b001);      // blue
		sleep(1000);
		LedSetByMask(0b101);      // red + blue
		sleep(1000);
		LedSetByMask(0b011);      // green + blue
		sleep(1000);
		LedSetByMask(0b110);      // red + green
		sleep(1000);
		LedSetByMask(0b111);      // red + green + blue
		sleep(1000);
	}

  return 0 ;
}

