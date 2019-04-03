#ifndef __NURO_NAVIGATION_CONSTANTS
#define __NURO_NAVIGATION_CONSTANTS

#define VRT_Defway						0//General Road						¤@¯ë¹D¸ô
#define VRT_Freeway						1//Freeway							°ª³t¤½¸ô
#define VRT_Fastway						2//Fastway							§Ö³t¹D¸ô
#define VRT_SlipRoad					3//Slip Road						¥æ¬y¹D
#define VRT_Overpass					4//Overpass							°ª¬[¾ô
#define VRT_Tunnel						5//Tunnel							ÀG¹D
#define VRT_Underpass					6//Underpass						¦a¤U¹D
#define VRT_UnOverpass					7//Road Under Overpass				°ª¬[¾ô¤U¹D¸ô
#define VRT_SurroundWay					8//Road Surrond the City			¥~Àô¹D
#define VRT_StopcockWay					9//Slip Road(Diff Name)				¥`¹D
#define VRT_GuideWay					10//Road along MajorRoad 			¤Þ¹D
#define VRT_3DSlipRoad					11//Overpass Slip Road				¥ß¥æ¾ô
#define VRT_Tollgate					20//Tollgate						¦¬¶O¯¸
#define VRT_RestStop					21//Reststop						¥ð®§°Ï
#define VRT_HasSlipRoad					22//warn user here has Slip Road	¦³¥æ¬y¹D
#define	VRT_ScanSpeed					23//´ú³t·Ó¬Û
#define VRT_IN_FASTROAD					27
#define	VRT_IN_SLOWROAD					28 
#define VRT_TrafficCircle				30//Traffic Circle					¶êÀô
#define VRT_ROADCROSS					31
#define VRT_FastToSlow                  33//(¿ì?Âý) for panasonic
#define VRT_SlowToFast					34//(Âý?¿ì) for panasonic
#define VRT_SlowWay						40//:Âý?µÀ
#define VRT_UTURN						41
#define VRT_LTURN						42
#define VRT_RTURN						43
#define VRT_FERRY						50 //°Ú¶É
#define VRT_Bridge                      51//¾ô±ç
#define VRT_StartSite					91//Start Site that u set set		°_ÂI
#define VRT_CityTownBoundary			96//City or Town Boundary			¶mÂíÃä¹Ò
#define VRT_MapBoundary					97//Near Map Boundary				¦a¹ÏÃä¹Ò
#define VRT_MidSite						98//Middle Site that uset set		¸g¥ÑÂI
#define VRT_EndSite						99//End Site that user set			²×ÂI
//TurnFlag in RoutingData 
#define T_No						    0//µL°Ê§@
#define T_Afore							1//ª½¦æ
#define T_Left						    2//¥ªÂà
#define T_Right							3//¥kÂà
#define T_LeftSide						4//§Ã¸ô¦V¥ª
#define T_RightSide						5//§Ã¸ô¦V¥k
#define T_MidWay						6//¤¤½u
#define T_LUturn						7//¥ª°jÂà
#define T_RUturn						8//¥k°jÂà
#define T_Direction						9//¨Ì«ü¥Ü¤è¦V
#define T_LeftLeft						18//¥ªÂà
#define T_RightRight				    19//¥kÂà
#define T_LeftLeftSide                  20//§Ã¸ô¦V¥ª
#define T_RightRightSide				21//§Ã¸ô¦V¥k
#define T_LULUturn						22//¥ª°jÂà
#define T_RURUturn						23//¥k°jÂà
//--------------------------------------------------------------
#define AF_NoPlay						9999//(§¹¥þ¤£¼½³ø)
#define AF_LoseHere						0//(¥u¼½³ø¶ZÂ÷»P¤è¦V)
#define AF_In_SlipRoad					1//¶i¤J¥æ¬y¹D
#define AF_In_Underpass					2//¶i¤J¦a¤U¹D
#define AF_In_Fastway					3//¶i¤J§Ö³t¹D¸ô
#define AF_In_Overpass					4//¶i¤J°ª¬[¾ô
#define AF_In_Freeway					5//¶i¤J°ª³t¤½¸ô
#define AF_In_Tunnel					6//¶i¤JÀG¹D
#define AF_Out_SlipRoad					7//¾pÂ÷¥æ¬y¹D
#define AF_Out_Underpass				8//¾pÂ÷¦a¤U¹D
#define AF_Out_Fastway					9//¾pÂ÷§Ö³t¹D¸ô
#define AF_Out_Overpass					10//¾pÂ÷°ª¬[¾ô
#define AF_Out_Freeway					11//¾pÂ÷°ª³t¤½¸ô
#define AF_Out_Tunnel					12//¾pÂ÷ÀG¹D
#define AF_In_UnOverpass				13//¨«¥­­±¹D¸ô
#define AF_In_Tollgate					14//¶i¤J¦¬¶O¯¸
#define AF_In_RestStop					15//¶i¤J¥ð®§¯¸
#define AF_Has_SlipRoad					16//¦³¥æ¬y¹D
#define	AF_MidSit						17//´ú³t·Ó¬Û
#define AF_In_TrafficCircle				18//In Traffic Circle					¶êÀô
#define AF_Out_TrafficCircle			19//Out Traffic Circle					¶êÀô
#define AF_Has_RestStop					20
#define AF_IN_FASTROAD					21
#define	AF_IN_SLOWROAD					22 
#define AF_OUT_FASTROAD					23
#define AF_OUT_SLOWROAD					24
#define AF_IN_MAINROAD					25 //½øÈëÖ÷µÀ
#define AF_IN_AUXROAD				    26  //½øÈë¸¨µÀ
#define AF_OUT_MAINROAD					27 //Àë¿ªÖ÷µÀ
#define AF_OUT_AUXROAD				    28  //Àë¿ª¸¨µÀ
#define AF_IN_Bridge                    29 //¶i¤J¾ô±ç

#define AF_CityTownBoundary				30//¨ì¹F«°¶mÃä¬É
#define AF_MapBoundary					31//¨ì¹FÃä¬É

#define AF_IN_UTURN						40
#define AF_IN_LTURN						41
#define AF_IN_RTURN						42
#define AF_IN_FERRY						43 //½øÈë°Ú¶É

//#define AF_MidSit						98//¨ì¹F¸g¥ÑÂI
#define AF_EndSite						99//¨ì¹F²×ÂI
#define AF_ChangeRoadName				100//ÅÜ´«¸ô

//Distance.txt index
#define	DIS_FOLLOW							1
#define	DIS_100M							2
#define DIS_300M							3
#define	DIS_700M							4
#define DIS_500M							5
#define DIS_1KM								6
#define DIS_2KM								7
#define DIS_IGNORE							0
#define DIS_ESTIMATION                      8
//
#define DIS_DRIVE                           9
#define DIS_PREPARE                         10
//fastway
#define  VOICEDIS2000_E						1850
#define  VOICEDIS2000_S						2150
#define  VOICEDIS1000_E						850
#define  VOICEDIS1000_S						1150
#define  VOICEDIS500_E_FastWay				400
#define  VOICEDIS500_S_FastWay				600
//
#define  VOICEDIS500_E_SlowWay				400
#define  VOICEDIS500_S_SlowWay				600
#define  VOICEDIS300_E						250
#define  VOICEDIS300_S						350
#define  VOICEDIS100_E						0
#define  VOICEDIS100_S						150 

//
#define  IN_REAL3DPIC						600
#define  IN_CROSSDIS_LEVEL_1				300
#define	 IN_CROSSDIS_LEVEL_2				150
#define  OUT_CROSSDIS_LEVEL_1				50
#define  OUT_CROSSDIS_LEVEL_2				100
//
#define  NO_ACTION							0
#define  IN_LEVEL_1							1
#define  IN_LEVEL_2							2
#define  OUT_LEVEL_1						3
#define	 OUT_LEVEL_2						4

//
#define  DIS_BETWEENINGORE							150
#endif

#define Elead_Roundabout_Right               1//°f®É°w
#define Elead_Roundabout_Left				 2//¶¶®É°w
#define Elead_In_TrafficCircle               3//¾aªñ¶êÀô
#define Elead_Straight						 4//ª½¦æ
#define Elead_Slight_Left					 5//¥ª«e¤èÂàÅs
#define Elead_Slight_Right					 6//¥k«e¤èÂàÅs
#define Elead_Keep_Left						 7//§Ã¸ô¦V¥ª
#define Elead_Keep_Right					 8//§Ã¸ô¦V¥k
#define Elead_Left							 9//¥ªÂà
#define Elead_Right							10//¥kÂà
#define Elead_Hard_Left						11//¥ª«á¤èÂàÅs
#define Elead_Hard_Right					12//¥k«á¤èÂàÅs
#define Elead_Uturn_Left					13//¥ª°jÂà
#define Elead_Uturn_Right					14//¥k°jÂà
#define Elead_Destination                   15//¨ì¹F¥Øªº¦a
#define Elead_LeaveNaviLine					16//°¾Â÷¾É¯è¸ô½u
#define Elead_Enter_Bridge                  17//¶i¤J¾ô±ç
#define Elead_Enterfreeway_Left             18//¥ª´ú¶i¤J°ª³t¤½¸ô
#define Elead_Enterfreeway_Right            19//¥k°¼¶i¤J°ª³t¤½¸ô
#define Elead_Exitfreeway_Left              20//¥ª°¼Â÷¶}°ª³t¤½¸ô
#define Elead_Exitfreeway_Right             21//¥k°¼Â÷¶}°ª³t¤½¸ô
#define Elead_Toll_Booth                    22//¦¬¶O¯¸
#define Elead_Enter_Tunnel                  23//¶i¤JÀG¹D
#define Elead_Enterferry_Left               24//¥ªÃä¶i¤J´ç½ü
#define Elead_Enterferry_Right              25//¥kÃä¶i¤J´ç½ü
#define Elead_Exitferry_Left                26//¥ªÃäÂ÷¶}´ç½ü
#define Elead_Exitferry_Right               27//¥kÃäÂ÷¶}´ç½ü
#define Elead_Exitferry                     28//¾aªñ´ç½ü
#define Elead_Takeramp_Right                29//¥kÃä¶i¤J¥`¹D
#define Elead_Takeramp_Left                 30//¥ªÃä¶i¤J¥`¹D