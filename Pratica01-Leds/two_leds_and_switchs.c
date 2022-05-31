/**
 * @file    MKL43Z4_led.c
 * @brief   Application entry point.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL43Z4.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#define PIN_LED01 29U
#define PIN_LED02 1U

#define PIN_SW01 5U
#define PIN_SW02 12U

int main(void) {
	/** clock activation */
	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortE);

	PORT_SetPinMux(PORTE, PIN_LED01, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, PIN_LED02, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTA, PIN_SW01, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTA, PIN_SW02, kPORT_MuxAsGpio);


	gpio_pin_config_t CONFIG_LED_01 = {kGPIO_DigitalOutput, 0},
					  CONFIG_LED_02 = {kGPIO_DigitalOutput, 0};

	/** configure led */
	GPIO_PinInit(GPIOE, PIN_LED01, &CONFIG_LED_01);
	GPIO_PinInit(GPIOE, PIN_LED02, &CONFIG_LED_02);

	/** configure switch */
	gpio_pin_config_t CONFIG_SW_01_PIN = {kGPIO_DigitalInput, 1};
	gpio_pin_config_t CONFIG_SW_02_PIN = {kGPIO_DigitalInput, 1};

	GPIO_PinInit(GPIOA, PIN_SW01, &CONFIG_SW_01_PIN);
	GPIO_PinInit(GPIOA, PIN_SW02, &CONFIG_SW_02_PIN);

	while (1) {
		if (GPIO_PinRead(GPIOA, PIN_SW01)) {
			GPIO_PinWrite(GPIOE, PIN_LED01, 1);
		} else {
			GPIO_PinWrite(GPIOE, PIN_LED01, 0);
		}

		if (GPIO_PinRead(GPIOA, PIN_SW02)) {
			GPIO_PinWrite(GPIOE, PIN_LED02, 1);
		} else {
			GPIO_PinWrite(GPIOE, PIN_LED02, 0);
		}
	}

    return 0 ;
}

