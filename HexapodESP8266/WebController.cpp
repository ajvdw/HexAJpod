//====================================================================
//Hardware setup: Websocket Controller
//====================================================================
#include <Arduino.h>
#include "WebController.h"

extern INCONTROLSTATE   g_InControlState;
extern void updateGait(void);

//[CONSTANTS]
#define WALKMODE          0
#define TRANSLATEMODE     1
#define ROTATEMODE        2
#define SINGLELEGMODE     3
#define GPPLAYERMODE      4


#define GAITSUBMODE       11
#define SPEEDSUBMODE      12
#define BALANCESUBMODE    13
#define DOUBLEHEIGHT      14
#define TRAVELLENGTH      15

//=============================================================================
// Global - Local to this file only...
//=============================================================================

static short      g_BodyYOffset;
static short      ControlMode;
static short      WalkSubMode;
static bool       DoubleHeightOn;
static bool       DoubleTravelOn;
static bool       WalkMethod;

const char* LegName[]  = {"Right Rear",  "Right Middle",  "Right Front",  "Left Rear",  "Left Middle",  "Left Front"};

//=============================================================================
void WebController::Init(void)
{
  g_InControlState.SpeedControl = 50;
  g_BodyYOffset = 30;
  ControlMode = WALKMODE;
  WalkSubMode = GAITSUBMODE;
  DoubleHeightOn = false;
  DoubleTravelOn = false;
  WalkMethod = false;
}

void WebController::SwitchOn(void)
{
  g_InControlState.fHexOn = 1;
}

void WebController::SwitchOff(void)
{
  if (g_InControlState.fHexOn) {
    //Turn off
    g_InControlState.BodyPos.x = 0;
    g_InControlState.BodyPos.y = 0;
    g_InControlState.BodyPos.z = 0;
    g_InControlState.BodyRot1.x = 0;
    g_InControlState.BodyRot1.y = 0;
    g_InControlState.BodyRot1.z = 0;
    g_InControlState.TravelLength.x = 0;
    g_InControlState.TravelLength.z = 0;
    g_InControlState.TravelLength.y = 0;
    g_BodyYOffset = 0;
    g_InControlState.SelectedLeg = 255;
    g_InControlState.fHexOn = 0;
    g_InControlState.BalanceMode = 0;
  }
}

String WebController::Status()
{
  String g_Status = "";
  if( ControlMode == WALKMODE )
  {
    if     ( WalkSubMode == GAITSUBMODE )     g_Status = String("Walking Gait ") + g_InControlState.GaitType;
    else if ( WalkSubMode == SPEEDSUBMODE )   g_Status = String("Walking Speed ") + g_InControlState.SpeedControl;
    else if ( WalkSubMode == BALANCESUBMODE ) g_Status = String("Walking Balance ") + ((g_InControlState.BalanceMode)?"On":"Off");
    else if ( WalkSubMode == DOUBLEHEIGHT )   g_Status = String("Walking Leglift ") + ((g_InControlState.LegLiftHeight > 60)?"Double":"Single");
    else if ( WalkSubMode == TRAVELLENGTH )   g_Status = String("Walking Doublestep ") + ((DoubleTravelOn)?"On":"Off");
    else g_Status = String("Walkmode Unknown!");
  }
  else if ( ControlMode == TRANSLATEMODE )   g_Status = String("Translate, Height ") + g_BodyYOffset;
  else if ( ControlMode == ROTATEMODE )      g_Status = String("Rotate, Lift ") + ((g_BodyYOffset==0)?"Down":"Up");
  else if ( ControlMode == SINGLELEGMODE )  g_Status = String("Single Leg ") + ((g_InControlState.SelectedLeg<6)?LegName[g_InControlState.SelectedLeg]:"None");
  else g_Status = String("Mode Unknown!");
  return g_Status;
}
void WebController::ButtonRed(void)
{ // Mode
  if     ( ControlMode == WALKMODE )        ControlMode = TRANSLATEMODE;
  else if ( ControlMode == TRANSLATEMODE )   ControlMode = ROTATEMODE;
  else if ( ControlMode == ROTATEMODE )      ControlMode = SINGLELEGMODE;
  else if ( ControlMode == SINGLELEGMODE ) {
    ControlMode = WALKMODE;
    WalkSubMode = GAITSUBMODE;
  }
}

void WebController::ButtonYellow(void)
{ // Decrease

  if ( ControlMode == WALKMODE && WalkSubMode == GAITSUBMODE )
  {
    if ( g_InControlState.GaitType > 0 ) g_InControlState.GaitType--;
    updateGait();
  }
  if ( ControlMode == WALKMODE && WalkSubMode == DOUBLEHEIGHT )
  {
    g_InControlState.LegLiftHeight = 50;
  }
  if ( ControlMode == WALKMODE && WalkSubMode == SPEEDSUBMODE )
  {
    g_InControlState.SpeedControl -= 50;
    if ( g_InControlState.SpeedControl < 0 ) g_InControlState.SpeedControl = 0;
  }
  if ( ControlMode == WALKMODE && WalkSubMode == TRAVELLENGTH )
  {
    DoubleTravelOn = false;
  }
  if ( ControlMode == WALKMODE && WalkSubMode == BALANCESUBMODE )
  {
    g_InControlState.BalanceMode = 0;
  }
  if ( ControlMode == SINGLELEGMODE )
  {
    if (g_InControlState.SelectedLeg == 255 )
      g_InControlState.SelectedLeg = 5 ;
    else if (g_InControlState.SelectedLeg > 0)
      g_InControlState.SelectedLeg--;
    else if( g_InControlState.SelectedLeg == 0 )
      g_InControlState.SelectedLeg = 255;
  }
  if ( ControlMode == TRANSLATEMODE )
  { // Full Lift Up
    g_BodyYOffset -= 10;
    if ( g_BodyYOffset < 0 ) g_BodyYOffset = 0;
  }
  if ( ControlMode == ROTATEMODE )
  { // Full Lift Down
    g_BodyYOffset = 0;
  }
    // Body lift
  g_InControlState.BodyPos.y = g_BodyYOffset;
}


void WebController::ButtonGreen(void)
{ // Submode
  if ( ControlMode == WALKMODE )
  {
    if     ( WalkSubMode == GAITSUBMODE )    WalkSubMode = SPEEDSUBMODE;
    else if ( WalkSubMode == SPEEDSUBMODE )   WalkSubMode = BALANCESUBMODE;
    else if ( WalkSubMode == BALANCESUBMODE ) WalkSubMode = DOUBLEHEIGHT;
    else if ( WalkSubMode == DOUBLEHEIGHT )   WalkSubMode = TRAVELLENGTH;
    else if ( WalkSubMode == TRAVELLENGTH )   WalkSubMode = GAITSUBMODE;
  }
}

void WebController::ButtonBlue(void)
{
  // Increase
  if ( ControlMode == WALKMODE && WalkSubMode == GAITSUBMODE )
  {
    if ( g_InControlState.GaitType < 4 )g_InControlState.GaitType++;
    updateGait();
  }
  if ( ControlMode == WALKMODE && WalkSubMode == DOUBLEHEIGHT )
  {
    g_InControlState.LegLiftHeight = 80;
  }
  if ( ControlMode == WALKMODE && WalkSubMode == SPEEDSUBMODE )
  {
    g_InControlState.SpeedControl += 50;
    if ( g_InControlState.SpeedControl > 2000 ) g_InControlState.SpeedControl = 2000;
  }
  if ( ControlMode == WALKMODE && WalkSubMode == TRAVELLENGTH )
  {
    DoubleTravelOn = true;
  }
  if ( ControlMode == WALKMODE && WalkSubMode == BALANCESUBMODE )
  {
    g_InControlState.BalanceMode = 1;
  }  if ( ControlMode == SINGLELEGMODE )
  {
    if (g_InControlState.SelectedLeg == 255 )
      g_InControlState.SelectedLeg =0 ;
    else if (g_InControlState.SelectedLeg < 5)
      g_InControlState.SelectedLeg++;
    else if (g_InControlState.SelectedLeg == 5 )
      g_InControlState.SelectedLeg = 255;    
  }
  if ( ControlMode == TRANSLATEMODE )
  { // Full Lift Up
    g_BodyYOffset += 10;
    if ( g_BodyYOffset > 60 ) g_BodyYOffset = 60;
  }
  if ( ControlMode == ROTATEMODE )
  { // Full Lift Up
    g_BodyYOffset = 60;
  }

  // Body lift
  g_InControlState.BodyPos.y = g_BodyYOffset;
}

//==============================================================================
// This is The main code to input function to process inputs from the WebClient and then
// process any commands.
//==============================================================================
void WebController::JoystickMove(int joyX, int joyY, int joyZ  )
{
  if ( !g_InControlState.fHexOn ) return;

  // Body Move
  if (ControlMode == WALKMODE) {
    if (DoubleTravelOn)
    {
      g_InControlState.TravelLength.x = -(joyX);
      g_InControlState.TravelLength.z = -(joyY);
      g_InControlState.TravelLength.y = -(joyZ)/4;
    }
    else
    {
      g_InControlState.TravelLength.x = -(joyX)/2;
      g_InControlState.TravelLength.z = -(joyY)/2;
      g_InControlState.TravelLength.y = -(joyZ)/4;
    }
  }

  // Body Translate
  if (ControlMode == TRANSLATEMODE) {
    g_InControlState.BodyPos.x  = (joyX)/2;
    g_InControlState.BodyPos.z  =-(joyY)/3;
    g_InControlState.BodyRot1.y = (joyZ)*2;
  }

  // Body rotate
  if (ControlMode == ROTATEMODE) {
    g_InControlState.BodyRot1.x = (joyX)/2;
    g_InControlState.BodyRot1.z =-(joyY)/2;
    g_InControlState.BodyRot1.y = (joyZ)/2;
  }

  // Single Leg move
  if (ControlMode == SINGLELEGMODE) {
    g_InControlState.SLLeg.x = (joyX)/2;
    g_InControlState.SLLeg.z = (joyY)/2;
    g_InControlState.SLLeg.y = (joyZ)/10;
  }

  //Calculate walking time delay
  g_InControlState.InputTimeDelay = 128 - max(max(abs(joyX),abs(joyY)),abs(joyZ));
}

