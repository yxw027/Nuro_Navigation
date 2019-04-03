#ifndef __NURO_TTS_DEFINE_20080417
#define __NURO_TTS_DEFINE_20080447

#include "NuroNaviConst.h"

#define DIS_FOLDER					nuTEXT("\\Distance\\")
#define DIS_FILE					nuTEXT("\\Distance.txt")
#define TURN_FOLDER					nuTEXT("\\Turning\\")
#define TURN_FILE					nuTEXT("\\Turning.txt")
#define ROAD_FOLDER					nuTEXT("\\RoadType\\")
#define ROAD_FILE					nuTEXT("\\RoadType.txt")
#define OTHER_FOLDER				nuTEXT("\\Others\\")
#define OTHER_FILE					nuTEXT("\\Others.txt")

#define NAME_LINE_NUM_IN_FILE		100
#define NAME_TTS_MAX				500

#define SOUND_IGNORE				0

#define ROAD_ENTER					0

#define ROAD_TYPE_IN_CIRCLE						AF_In_TrafficCircle	
#define ROAD_TYPE_OUT_CIRCLE					AF_Out_TrafficCircle
#define ROAD_TYPE_REACH_TARGET					AF_EndSite
#define ROAD_TYPE_OUT_FREEWAY					AF_Out_Freeway
#define ROAD_TYPE_OUT_FASTWAY                   AF_Out_Fastway
#define ROAD_TYPE_OUT_OVERPASS					AF_Out_Overpass
#define ROAD_TYPE_IN_SLIPROAD					AF_In_SlipRoad
#define ROAD_TYPE_OUT_SLIPROAD                  AF_Out_SlipRoad
//Others' voice
#define O_IGNORE							0
#define O_RANDOM0							1
#define O_RANDOM1							2
#define O_RANDOM2							3

#define O_WELCOME							5
#define O_NAVISTART							6
#define O_REROUTE							7
#define O_GPSREADY							8
#define O_UTURN								9//O_NAVIDEPARTURE
#define O_NAVIDEPARTURE						10
#define O_NAVIEND							11	
#define O_CCDFIND							12	
#define O_OVERSPEED							13	
#define O_EEYESFIND							14
#define O_SOUNDNAVITTS						15
#define O_REACH_DESTINATION					16
#define O_LEFT_DESTINATION					17
#define	O_RIGHT_DESTINATION					18



#endif