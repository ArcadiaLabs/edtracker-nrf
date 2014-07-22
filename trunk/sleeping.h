#ifndef SLEEPING_H
#define SLEEPING_H

// this MUST be included in the source file which containing main()
#ifndef __C51__
void ISR_RFIRQ(void) __interrupt INTERRUPT_RFIRQ;
#endif

// inits CLKLF and sets up the interrupts
void init_sleep(void);

// we go into standby power down mode
void sleep_rfirq(void);		// goes to standby until the radio wakes us up

#endif	// SLEEPING_H