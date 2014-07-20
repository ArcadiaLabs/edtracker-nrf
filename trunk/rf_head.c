#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nRF24L.h"
#include "edtracker.h"
#include "rf_protocol.h"
#include "rf_head.h"
#include "nrfutils.h"
#include "nrfdbg.h"
#include "sleeping.h"

// we want to count the lost packets
// uint32_t plos_total, arc_total, rf_packets_total;

void rf_head_init(void)
{
	nRF_Init();

	// write the addresses
	nRF_WriteAddrReg(TX_ADDR, DongleAddr, 5);

	// we need to set the RX address to the same as TX to be
	// able to receive ACK
	nRF_WriteAddrReg(RX_ADDR_P0, DongleAddr, 5);

	nRF_WriteReg(EN_AA, vENAA_P0);			// enable auto acknowledge
	nRF_WriteReg(EN_RXADDR, vERX_P0);		// enable RX address (for ACK)
	
	nRF_WriteReg(SETUP_RETR, vARD_250us 	// auto retransmit delay - ARD
							| 0x02);		// auto retransmit count - ARC
	nRF_WriteReg(FEATURE, vEN_DPL | vEN_ACK_PAY);	// enable dynamic payload length and ACK payload
	nRF_WriteReg(DYNPD, vDPL_P0);					// enable dynamic payload length for pipe 0

	nRF_FlushRX();
	nRF_FlushTX();
	
	nRF_WriteReg(STATUS, vRX_DR | vTX_DS | vMAX_RT);	// reset the IRQ flags
	nRF_WriteReg(RF_CH, CHANNEL_NUM);					// set the channel
	
	// reset the the lost packet counters
	// plos_total = arc_total = rf_packets_total = 0;
}

bool rf_head_send_message(const void* buff, const uint8_t num_bytes)
{
	bool is_sent;
	uint8_t status;

	// wait for XOSC16M to start
	while ((CLKLFCTRL & 0x08) == 0)
		;

	nRF_WriteReg(RF_SETUP, vRF_DR_2MBPS			// data rate 
							| vRF_PWR_0DBM);	// output power

	nRF_FlushTX();
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO | vPWR_UP);	// power up
	nRF_WriteReg(STATUS, vTX_DS | vRX_DR | vMAX_RT);	// reset the status flag
	nRF_WriteTxPayload(buff, num_bytes);

	nRF_CE_hi();	// signal the transceiver to send the packet
	sleep_rfirq();	// wait for the RF module to wake us up
	nRF_CE_lo();

	status = nRF_NOP();					// read the status reg
	is_sent = (status & vTX_DS) != 0;	// did we get an ACK?
	
	nRF_WriteReg(STATUS, vMAX_RT | vTX_DS | vRX_DR);	// reset the status flags

	// read the ARC
	//nRF_ReadReg(OBSERVE_TX);
	//arc_total += nRF_data[1] & 0x0f;
	
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO);		// nRF power down
	
	return is_sent;
}

uint8_t rf_head_read_ack_payload(void* buff, const uint8_t buff_size)
{
	uint8_t ret_val = 0, fifo_status;

	nRF_ReadReg(FIFO_STATUS);
	fifo_status = nRF_data[1];

	if ((fifo_status & vRX_EMPTY) == 0)
	{
		uint8_t ack_bytes;
		
		nRF_ReadRxPayloadWidth();
		ack_bytes = nRF_data[1];

		// the max ACK payload size has to be 2
		if (ack_bytes <= 2)
		{
			// read the entire payload
			nRF_ReadRxPayload(ack_bytes);

			// copy up to buff_size bytes
			ret_val = ack_bytes < buff_size ? ack_bytes : buff_size;
			memcpy(buff, nRF_data + 1, ret_val);
		} else {
			nRF_FlushRX();
		}
	}

	return ret_val;
}

void rf_head_get_observe(uint8_t* arc, uint8_t* plos)
{
	nRF_ReadReg(OBSERVE_TX);
	if (arc)
		*arc = nRF_data[1] & 0x0f;

	if (plos)
		*plos = nRF_data[1] >> 4;
}

bool rf_head_process_ack_payloads(uint8_t* msg_buff_free, uint8_t* msg_buff_capacity)
{
	bool ret_val = false;
	uint8_t buff[3];

	// set defaults
	if (msg_buff_free)		*msg_buff_free = 0;
	if (msg_buff_capacity)	*msg_buff_capacity = 0;
	
	while (rf_head_read_ack_payload(buff, sizeof buff))
	{
	/*
		if (buff[0] == MT_LED_STATUS)
		{
			//set_leds(buff[1], 25);

		} else if (buff[0] == MT_TEXT_BUFF_FREE) {
			
			// make a proper message pointer
			rf_msg_text_buff_state_t* msg_free_buff = (rf_msg_text_buff_state_t*) buff;
			
			ret_val = true;
			
			if (msg_buff_free)
				*msg_buff_free = msg_free_buff->bytes_free;
				
			if (msg_buff_capacity)
				*msg_buff_capacity = msg_free_buff->bytes_capacity;
		}
		*/
	}
	
	return ret_val;
}
