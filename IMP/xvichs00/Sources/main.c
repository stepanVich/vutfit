/*
 * Autor: Štìpán Vích, xvichs00
 * Popis: Blikání LEDky na fitkitu3
 * Popis zmìn: Zapnutí modulu WatchDog a jeho postupné refreshování bìhem blikání ledky v normálních nebo okénkovém režimu.
 * 			   Pomocí maker win_mod a err_mod lze vypnout/zapnout okenkovy rezim, nebo demonstrovat neobslouzeni modulu watchdog.
 * Velikost zmìn: pøidáno 20% øádkù oproti pùvodní aplikaci
 * Datum: 29.12.2017
 *
 * */


/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"
#include "stdio.h"

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

/* Constants specifying loop duration */
#define	DELAY_T 200000

/* Mapping of LEDs and buttons to specific port pins: */
// Note: only D9, SW3 and SW5 are used in this sample app
#define LED_D9  0x20 // Port B, bit 5
#define LED_D10 0x10 // Port B, bit 4
#define LED_D11 0x8 // Port B, bit 3
#define LED_D12 0x4 // Port B, bit 2

#define BTN_SW2 0x400 // Port E, bit 10
#define BTN_SW3 0x1000 // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26
#define BTN_SW6 0x800 // Port E, bit 11

// definice volitelnych maker
#define win_mode 0
#define err_mode 0

int pressed_up = 0, pressed_down = 0;
unsigned int compare = 0x200;

// Pocet cyklu pro blikani ledky
// Po refreshi watchdogu se vynuluje
unsigned long cycles = 0;
// Celkovy pocet refreshu watchdogu
unsigned int refresh_count = 0;

/* Initialize the MCU - basic clock settings, turning the watchdog on */
void MCUInit(void)  {

    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);

    // unlock watchdog
    WDOG_UNLOCK = (0xC520);
    WDOG_UNLOCK = (0xD928);

    // set watchdog timeout
    // timeout set on approximately 1s
    WDOG_TOVALH = 0;
    WDOG_TOVALL = (6000/5);

    // set prescale register
    WDOG_PRESC = 0x00;

    if(!win_mode) {
    	// window mode disabled
    	WDOG_STCTRLH = 0xD1;
    } else {
    	// window mode enabled
    	WDOG_STCTRLH = 0xD9;
    	// set window size
    	// size set on approximately 0.5s
    	WDOG_WINH = 0;
    	WDOG_WINL = (3000/5);
    }

}

void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;

    /* Set corresponding PTB pins (connected to LED's) for GPIO functionality */
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); // D9
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); // D10
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); // D11
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); // D12

    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    /* Change corresponding PTB port pins as outputs */
    PTB->PDDR = GPIO_PDDR_PDD( 0x3C );
    PTB->PDOR |= GPIO_PDOR_PDO( 0x3C); // turn all LEDs OFF
}

void LPTMR0_IRQHandler(void)
{
    // Set new compare value set by up/down buttons
    LPTMR0_CMR = compare; // !! the CMR reg. may only be changed while TCF == 1
    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK; // writing 1 to TCF tclear the flag
    GPIOB_PDOR ^= LED_D9; // invert D9 state
}

void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;     // Turn OFF LPTMR to perform setup

    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK  // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ; // use the choice of clock

    LPTMR0_CMR = count;  // Set compare value

    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK   // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
                );

    NVIC_EnableIRQ(LPTMR0_IRQn);

    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   // Turn ON LPTMR0 and start counting
}




int main(void)
{
    MCUInit();
    PortsInit();
    LPTMR0Init(compare);
    while (1) {

    	// watchdog refresh
    	cycles ++;
    	if(cycles == 400000 && !err_mode) {
    		WDOG_REFRESH = 0xA602;
    		WDOG_REFRESH = 0xB480;
    		printf("Refresh cislo: %i\n", refresh_count);
    		cycles = 0;
    		refresh_count+=1;
    	}


        // pressing the up button decreases the compare value,
        // i.e. the compare event will occur more often;
        // testing pressed_up avoids uncontrolled modification
        // of compare if the button is hold.
        if (!pressed_up && !(GPIOE_PDIR & BTN_SW5))
        {
            pressed_up = 1;
            compare -= 0x40;
        }
        else if (GPIOE_PDIR & BTN_SW5) pressed_up = 0;
        // pressing the down button increases the compare value,
        // i.e. the compare event will occur less often;
        if (!pressed_down && !(GPIOE_PDIR & BTN_SW3))
        {
            pressed_down = 1;
            compare += 0x40;
        }
        else if (GPIOE_PDIR & BTN_SW3) pressed_down = 0;
        // some limits - in order to keep the LED blinking reasonable
        if (compare < 0x40) compare = 0x40;
        if (compare > 0x400) compare = 0x400;

    }

    return 0;
}
