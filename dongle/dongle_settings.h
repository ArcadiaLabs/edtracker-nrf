#ifndef DONGLE_SETTINGS_H
#define DONGLE_SETTINGS_H

const FeatRep_DongleSettings __xdata * get_settings(void);
void save_settings(const FeatRep_DongleSettings __xdata * pNewSettings);

// these are for testing
//void list_settings(void);

#endif	// DONGLE_SETTINGS_H
