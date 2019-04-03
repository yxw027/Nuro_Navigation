// nuroGpsRelease.cpp: implementation of the nuroGpsRelease class.
//
//////////////////////////////////////////////////////////////////////
#include "nuroGpsRelease.h"
#include "EMG_Plugin.h"


unsigned int wgtochina_lb(DECRYPTION_TYPE decryption_type, int wg_flag, unsigned int wg_lng, unsigned int wg_lat, int wg_heit, int wg_week, unsigned int wg_time, unsigned  int *china_lng, unsigned int *china_lat)
{
	unsigned int ret_code = 0;

	switch (decryption_type)
	{
	case NO_DECRYPTION:
			// Do nothing if no action.
			break;
		
	case EMG_DECRYPTION:
			ret_code = emg_gps_decryption_plugin(wg_flag, wg_lng, wg_lat, wg_heit, wg_week, wg_time, china_lng, china_lat);
			break;
		
	case GVISION_DECRYPTION:
			// TODO...
			break;

		default:
			break;
	}
	
	return ret_code;
}
