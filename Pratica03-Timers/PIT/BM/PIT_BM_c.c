#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL43Z4.h"
#include "fsl_debug_console.h"

typedef struct {
	uint32_t mcr;
	uint32_t ltmr64h;
	uint32_t ltmr64l;
} PIT_Regs_t;

#define PIT_REG ((PIT_Regs_t *) 0x40037000)

typedef struct {
	uint32_t ldval;
	uint32_t cval;
	uint32_t tctrl;
	uint32_t tflag;
}PIT_Chnl_Regs_t;
#define PIT_CH_REG ((PIT_Chnl_Regs_t *) 0x40037100)

typedef struct {
	uint32_t PCR[32];
} PORTRegs_t;

#define PORT_B ((PORTRegs_t *) 0x4004A000)
#define PORT_D ((PORTRegs_t *) 0x4004C000)
#define PORT_E ((PORTRegs_t *) 0x4004D000)


typedef struct {
	uint32_t PDOR;
	uint32_t PSOR;
	uint32_t PCOR;
	uint32_t PTOR;
	uint32_t PDIR;
	uint32_t PDDR;
} GPIORegs_t;

#define GPIO_B ((GPIORegs_t *) 0x400FF040)
#define GPIO_D ((GPIORegs_t *) 0x400FF0C0)
#define GPIO_E ((GPIORegs_t *) 0x400FF100)

typedef struct {
	uint32_t iser[1];
	uint32_t rsvd[31];
	uint32_t icer[1];
	uint32_t rsvd1[31];
	uint32_t ispr[1];
	uint32_t rsvd2[31];
	uint32_t icpr[1];
	uint32_t rsvd3[31];
	uint32_t rsvd4[64];
	uint32_t ipr[8];
}NVIC_Regs_t;

#define NVIC_REG ((NVIC_Regs_t *) 0xE000E100)

/*
 * Timer em N segundos cnt = (CLOCK_GetBusClkFreq) x N
 *
 * Timer em N ms cnt = (CLOCK_GetBusClkFreq / 1000) X N
 *
 * Timer em N us cnt = (CLOCK_GetBusClkFreq / 1000000) x N
 */

#define GET_SEC_COUNT(x) (CLOCK_GetBusClkFreq() * x)

int cnt_clock = 0;

void PIT_IRQHandler(void) {
	GPIO_E->PTOR = (1 << 31);

	if (cnt_clock == 2) {
		GPIO_D->PTOR = (1 << 5);
	}

	cnt_clock = (cnt_clock + 1) % 3;

	/*
	 * 	Timer Flag Register (PIT_TFLGn)
	 *|   0 -- 30 |  31 |
	 *|      0    | TIF |
	 *|			  | wic |
	 */
	PIT_CH_REG->tflag = (1 << 0);
}

/*
 * @brief   Application entry point.
 */
int main(void) {

    uint32_t clock = CLOCK_GetBusClkFreq();

    //Ativando o Clock enable for PORTB
    /*
	 * 	System Clock Gating Control Register 5 (SIM_SCGC5)
	 *| 31  --  20| 19 | 18 -- 14 | 13 | 12 | 11 | 10 |  9 | 8  7 | 6 |   5 | 4  3  2 | 1 |   0   |
	 *|      0	  |  0 |     0    | PE | PD | PC | PB | PA |   1  | 0 | TSI	|    0    | 0 | LPTMR |
	 */
	SIM->SCGC5 = (1 << 13) | (1 << 12);

	// PORT_E_31 como GPIO
	PORT_E->PCR[31] = (1 << 8);
	PORT_D->PCR[5] = (1 << 8);

	// GPIO_E_31 como Output
	GPIO_E->PDDR = (1 << 31);
	GPIO_D->PDDR = (1 << 5);

	//Ativar o clock do PIT
	/*
	 * 	System Clock Gating Control Register 6 (SIM_SCGC6)
	 *|   31 | 30 |  29 | 28 |  27  |  26  |  25  |  24  |  23 | 22 -- 2 |    1   |  0  |
	 *| DAC0 |  0 | RTC |  0 | ADC0 | TPM2 | TPM1 | TPM0 | PIT |    0    | DMAMUX | FTF |
	 */
	SIM->SCGC6 = (1 << 23);

	// Ativar PIT Module
	// 0 -- Clock for standard PIT timers is enabled
	/*
	 * 	PIT Module Control Register (PIT_MCR)
	 *|   0 -- 28 |     29    |  30  | 31  |
	 *|      0    |  Reserved | MDIS | FRZ |
	 */
	PIT_REG->mcr = 0;

	// Configurando o Time Period
	PIT_CH_REG->ldval = GET_SEC_COUNT(2);

	// Ativando o timer interrupt & timer
	/*
	 * 	Timer Control Register (PIT_TCTRLn)
	 *|   0 -- 28 |  29 |  30 |  31 |
	 *|      0    | CHN | TIE | TEN |
	 */
	PIT_CH_REG->tctrl = (1 << 0) | (1 << 1);

	//NVIC_EnableIRQ(PIT_IRQn);
	NVIC_REG->iser[0] = (1 << 22);

	while(1){
	}

  return 0 ;
}

