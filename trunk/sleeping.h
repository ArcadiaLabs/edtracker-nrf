#pragma once

// this MUST be included in the source file which containing main()
//void ISR_IPF(void) __interrupt INTERRUPT_IPF;
//void ISR_RFIRQ(void) __interrupt INTERRUPT_RFIRQ;

// inits CLKLF and sets up the interrupts
void init_sleep(void);

// sleep for for a variable amount of time
// the time to sleep depends on time since last matrix change
//void sleep_dynamic(void);

// sleep for the number of 32KHz clock cycles
// so, we'll sleep for about sleep_cnt*30.52us
//void sleep_regret(uint16_t sleep_cnt);

// we go into standby power down mode
// RFIRQ or IPF wakes us up
void sleep(void);

//void sleep_standby(uint16_t sleep_cnt);
