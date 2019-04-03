#ifndef Def_ReCord
	#define Def_ReCord
		#include "NuroEngineStructs.h"

		#define RunNavidata		1
		#define	StartRoute		2
		#define DataError		3
		class ReCord //public strNSS_data
		{
			public:
				typedef struct strNSS_data {
					nuULONG    RoadClass : 16;
					nuULONG    SPTFlag: 1; //SignPost Flag
					nuULONG    Reserve: 15; //Reserve
				}strNSS_data;
				nuBOOL ReCord_Write(ROUTINGDATA *NaviData, nuLONG RunDataNumber);
				nuBOOL ReCord_Read(ROUTINGDATA *NaviData, nuLONG RunDataNumber);
				nuBOOL ReCord_ReadCoor(NUROPOINT *FCoor, NUROPOINT*TCoor, nuLONG RunDataNumber);

			private:
				nuVOID ReCord_Path(nuLONG RunDataNumber, nuTCHAR *FilePath);
				nuFILE *ReCording;
				ROUTINGDATA *routingdata;
				nuLONG	WriteNAVIDATA;
				nuLONG	RunNAVIDATA;

		};
#endif