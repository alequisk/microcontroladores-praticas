/**
 * @file    GPIO_KSDK.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"

#define PIN_LED01 13U
#define PIN_LED02 8U

#define PIN_SW01 12U
#define PIN_SW02  4U

/**
 * PORTAS
 * PORTC8 e PORTA13 - LEDS
 * PORTD4 e PORTA12 - SWITCHS
 *
 */

void PORTA_IRQHandler(void) {
	if(GPIO_ReadPinInput(GPIOA, PIN_SW01)) {
		GPIO_SetPinsOutput(GPIOA, (1 << PIN_LED01));
	}else{
		GPIO_ClearPinsOutput(GPIOA, (1 << PIN_LED01));
	}

	PORT_ClearPinsInterruptFlags(PORTA, (1 << PIN_SW01));
}

void PORTD_IRQHandler(void) {
	if(GPIO_ReadPinInput(GPIOD, PIN_SW02)) {
		GPIO_SetPinsOutput(GPIOC, (1 << PIN_LED02));
	}else{
		GPIO_ClearPinsOutput(GPIOC, (1 << PIN_LED02));
	}

	PORT_ClearPinsInterruptFlags(PORTD, (1 << PIN_SW02));
}

/*
 * @brief   Application entry point.
 */
int main(void) {

	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);

	// Configuracao dos Leds
	PORT_SetPinMux(PORTA, PIN_LED01, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTC, PIN_LED02, kPORT_MuxAsGpio);

	// Setup de switchs
	port_pin_config_t SETUP_SW1={0};
	SETUP_SW1.mux        = kPORT_MuxAsGpio;
	SETUP_SW1.pullSelect = kPORT_PullUp;

	port_pin_config_t SETUP_SW2={0};
	SETUP_SW2.mux        = kPORT_MuxAsGpio;
	SETUP_SW2.pullSelect = kPORT_PullUp;

	// Configurando Switch 1
	PORT_SetPinConfig(PORTA, PIN_SW01, &SETUP_SW1);
	PORT_SetPinInterruptConfig(PORTA, PIN_SW01, kPORT_InterruptEitherEdge);

	// Configurando Switch 2
	PORT_SetPinConfig(PORTD, PIN_SW02, &SETUP_SW2);
	PORT_SetPinInterruptConfig(PORTD, PIN_SW02, kPORT_InterruptEitherEdge);

	gpio_pin_config_t LED01 = { kGPIO_DigitalOutput, 0 }; // C8 e A13
	gpio_pin_config_t LED02 = { kGPIO_DigitalOutput, 0 };

	gpio_pin_config_t SW01 = {kGPIO_DigitalInput, 0 }; // D4 e A12
	gpio_pin_config_t SW02 = {kGPIO_DigitalInput, 0 };

	// Configurando GPIO_C_9 como saída
	GPIO_PinInit(GPIOA, PIN_LED01, &LED01);
	GPIO_PinInit(GPIOC, PIN_LED02, &LED02);

	// Configurando GPIO_A_12 como entrada
	GPIO_PinInit(GPIOA, PIN_SW01, &SW01);
	GPIO_PinInit(GPIOD, PIN_SW02, &SW02);

	// Ativando PORTA_A da interrupção no NVIC
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);

	while(1) {
	}

    return 0;
}




