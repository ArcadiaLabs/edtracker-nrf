#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <compiler_mcs51.h>

#include "rf_protocol.h"
#include "nRF24L.h"
#include "hw_defs.h"

void rf_dngl_init(void)
{
	nRF_Init();

	// set the addresses
	nRF_WriteAddrReg(RX_ADDR_P0, DongleAddr, NRF_ADDR_SIZE);

	nRF_WriteReg(EN_AA, vENAA_P0);			// enable auto acknowledge
	nRF_WriteReg(SETUP_RETR, vARD_250us);	// ARD=250us, ARC=disabled
	nRF_WriteReg(RF_SETUP, vRF_DR_2MBPS		// data rate
						| vRF_PWR_0DBM);	// output power

	nRF_WriteReg(FEATURE, vEN_DPL | vEN_ACK_PAY);	// enable dynamic payload length and ACK payload
	nRF_WriteReg(DYNPD, vDPL_P0);					// enable dynamic payload length for pipe 0

	nRF_FlushRX();
	nRF_FlushTX();
	
	nRF_WriteReg(EN_RXADDR, vERX_P0);					// enable RX address
	nRF_WriteReg(STATUS, vRX_DR | vTX_DS | vMAX_RT);	// reset the IRQ flags

	nRF_WriteReg(RF_CH, CHANNEL_NUM);			// set the channel
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO 		// enable a 2 byte CRC
								| vMASK_TX_DS	// we don't care about the TX_DS status flag
								| vPRIM_RX		// RX mode
								| vPWR_UP);		// power up the transceiver

	nRF_CE_hi();		// start receiving
}

uint8_t rf_dngl_recv(void __xdata * buff, uint8_t buff_size)
{
	uint8_t ret_val = 0;
	
	// check if there's data in the RX FIFO
	nRF_ReadReg(FIFO_STATUS);
	if ((nRF_data[1] & vRX_EMPTY) == 0)
	{
		// read the payload
		nRF_ReadRxPayloadWidth();
		ret_val = nRF_data[1];

		// the nRF specs state I have to drop the packet if the length is > 32
		if (ret_val > 32)
		{
			nRF_FlushRX();
			ret_val = 0;
		} else {
			nRF_ReadRxPayload(ret_val);
			memcpy(buff, nRF_data + 1, ret_val > buff_size ? buff_size : ret_val);
		}

		// reset the TX_DS
		if (nRF_data[0] & vTX_DS)
			nRF_WriteReg(STATUS, vTX_DS);
	}
	
	return ret_val;
}

void rf_dngl_queue_ack_payload(void __xdata * buff, const uint8_t num_bytes)
{
	// get the TX FIFO status
	nRF_ReadReg(FIFO_STATUS);

	// clear any unsent ACK payloads
	if (!(nRF_data[1]  &  vTX_EMPTY))
		nRF_FlushTX();

	// send the payload
	nRF_WriteAckPayload(buff, num_bytes, 0);	// pipe 0
}
