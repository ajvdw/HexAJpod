//=============================================================================
//Project Lynxmotion Phoenix
//Description: Phoenix software
//Software version: V2.0
//Date: 29-10-2009
//Programmer: Jeroen Janssen [aka Xan]
//            Kurt Eckhardt(KurtE) converted to C and Arduino
//
// This version of the Phoenix code was ported to ESP8266 by AJ
//
//KNOWN BUGS:
//    - Lots ;)
//
//=============================================================================
#ifndef _PHOENIX_h_
#define _PHOENIX_h_

#include <Arduino.h>
#include "Config.h"

#define BalanceDivFactor 6    //;Other values than 6 can be used, testing...CAUTION!! At your own risk ;)


//--------------------------------------------------------------------
//[TABLES]
//ArcCosinus Table
//Table build in to 3 part to get higher accuracy near cos = 1. 
//The biggest error is near cos = 1 and has a biggest value of 3*0.012098rad = 0.521 deg.
//-    Cos 0 to 0.9 is done by steps of 0.0079 rad. [1/127]
//-    Cos 0.9 to 0.99 is done by steps of 0.0008 rad [0.1/127]
//-    Cos 0.99 to 1 is done by step of 0.0002 rad [0.01/64]
//Since the tables are overlapping the full range of 127+127+64 is not necessary. Total bytes: 277

static const byte GetACos[] = {    
  255,254,252,251,250,249,247,246,245,243,242,241,240,238,237,236,234,233,232,231,229,228,227,225, 
  224,223,221,220,219,217,216,215,214,212,211,210,208,207,206,204,203,201,200,199,197,196,195,193, 
  192,190,189,188,186,185,183,182,181,179,178,176,175,173,172,170,169,167,166,164,163,161,160,158, 
  157,155,154,152,150,149,147,146,144,142,141,139,137,135,134,132,130,128,127,125,123,121,119,117, 
  115,113,111,109,107,105,103,101,98,96,94,92,89,87,84,81,79,76,73,73,73,72,72,72,71,71,71,70,70, 
  70,70,69,69,69,68,68,68,67,67,67,66,66,66,65,65,65,64,64,64,63,63,63,62,62,62,61,61,61,60,60,59,
  59,59,58,58,58,57,57,57,56,56,55,55,55,54,54,53,53,53,52,52,51,51,51,50,50,49,49,48,48,47,47,47,
  46,46,45,45,44,44,43,43,42,42,41,41,40,40,39,39,38,37,37,36,36,35,34,34,33,33,32,31,31,30,29,28,
  28,27,26,25,24,23,23,23,23,22,22,22,22,21,21,21,21,20,20,20,19,19,19,19,18,18,18,17,17,17,17,16,
  16,16,15,15,15,14,14,13,13,13,12,12,11,11,10,10,9,9,8,7,6,6,5,3,0 };//
                    
//Sin table 90 deg, persision 0.5 deg [180 values]
static const word GetSin[]  = {
  0, 87, 174, 261, 348, 436, 523, 610, 697, 784, 871, 958, 1045, 1132, 1218, 1305, 1391, 1478, 1564, 
  1650, 1736, 1822, 1908, 1993, 2079, 2164, 2249, 2334, 2419, 2503, 2588, 2672, 2756, 2840, 2923, 3007, 
  3090, 3173, 3255, 3338, 3420, 3502, 3583, 3665, 3746, 3826, 3907, 3987, 4067, 4146, 4226, 4305, 4383, 
  4461, 4539, 4617, 4694, 4771, 4848, 4924, 4999, 5075, 5150, 5224, 5299, 5372, 5446, 5519, 5591, 5664, 
  5735, 5807, 5877, 5948, 6018, 6087, 6156, 6225, 6293, 6360, 6427, 6494, 6560, 6626, 6691, 6755, 6819, 
  6883, 6946, 7009, 7071, 7132, 7193, 7253, 7313, 7372, 7431, 7489, 7547, 7604, 7660, 7716, 7771, 7826, 
  7880, 7933, 7986, 8038, 8090, 8141, 8191, 8241, 8290, 8338, 8386, 8433, 8480, 8526, 8571, 8616, 8660, 
  8703, 8746, 8788, 8829, 8870, 8910, 8949, 8987, 9025, 9063, 9099, 9135, 9170, 9205, 9238, 9271, 9304, 
  9335, 9366, 9396, 9426, 9455, 9483, 9510, 9537, 9563, 9588, 9612, 9636, 9659, 9681, 9702, 9723, 9743, 
  9762, 9781, 9799, 9816, 9832, 9848, 9862, 9876, 9890, 9902, 9914, 9925, 9935, 9945, 9953, 9961, 9969, 
  9975, 9981, 9986, 9990, 9993, 9996, 9998, 9999, 10000 };//

//Build tables for Leg configuration like I/O and MIN/imax values to easy access values using a FOR loop
//Constants are still defined as single values in the cfg file to make it easy to read/configure

// Servo Horn offsets
#ifdef cRRFemurHornOffset1   // per leg configuration
static const short cFemurHornOffset1[] = {cRRFemurHornOffset1,  cRMFemurHornOffset1,  cRFFemurHornOffset1,  cLRFemurHornOffset1,  cLMFemurHornOffset1,  cLFFemurHornOffset1};
#define CFEMURHORNOFFSET1(LEGI) ((short)cFemurHornOffset1[LEGI])
#else   // Fixed per leg, if not defined 0
#ifndef cFemurHornOffset1
#define cFemurHornOffset1  0
#endif
#define CFEMURHORNOFFSET1(LEGI)  (cFemurHornOffset1)
#endif

#define cPWMC 375  // Servo center PWM
#define cPWMD 225  // Servo PWM for 90 deg

#define cTravelDeadZone         4    //The deadzone for the analog input from the remote

//Leg Lengths
const byte cCoxaLength[]  = {cRRCoxaLength,  cRMCoxaLength,  cRFCoxaLength,  cLRCoxaLength,  cLMCoxaLength,  cLFCoxaLength};
const byte cFemurLength[] = {cRRFemurLength, cRMFemurLength, cRFFemurLength, cLRFemurLength, cLMFemurLength, cLFFemurLength};
const byte cTibiaLength[] = {cRRTibiaLength, cRMTibiaLength, cRFTibiaLength, cLRTibiaLength, cLMTibiaLength, cLFTibiaLength};

//Body Offsets [distance between the center of the body and the center of the coxa]
const short cOffsetX[] = {cRROffsetX, cRMOffsetX, cRFOffsetX, cLROffsetX, cLMOffsetX, cLFOffsetX};
const short cOffsetZ[] = {cRROffsetZ, cRMOffsetZ, cRFOffsetZ, cLROffsetZ, cLMOffsetZ, cLFOffsetZ};

//Default leg angle
const short cCoxaAngle1[] = {cRRCoxaAngle1, cRMCoxaAngle1, cRFCoxaAngle1, cLRCoxaAngle1, cLMCoxaAngle1, cLFCoxaAngle1};

//Start positions for the leg
const short cInitPosX[] = {cRRInitPosX, cRMInitPosX, cRFInitPosX, cLRInitPosX, cLMInitPosX, cLFInitPosX};
const short cInitPosY[] = {cRRInitPosY, cRMInitPosY, cRFInitPosY, cLRInitPosY, cLMInitPosY, cLFInitPosY};
const short cInitPosZ[] = {cRRInitPosZ, cRMInitPosZ, cRFInitPosZ, cLRInitPosZ, cLMInitPosZ, cLFInitPosZ};

class Phoenix {
public:
  void Init(void);
  void loop(void);  
  void StartUpdateServos(void);
  String StatusJSON(void);
  void GaitSelect(void);

  //[SERVO PARMS] 
  // Actual angles
  short CoxaAngle1[6];    //Actual Angle of the horizontal hip, decimals = 1
  short FemurAngle1[6];   //Actual Angle of the vertical hip, decimals = 1
  short TibiaAngle1[6];   //Actual Angle of the knee, decimals = 1
  //Mechanical limits
  short cCoxaMin1[6]  = {cRRCoxaMin1,  cRMCoxaMin1,  cRFCoxaMin1,  cLRCoxaMin1,  cLMCoxaMin1,  cLFCoxaMin1};
  short cCoxaMax1[6]  = {cRRCoxaMax1,  cRMCoxaMax1,  cRFCoxaMax1,  cLRCoxaMax1,  cLMCoxaMax1,  cLFCoxaMax1};
  short cFemurMin1[6] = {cRRFemurMin1, cRMFemurMin1, cRFFemurMin1, cLRFemurMin1, cLMFemurMin1, cLFFemurMin1};
  short cFemurMax1[6] = {cRRFemurMax1, cRMFemurMax1, cRFFemurMax1, cLRFemurMax1, cLMFemurMax1, cLFFemurMax1};
  short cTibiaMin1[6] = {cRRTibiaMin1, cRMTibiaMin1, cRFTibiaMin1, cLRTibiaMin1, cLMTibiaMin1, cLFTibiaMin1};
  short cTibiaMax1[6] = {cRRTibiaMax1, cRMTibiaMax1, cRFTibiaMax1, cLRTibiaMax1, cLMTibiaMax1, cLFTibiaMax1};
  //Servo Physics
  short cCoxaPWMC[6]  = {cPWMC,  cPWMC,  cPWMC,  cPWMC,  cPWMC,  cPWMC};
  short cCoxaPWMD[6]  = {cPWMD,  cPWMD,  cPWMD,  -cPWMD,  -cPWMD,  -cPWMD};
  short cFemurPWMC[6] = {cPWMC,  cPWMC,  cPWMC,  cPWMC,  cPWMC,  cPWMC};
  short cFemurPWMD[6] = {cPWMD,  cPWMD,  cPWMD,  -cPWMD,  -cPWMD,  -cPWMD};
  short cTibiaPWMC[6] = {cPWMC,  cPWMC,  cPWMC,  cPWMC,  cPWMC,  cPWMC};
  short cTibiaPWMD[6] = {cPWMD,  cPWMD,  cPWMD,  -cPWMD,  -cPWMD,  -cPWMD};
    
private:
  // Function prototypes
  void SingleLegControl(void);
  void GaitSeq(void);
  void BalanceBody(void);
  void CheckAngles();
  void BalCalcOneLeg (short PosX, short PosZ, short PosY, byte BalLegNr);
  void BodyFK (short PosX, short PosZ, short PosY, short RotationY, byte BodyIKLeg) ;
  void LegIK (short IKFeetPosX, short IKFeetPosY, short IKFeetPosZ, byte LegIKLegNr);
  void Gait (byte GaitCurrentLegNr);
  short GetATan2 (short AtanX, short AtanY);
  void GetSinCos(short AngleDeg1);
  long GetArcCos(short cos4);
  unsigned long isqrt32 (unsigned long n);
  
  // Variables  
  //====================================================================
  
  //--------------------------------------------------------------------
  //[POSITIONS SINGLE LEG CONTROL]
  short           LegPosX[6];    //Actual X Posion of the Leg
  short           LegPosY[6];    //Actual Y Posion of the Leg
  short           LegPosZ[6];    //Actual Z Posion of the Leg

  //--------------------------------------------------------------------
  //[VARIABLES]
  byte            Index;                    //Index universal used
  byte            LegIndex;                //Index used for leg Index Number
  
  //GetSinCos / ArcCos
  short           AngleDeg1;        //Input Angle in degrees, decimals = 1
  short           sin4;             //Output Sinus of the given Angle, decimals = 4
  short           cos4;            //Output Cosinus of the given Angle, decimals = 4
  short           AngleRad4;        //Output Angle in radials, decimals = 4
  
  //GetAtan2
  short           AtanX;            //Input X
  short           AtanY;            //Input Y
  short           Atan4;            //ArcTan2 output
  short           XYhyp2;            //Output presenting Hypotenuse of X and Y
  
  //Body Inverse Kinematics
  short           PosX;            //Input position of the feet X
  short           PosZ;            //Input position of the feet Z
  short           PosY;            //Input position of the feet Y
  //long          TotalX;            //Total X distance between the center of the body and the feet
  //long          TotalZ;            //Total Z distance between the center of the body and the feet
  long            BodyFKPosX;        //Output Position X of feet with Rotation
  long            BodyFKPosY;        //Output Position Y of feet with Rotation
  long            BodyFKPosZ;        //Output Position Z of feet with Rotation
  // New with zentas stuff
  short           BodyRotOffsetX;    //Input X offset value to adjust centerpoint of rotation
  short           BodyRotOffsetY;    //Input Y offset value to adjust centerpoint of rotation
  short           BodyRotOffsetZ;    //Input Z offset value to adjust centerpoint of rotation
  
  
  //Leg Inverse Kinematics
  long            IKFeetPosX;        //Input position of the Feet X
  long            IKFeetPosY;        //Input position of the Feet Y
  long            IKFeetPosZ;        //Input Position of the Feet Z
  boolean         IKSolution;        //Output true if the solution is possible
  boolean         IKSolutionWarning;    //Output true if the solution is NEARLY possible
  boolean         IKSolutionError;    //Output true if the solution is NOT possible
  //--------------------------------------------------------------------
  //[TIMING]
  unsigned long   lTimerStart;      //Start time of the calculation cycles
  unsigned long   lTimerEnd;        //End time of the calculation cycles
  byte            CycleTime;        //Total Cycle time
  
  word            ServoMoveTime;        //Time for servo updates
  word            PrevServoMoveTime;    //Previous time for the servo updates  
  //[Balance]
  long            TotalTransX;
  long            TotalTransZ;
  long            TotalTransY;
  long            TotalYBal1;
  long            TotalXBal1;
  long            TotalZBal1;
  
  //[Single Leg Control]
  byte            PrevSelectedLeg;
  boolean         AllDown;
  
  //[gait]
  short           NomGaitSpeed;   //Nominal speed of the gait
  short           TLDivFactor;         //Number of steps that a leg is on the floor while walking
  short           NrLiftedPos;         //Number of positions that a single leg is lifted [1-3]
  byte            LiftDivFactor;       //Normaly: 2, when NrLiftedPos=5: 4
  
  boolean         HalfLiftHeigth;      //If TRUE the outer positions of the ligted legs will be half height    
  
  boolean         TravelRequest;        //Temp to check if the gait is in motion
  byte            StepsInGait;         //Number of steps in gait
  
  boolean         LastLeg;             //TRUE when the current leg is the last leg of the sequence
  byte            GaitStep;            //Actual Gait step
  
  byte            GaitLegNr[6];        //Init position of the leg
  byte            GaitLegNrIn;         //Input Number of the leg
  
  long            GaitPosX[6];         //Array containing Relative X position corresponding to the Gait
  long            GaitPosY[6];         //Array containing Relative Y position corresponding to the Gait
  long            GaitPosZ[6];         //Array containing Relative Z position corresponding to the Gait
  long            GaitRotY[6];         //Array containing Relative Y rotation corresponding to the Gait
  
  boolean         fWalking;            //  True if the robot are walking
  boolean         fContinueWalking;    // should we continue to walk?
  
} ; 


#endif
