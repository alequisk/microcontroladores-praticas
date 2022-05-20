#include "MKL43Z4.h"
#include "fsl_debug_console.h"

/** PORT struct config **/
typedef struct{
	uint32_t PCR[32];
} PORTRegs_t;

/** Base address of PORTS **/
#define PORT_A ((PORTRegs_t *) 0x40049000)
#define PORT_B ((PORTRegs_t *) 0x4004A000)
#define PORT_C ((PORTRegs_t *) 0x4004B000)
#define PORT_D ((PORTRegs_t *) 0x4004C000)
#define PORT_E ((PORTRegs_t *) 0x4004D000)

/** GPIO struct config **/
typedef struct{
	uint32_t PDOR;
	uint32_t PSOR;
	uint32_t PCOR;
	uint32_t PTOR;
	uint32_t PDIR;
	uint32_t PDDR;
} GPIORegs_t;

/** Base address of GPIO **/
#define GPIO_REG_A ((GPIORegs_t *) 0x400FF000)
#define GPIO_REG_B ((GPIORegs_t *) 0x400FF040)
#define GPIO_REG_C ((GPIORegs_t *) 0x400FF080)
#define GPIO_REG_D ((GPIORegs_t *) 0x400FF0C0)
#define GPIO_REG_E ((GPIORegs_t *) 0x400FF100)

#define PIN_D 5
#define PIN_E 31

/*====== Low Power Timer struct configuration ======*/
typedef struct {
	uint32_t CSR; // status register
	uint32_t PSR; // prescale
	uint32_t CMR; // comparator
	uint32_t CNR; // counter
} LPTMR_Regs_t;

/** Base address of LPT **/
#define LPTMR_REG ((LPTMR_Regs_t *) 0x40040000)

/** Timer Enable Flag **/
#define TEN 0
/** Prescale macro **/
#define prescale(x) ((x & 0b1111) << 3)
/** LPTMR interruption bit (pag 54) **/
#define LPTMR0_IR 28
/** ByPass prescale bit **/
#define BYPSS 2
/** Time Interrupt Enable **/
#define TIE 6
/** Timer compare flag **/
#define TCF 7
/** Low Power Timer Access Control (p.163) **/
#define LPTMR_CLK 0

/** Interruption vector function **/
void LPTMR0_IRQHandler(void){
	/** Toggle Red led **/
	GPIO_REG_E->PTOR |= (1 << PIN_E);

	/** Reset timer **/
	LPTMR_REG->CSR |= (1 << TCF);
}

/** PIN OF CLOCK PORTS (SCGC5) **/
#define PIN_CLOCK_PORTA 9
#define PIN_CLOCK_PORTB 10
#define PIN_CLOCK_PORTC 11
#define PIN_CLOCK_PORTD 12
#define PIN_CLOCK_PORTE 13

#define GPIO_MUX 8

typedef struct {
	uint32_t ISER[1U];          // Interruption SET
	uint32_t RESERVED0[31U];
	uint32_t ICER[1U];          // Interruption CLEAR
	uint32_t RSERVED1[31U];
	uint32_t ISPR[1U];          // Interruption SET Pending
	uint32_t RESERVED2[31U];
	uint32_t ICPR[1U];          // Interruption CLEAR Pending
	uint32_t RESERVED3[31U];
	uint32_t RESERVED4[64U];
	uint32_t IP[8U];            // Interruption SET Priority
} NVIC_Regs_t;

#define NVIC_REG ((NVIC_Regs_t *)0xE000E100) // B3-245, Armv6-m Architecture

int main(void) {
	/*====== Clock setting ======*/

	/** Enable PORTE | LPTMR clocks **/
	SIM->SCGC5 = (1 << PIN_CLOCK_PORTE) | (1 << LPTMR_CLK);

	/*====== GPIO setting ======*/

	/** setting 31 PORT_E as GPIO **/
	PORT_E->PCR[PIN_E] = (1 << GPIO_MUX);

	/** Setting direction to Output **/
	GPIO_REG_E->PDDR |= (1 << PIN_E);

	/** Turn off red led **/
	GPIO_REG_E->PSOR |= (1 << PIN_E);

	/*====== LPTMR configuration ======*/

	/** Disable LPTMR to correctly timer configuration **/
	LPTMR_REG->CSR &= ~(1 << TEN);

	/** Disable bypass **/
	LPTMR_REG->PSR &= ~(1 << BYPSS);

	/** Use LPO clock **/
	LPTMR_REG->PSR |= (1 << 0);

	/** Setting prescale to divide clock by 8 **/
	LPTMR_REG->PSR |= prescale(0b0010);

	/** Setting compare value **/
	LPTMR_REG->CMR = 1000; // 2 second

	/** Enable interruption when TCF is set **/
	LPTMR_REG->CSR |= (1 << TIE);

	/** Enable LPTMR **/
	LPTMR_REG->CSR |= (1 << TEN);

	/** Enable interruptions by compare timer **/
	NVIC_REG->ISER[0] = (1 << LPTMR0_IR);

	while (1) {
	}

  return 0 ;
}
