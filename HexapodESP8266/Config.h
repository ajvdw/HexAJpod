//====================================================================
//Project Lynxmotion
//Description: 
// Phoenix hardware configuration file for the Hex Robot.
//
// Date: March 18, 2012
// Programmer: Kurt (aka KurtE)
//
//==================================================================================================================================
#ifndef _CONFIG_h_
#define _CONFIG_h_

// Something's fishy with the max,min,abs stuff in Arduino 8266 Libs
#undef min
#undef max
#undef abs

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define abs(a) ((a < 0 )?(-a):(a))

#define NUM_GAITS    5

//=============================================================================
//[CONSTANTS]
#define BUTTON_DOWN 0
#define BUTTON_UP   1

#define SOUND_PIN   99 //unknown

#define c1DEC   10
#define c2DEC   100
#define c4DEC   10000
#define c6DEC   1000000

#define cRR     0
#define cRM     1
#define cRF     2
#define cLR     3
#define cLM     4
#define cLF     5


//--------------------------------------------------------------------
//[MIN/MAX ANGLES]
#define cRRCoxaMin1     -650      //Mechanical limits of the Right Rear Leg
#define cRRCoxaMax1     650
#define cRRFemurMin1    -900 //  -1050
#define cRRFemurMax1     900 //   750
#define cRRTibiaMin1    -900  // -530
#define cRRTibiaMax1    900

#define cRMCoxaMin1     -650      //Mechanical limits of the Right Middle Leg
#define cRMCoxaMax1     650
#define cRMFemurMin1    -900
#define cRMFemurMax1    900
#define cRMTibiaMin1    -900
#define cRMTibiaMax1    900

#define cRFCoxaMin1     -650      //Mechanical limits of the Right Front Leg
#define cRFCoxaMax1     650
#define cRFFemurMin1    -900
#define cRFFemurMax1    900
#define cRFTibiaMin1    -900
#define cRFTibiaMax1    900

#define cLRCoxaMin1     -650      //Mechanical limits of the Left Rear Leg
#define cLRCoxaMax1     650
#define cLRFemurMin1    -900
#define cLRFemurMax1    900
#define cLRTibiaMin1    -900
#define cLRTibiaMax1    900

#define cLMCoxaMin1     -650      //Mechanical limits of the Left Middle Leg
#define cLMCoxaMax1     650
#define cLMFemurMin1    -900
#define cLMFemurMax1    900
#define cLMTibiaMin1    -900
#define cLMTibiaMax1    900

#define cLFCoxaMin1     -650      //Mechanical limits of the Left Front Leg
#define cLFCoxaMax1     650
#define cLFFemurMin1    -900
#define cLFFemurMax1    900
#define cLFTibiaMin1    -900
#define cLFTibiaMax1    900

//--------------------------------------------------------------------
//[LEG DIMENSIONS]
//Universal dimensions for each leg in mm
#define cXXCoxaLength     45    // 29
#define cXXFemurLength    38    // 57  => 38
#define cXXTibiaLength    90    // 141 => 65-70 WAS 90


#define cRRCoxaLength     cXXCoxaLength	    //Right Rear leg
#define cRRFemurLength    cXXFemurLength
#define cRRTibiaLength    cXXTibiaLength

#define cRMCoxaLength     cXXCoxaLength	    //Right middle leg
#define cRMFemurLength    cXXFemurLength
#define cRMTibiaLength    cXXTibiaLength

#define cRFCoxaLength     cXXCoxaLength	    //Rigth front leg
#define cRFFemurLength    cXXFemurLength
#define cRFTibiaLength    cXXTibiaLength

#define cLRCoxaLength     cXXCoxaLength	    //Left Rear leg
#define cLRFemurLength    cXXFemurLength
#define cLRTibiaLength    cXXTibiaLength

#define cLMCoxaLength     cXXCoxaLength	    //Left middle leg
#define cLMFemurLength    cXXFemurLength
#define cLMTibiaLength    cXXTibiaLength

#define cLFCoxaLength     cXXCoxaLength	    //Left front leg
#define cLFFemurLength    cXXFemurLength
#define cLFTibiaLength    cXXTibiaLength


//--------------------------------------------------------------------
//[BODY DIMENSIONS]
#define cRRCoxaAngle1   -600   //Default Coxa setup angle, decimals = 1
#define cRMCoxaAngle1    0     //Default Coxa setup angle, decimals = 1
#define cRFCoxaAngle1    600   //Default Coxa setup angle, decimals = 1
#define cLRCoxaAngle1    -600  //Default Coxa setup angle, decimals = 1
#define cLMCoxaAngle1    0     //Default Coxa setup angle, decimals = 1
#define cLFCoxaAngle1    600   //Default Coxa setup angle, decimals = 1

#define cRROffsetX      -32    //Distance X from center of the body to the Right Rear coxa
#define cRROffsetZ      54     //Distance Z from center of the body to the Right Rear coxa
#define cRMOffsetX      -68    //Distance X from center of the body to the Right Middle coxa
#define cRMOffsetZ      0      //Distance Z from center of the body to the Right Middle coxa
#define cRFOffsetX      -32    //Distance X from center of the body to the Right Front coxa
#define cRFOffsetZ      -54    //Distance Z from center of the body to the Right Front coxa

#define cLROffsetX      32     //Distance X from center of the body to the Left Rear coxa
#define cLROffsetZ      54     //Distance Z from center of the body to the Left Rear coxa
#define cLMOffsetX      68     //Distance X from center of the body to the Left Middle coxa
#define cLMOffsetZ      0      //Distance Z from center of the body to the Left Middle coxa
#define cLFOffsetX      32     //Distance X from center of the body to the Left Front coxa
#define cLFOffsetZ      -54    //Distance Z from center of the body to the Left Front coxa

//--------------------------------------------------------------------
//[START POSITIONS FEET]
#define cHexInitXZ	      90 //
#define CHexInitXZCos60   45 //   cos(60) = .5
#define CHexInitXZSin60   78 //   sin(60) = .866
#define CHexInitY	        50 //

#define cRRInitPosX     CHexInitXZCos60       //Start positions of the Right Rear leg
#define cRRInitPosY     CHexInitY
#define cRRInitPosZ     CHexInitXZSin60

#define cRMInitPosX     cHexInitXZ            //Start positions of the Right Middle leg
#define cRMInitPosY     CHexInitY
#define cRMInitPosZ     0

#define cRFInitPosX     CHexInitXZCos60       //Start positions of the Right Front leg
#define cRFInitPosY     CHexInitY
#define cRFInitPosZ     -CHexInitXZSin60

#define cLRInitPosX     CHexInitXZCos60       //Start positions of the Left Rear leg
#define cLRInitPosY     CHexInitY
#define cLRInitPosZ     CHexInitXZSin60

#define cLMInitPosX     cHexInitXZ            //Start positions of the Left Middle leg
#define cLMInitPosY     CHexInitY
#define cLMInitPosZ     0

#define cLFInitPosX     CHexInitXZCos60       //Start positions of the Left Front leg
#define cLFInitPosY     CHexInitY
#define cLFInitPosZ     -CHexInitXZSin60

#endif

