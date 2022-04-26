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

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
void PORTA_IRQHandler(void)
{
	if(GPIO_ReadPinInput(GPIOA, 12)) {
		GPIO_SetPinsOutput(GPIOC, (1<<9));
	}else{
		GPIO_ClearPinsOutput(GPIOC, (1 << 9));
	}

	PORT_ClearPinsInterruptFlags(PORTA, (1 << 12));
}

/*
 * @brief   Application entry point.
 */
int main(void) {
	port_pin_config_t config={0};
	gpio_pin_config_t gpioLed = {
			kGPIO_DigitalOutput,
			1
	};

	gpio_pin_config_t swLed = {
			kGPIO_DigitalInput,
			0
	};
	// Ativar o clock para as PORTA A e C 
	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortC);

	// Ativar PORTA_C_9 como GPIO 
	PORT_SetPinMux(PORTC, 9, kPORT_MuxAsGpio);

	// Ativar PORTA_A_12 como GPIO em Pull UP 
	config.mux = kPORT_MuxAsGpio;
	config.pullSelect = kPORT_PullUp;
	PORT_SetPinConfig(PORTA, 12, &config);
	PORT_SetPinInterruptConfig(PORTA, 12, kPORT_InterruptEitherEdge);

	// Configurando GPIO_C_9 como saída
	GPIO_PinInit(GPIOC, 9, &gpioLed);

	// Configurando GPIO_A_12 como entrada
	GPIO_PinInit(GPIOA, 12, &swLed);

	// Ativando PORTA_A da interrupção no NVIC 
	NVIC_EnableIRQ(PORTA_IRQn);

	while(1) {
	}

    return 0;
}



