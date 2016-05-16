//==============================================================================
// WebController.h 
//==============================================================================
#ifndef _WEB_CONTROLLER_h_
#define _WEB_CONTROLLER_h_

class WebController {
  public:
    void     Init(void);
    void     SwitchOn(void);
    void     SwitchOff(void);
    void     ButtonRed(void);
    void     ButtonYellow(void);
    void     ButtonGreen(void);
    void     ButtonBlue(void);
    void     JoystickMove(int joyX, int joyY, int joyZ  );
    String   Status();
} ;   

typedef struct _Coord3D {
    long      x;
    long      y;
    long      z;
} COORD3D;


//==============================================================================
// class ControlState: This is the main structure of data that the Control 
//      manipulates and is used by the main Phoenix Code to make it do what is
//      requested.
//==============================================================================
typedef struct _InControlState {
   bool		fHexOn;				  //Switch to turn on Phoenix
   bool		fPrev_HexOn;	  //Previous loop state 

   //Body position
   COORD3D    BodyPos;

   //Body Inverse Kinematics
   COORD3D  BodyRot1;         //X -Pitch, Y-Rotation, Z-Roll

    //[gait]
   short		GaitType;			    //Gait type
   short		LegLiftHeight;		//Current Travel height
   COORD3D  TravelLength;     //X-Z or Length, Y is rotation.

   //[Single Leg Control]
   short	SelectedLeg;
   COORD3D  SLLeg;            // 
   bool  fSLHold;		 	    //Single leg control mode

   //[Balance]
   bool   BalanceMode;

   //[TIMING]
   short	InputTimeDelay;	//Delay that depends on the input to get the "sneaking" effect
   short	SpeedControl;	//Adjustible Delay
} INCONTROLSTATE;

#endif //_WEB_CONTROLLER_h_
