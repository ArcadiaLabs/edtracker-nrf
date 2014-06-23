#pragma once

// this MUST be included in the source file which containing main()
void ISR_TICK(void) __interrupt INTERRUPT_TICK;
void ISR_RFIRQ(void) __interrupt INTERRUPT_RFIRQ;

// inits CLKLF and sets up the interrupts
void init_sleep(void);

// sleep for for a variable amount of time
// the time to sleep depends on time since last matrix change
void sleep_dynamic(void);

// sleep for the number of 32KHz clock cycles
// so, we'll sleep for about sleep_cnt*30.52us
void sleep_regret(uint16_t sleep_cnt);

// we go into standby power down mode
// RFIRQ wakes us up
void sleep_rfirq(void);

void sleep_standby(uint16_t sleep_cnt);

/*
void wait_for_all_keys_up(void);
void wait_for_key_down(void);
void wait_for_matrix_change(void);

uint16_t get_seconds(void);

// these return the number of seconds since reset (with overflow)
uint32_t get_seconds32(void);
uint16_t get_seconds(void);

uint16_t capture_rtc(void);

// returns the time since reset
void get_time(uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

// used to setup the sleep schedule
typedef struct 
{
	uint16_t	duration_sec;	// the duration of this period in the schedule
	uint16_t	num_ticks;		// number of ticks of sleep between refreshes
} sleep_schedule_period_t;
*/