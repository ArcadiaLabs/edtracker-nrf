#ifndef RF_DNGL_H
#define RF_DNGL_H

void rf_dngl_init(void);
uint8_t rf_dngl_recv(void __xdata * buff, uint8_t buff_size);

void rf_dngl_queue_ack_payload(void __xdata * buff, uint8_t num_bytes);

#endif	// RF_DNGL_H