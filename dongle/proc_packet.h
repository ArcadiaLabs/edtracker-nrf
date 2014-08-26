#ifndef PROC_PACKET_H
#define PROC_PACKET_H

bool process_packet(mpu_packet_t* pckt);

// drift compensation configuration
void save_x_drift_comp(void);
float get_curr_x_drift_comp(void);

// recenter initiated from the PC
void recenter(void);

extern int16_t driftSamples;
extern float dX;

#endif	// PROC_PACKET_H
