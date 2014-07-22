#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "reg24le1.h"
#include "nrfdbg.h"

#include "sleeping.h"
#include "edtracker.h"
#include "nrfutils.h"

/*
uint32_t watch_sec = 0;		// this is our watch

uint32_t get_seconds32(void)
{
	return watch_sec;
}

uint16_t get_seconds(void)
{
	return watch_sec;
}

void get_time(uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds)
{
	uint32_t sec = watch_sec;
	*seconds = sec % 60;
	sec /= 60;
	*minutes = sec % 60;
	sec /= 60;
	*hours = sec % 24;
	sec /= 24;
	*days = sec;
}

#define LSB(a) (((a) & 0xFFU))
#define MSB(a) (((a) & 0xFF00U) >> 8)

void set_compare_value(uint16_t value)
{
	RTC2CON = 1;				// disable compare
	RTC2CMP0 = LSB(value);		// write value
	RTC2CMP1 = MSB(value);
	RTC2CON = 5;				// enable compare
}
*/

void init_sleep(void)
{
	IEN1 = 0x02;		// enable RFIRQ interrupt
	EA = 1; 			// enable interrupts
}

/*
uint16_t capture_rtc(void)
{
	RTC2CON |= _BV(4);	// capture the counter
	return (RTC2CPT01 << 8) | RTC2CPT00;
}

// this interrupt wakes us up from sleep_regret()
void ISR_TICK(void) __interrupt INTERRUPT_TICK
{}
*/

// this ISR wakes us up if we get an ACK or the transceiver quits sending
void ISR_RFIRQ(void) __interrupt INTERRUPT_RFIRQ
{}

// this ISR wakes us up if we get an ACK or the transceiver quits sending
//void ISR_IPF(void) __interrupt INTERRUPT_IPF
//{
//}

/*
void process_clock(void)
{
	static uint16_t prev_rtc2cpt = 0;
	uint16_t curr_rtc2cnt = capture_rtc();

	// did we have an overflow since the last call?
	if (curr_rtc2cnt < prev_rtc2cpt)
		watch_sec += 2;
	
	prev_rtc2cpt = curr_rtc2cnt;
}

// sleep for a certain number of 32KHz ticks
void sleep_regret(uint16_t ticks)
{
	set_compare_value(capture_rtc() + ticks);
	PWRDWN = PWRDWN_REG_RET;
	
	process_clock();
}

// sleep for a certain number of 32KHz ticks, but can be woken up by RFIRQ
void sleep_standby(uint16_t ticks)
{
	set_compare_value(capture_rtc() + ticks);
	PWRDWN = PWRDWN_STANDBY;

	process_clock();
}
*/

void sleep_rfirq(void)
{
	PWRDWN = PWRDWN_STANDBY;
}

/*
const __code sleep_schedule_period_t sleep_schedule_default[] =
{
	{   300,  197},		// 5 minutes, ~6ms refresh
	{   900,  262},		// 15 minutes, ~8ms refresh
	{  1800,  660},		// 30 minutes, ~20ms refresh
	{0xffff, 3280},		// forever, ~100ms refresh
};

const __code sleep_schedule_period_t* active_sleep_schedule = sleep_schedule_default;
const __code sleep_schedule_period_t* curr_sleep_period;
uint16_t sleep_period_started = 0;

void sleep_dynamic(void)
{
	// if the current period is not forever
	if (curr_sleep_period->duration_sec != 0xffff)
	{
		// get the time elapsed in this period
		uint16_t curr_seconds = get_seconds();
		uint16_t sec_elapsed = curr_seconds - sleep_period_started;
		if (sec_elapsed >= curr_sleep_period->duration_sec)
		{
			// advance to the next period
			++curr_sleep_period;
			
			// remember the time
			sleep_period_started = curr_seconds;
		}
	}

	sleep_regret(curr_sleep_period->num_ticks);
}

void sleep_reset(void)
{
	curr_sleep_period = active_sleep_schedule;
	sleep_period_started = get_seconds();
}

void wait_for_all_keys_up(void)
{
	sleep_reset();
	matrix_scan();
	while (get_num_keys_pressed())
	{
		sleep_dynamic();
		matrix_scan();
	}
}

void wait_for_key_down(void)
{
	sleep_reset();
	matrix_scan();
	while (get_num_keys_pressed() == 0)
	{
		sleep_dynamic();
		matrix_scan();
	}
}

void wait_for_matrix_change(void)
{
	sleep_reset();
	while (!matrix_scan())
		sleep_dynamic();
}
*/