#ifndef __ROBOTSELECTINIT_H
#define __ROBOTSELECTINIT_H
#include "sys.h"
#include "system.h"

typedef struct  
{
  float WheelSpacing;      //轮距
  float AxleSpacing;       //轴距  
  int GearRatio;         //电机减速比
  int EncoderAccuracy;   //编码器精度
  float WheelDiameter;     //轮径
  float OmniTurnRadiaus;   //全向轮旋转半径
}Robot_Parament_InitTypeDef;

typedef struct  
{
  int A;      
  int B; 
	int C; 
	int D; 
}Encoder;

//轮距 麦轮是一半
#define MEC_wheelspacing         0.109
#define Akm_wheelspacing         0.155f
#define Diff_wheelSpacing        0.155f
#define Four_Mortor_wheelSpacing 0.187f
#define Tank_wheelSpacing        0.235f

//轴距 麦轮是一半
#define MEC_axlespacing           0.085
#define Akm_axlespacing           0.143f
#define Diff_axlespacing          0.155f
#define Four_Mortor__axlespacing  0.173f
#define Tank_axlespacing          0.222f

//电机减速比
#define   HALL_30F    30
#define   MD36N_5_18  5.18
#define   MD36N_27    27
#define   MD36N_51    51
#define   MD36N_71    71
#define   MD60N_18    18
#define   MD60N_47    47

//编码器精度
#define		Photoelectric_500 500
#define	  Hall_13           13

//麦轮轮胎直径
#define		Mecanum_60  0.060f
#define		Mecanum_75  0.075f
#define		Mecanum_100 0.100f
#define		Mecanum_127 0.127f
#define		Mecanum_152 0.152f
 
//轮径全向轮直径系列
#define	  FullDirecion_60  0.060
#define	  FullDirecion_75  0.075
#define	  FullDirecion_127 0.127
#define	  FullDirecion_152 0.152
#define	  FullDirecion_203 0.203
#define	  FullDirecion_217 0.217

//黑色轮胎、履带车轮径
#define	  Black_WheelDiameter   0.065
//#define	  Tank_WheelDiameter 0.047
#define	  Tank_WheelDiameter 0.043

//全向轮小车旋转半径
#define   Omni_Turn_Radiaus_109 0.109
#define   Omni_Turn_Radiaus_164 0.164
#define   Omni_Turn_Radiaus_180 0.180
#define   Omni_Turn_Radiaus_290 0.290

//编码器倍频数 编码器频率
#define   EncoderMultiples  4
#define   EncoderFrequency  100
#define   CONTROL_FREQUENCY 100

#define PI 3.1415f  //圆周率

void Robot_Select(void);
void Robot_Init(float wheelspacing, float axlespacing, float omni_turn_radiaus, float gearratio,float Accuracy,float tyre_diameter);

#endif
