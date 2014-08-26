#ifndef PROC_PACKET_H
#define PROC_PACKET_H

bool process_packet(mpu_packet_t* pckt);

// drift compensation configuration
void reset_x_drift_comp(void);
void save_x_drift_comp(void);
float get_curr_x_drift_comp(void);

// recenter initiated from the PC
void recenter(void);

#endif	// PROC_PACKET_H
