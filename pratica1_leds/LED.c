/**
 * @file    LED.c
 * @content A program to blink internal led to a sequence of lights. 
 */
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

#define PORT_B ((PORTRegs_t *) 0x4004A000)
#define PORT_C ((PORTRegs_t *) 0x4004B000)
#define PORT_D ((PORTRegs_t *) 0x4004C000)

typedef struct{
	 uint32_t PDOR;
	 uint32_t PSOR;
	 uint32_t PCOR;
	 uint32_t PTOR;
	 uint32_t PDIR;
	 uint32_t PDDR;
} GPIORegs_t;

#define GPIO_B ((GPIORegs_t *) 0x400FF040)
#define GPIO_C ((GPIORegs_t *) 0x400FF080)
#define GPIO_D ((GPIORegs_t *) 0x400FF0C0)

/**
 * Delay function (haven't precision)
 */
void sleep(int n) {
	for (int i =  0; i < n; ++i)
	for (int j = 0; j < 4800; ++j);
}

/**
 * Function to alternate led state;
 * Red -> Green -> Blue
 */
int choice = 0;
void next() {
	switch(choice) {
		case 0:
			GPIO_B->PCOR = (1 << 18); // turn on red
			GPIO_B->PSOR = (1 << 19); // turn off green
			GPIO_D->PSOR = (1 << 1);  // turn off blue
			break;
		case 1:
			GPIO_B->PSOR = (1 << 18); // turn off red
			GPIO_B->PCOR = (1 << 19); // turn on green
			GPIO_D->PSOR = (1 << 1);  // turn off blue
			break;
		case 2:
			GPIO_B->PSOR = (1 << 18); // turn off red
			GPIO_B->PSOR = (1 << 19); // turn off green
			GPIO_D->PCOR = (1 << 1);  // turn on blue
			break;
	}SCGC5
	choice = (choice + 1) % 3;
}

/**
 * The board FDRM-KL25Z turn on led with 0 signal,
 */

int main(void) {
	/**
	 * Led setup
	 * */
	SIM->SCGC5 = (1 << 10) | (1 << 12); // Enable PORT B clock.

	PORT_B->PCR[18] = 1 << 8; // Set port B, pin 18 as GPIO (red led).
	PORT_B->PCR[19] = 1 << 8; // Set port B, pin 19 as GPIO (green led).
	PORT_D->PCR[1]  = 1 << 8; // Set port D, pin 1 as GPIO (blue led).


	GPIO_B->PDDR = (1 << 18); // GPIO_B pin 18 as output.
	GPIO_B->PSOR = (1 << 18); // turn off red led

	GPIO_B->PDDR |= (1 << 19); // GPIO_B pin 19 as output.
	GPIO_B->PSOR = (1 << 19);  // turn off green led.

	GPIO_D->PDDR = (1 << 1);  // GPIO_D pin 1 as output.
	GPIO_D->PSOR = (1 << 1);  // turn off blue led.

	// loop code
	while (1) {
		sleep(1000);
		next();
	}

    return 0 ;
}

