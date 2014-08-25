#ifndef PROC_PACKET_H
#define PROC_PACKET_H

bool process_packet(mpu_packet_t* pckt);
void save_x_drift_comp(void);
float get_curr_x_drift_comp(void);

#endif	// PROC_PACKET_H
