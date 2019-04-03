// nuroGpsRelease.h: interface for the nuroGpsRelease class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROGPSRELEASE_H__7DE0C360_76C7_43A2_91A3_083E9C52697B__INCLUDED_)
#define AFX_NUROGPSRELEASE_H__7DE0C360_76C7_43A2_91A3_083E9C52697B__INCLUDED_


typedef enum _DECRYPTION_TYPE {
	NO_DECRYPTION = 0,
	EMG_DECRYPTION,			// E Map Go
	GVISION_DECRYPTION      // Geo Vision
} DECRYPTION_TYPE;


unsigned int wgtochina_lb(DECRYPTION_TYPE decryption_type, int wg_flag, unsigned int wg_lng, unsigned int wg_lat, int wg_heit, int wg_week, unsigned int wg_time, unsigned  int *china_lng, unsigned int *china_lat);

#endif // !defined(AFX_NUROGPSRELEASE_H__7DE0C360_76C7_43A2_91A3_083E9C52697B__INCLUDED_)
