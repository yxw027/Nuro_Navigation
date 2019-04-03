#pragma once

#ifndef __MMF_DLL__
#define __MMF_DLL__

#ifdef __cplusplus
extern "C" {
#endif

void  tkmmf_GetInitNum(int GpsTime, int* x1, int* x2, int* x3);

int  tkmmf_CheckInitNum(int r1, int r2, int r3);

void  tkmmf_SetLimitAngle(float fAngle);

void  tkmmf_SetLimitTime(int time);

void  tkmmf_SetLimitSpeed(int speed);

void  tkmmf_ResetMMF(void);

void  tkmmf_GetPredicPos(int in_x, int in_y, float in_angle, int in_speed, float AngularChange, 
								  int* out_x, int* out_y, float* out_angle);

int  tkmmf_ModifyRtoem(int in_x, int in_y, int in_angle, float AngularChangem, int fCarSpeed, 
								int gps_angle, int gps_posX, int gps_poxY, 
								int* out_x, int* out_y, int* out_angle);

void tkmmf_WriteCOM(char* data, int iLength);
#ifdef __cplusplus
}
#endif
#endif