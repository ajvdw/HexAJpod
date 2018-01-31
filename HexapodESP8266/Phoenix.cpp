//=============================================================================
//Project Lynxmotion Phoenix
//Description: Phoenix software
//Software version: V2.0
//Date: 29-10-2009
//Programmer: Jeroen Janssen [aka Xan]
//         Kurt Eckhardt(KurtE) converted to C and Arduino
//
// This version of the Phoenix code was ported to ESP8266 by AJ
//
//KNOWN BUGS:
//    - Lots ;)
//
//=============================================================================
#include "Phoenix.h"
#include "Servodriver.h"
#include "WebController.h"

extern void backgroundProcessing(int ms);

// Define our global Input Control State object
INCONTROLSTATE   g_InControlState;      

// Define our ServoDriver objects
Servodriver  g_ServoDriverR = Servodriver(0x41);     // our global servo driver right side legs
Servodriver  g_ServoDriverL = Servodriver(0x40);     // our global servo driver left side legs

void Phoenix::Init(void)
{        
    //Tars Init Positions
    for (LegIndex= 0; LegIndex <= 5; LegIndex++ )
    {
        LegPosX[LegIndex] = cInitPosX[LegIndex];    //Set start positions for each leg
        LegPosY[LegIndex] = cInitPosY[LegIndex];
        LegPosZ[LegIndex] = cInitPosZ[LegIndex];  
    }
    
    //Single leg control. Make sure no leg is selected
    g_InControlState.SelectedLeg = 255; // No Leg selected
    PrevSelectedLeg = 255;
    
    //Body Positions
    g_InControlState.BodyPos.x = 0;
    g_InControlState.BodyPos.y = 0;
    g_InControlState.BodyPos.z = 0;
        
    //Body Rotations
    g_InControlState.BodyRot1.x = 0;
    g_InControlState.BodyRot1.y = 0;
    g_InControlState.BodyRot1.z = 0;
    
    BodyRotOffsetX = 0;
    BodyRotOffsetY = 0;        //Input Y offset value to adjust centerpoint of rotation
    BodyRotOffsetZ = 0;
    
        
    //Gait
    g_InControlState.GaitType = 2;  // 0; Devon wanted 
    g_InControlState.BalanceMode = 0;
    g_InControlState.LegLiftHeight = 50;
    GaitStep = 1;
    GaitSelect();

    g_InControlState.fHexOn = 0;
    
    // Init our ServoDrivers
    g_ServoDriverR.begin();
    g_ServoDriverR.setPWMFreq(60); 
    g_ServoDriverL.begin();
    g_ServoDriverL.setPWMFreq(60); 

    ServoMoveTime = 150;

    // Move to Initial Location
    for( byte LegIndex=0; LegIndex < 6; LegIndex++ )
    {
      CoxaAngle1[LegIndex]=0;FemurAngle1[LegIndex]=0;TibiaAngle1[LegIndex]=0;
    }
    StartUpdateServos();    
}


String Phoenix::StatusJSON(void)
{
  String data="[";
  for( int LegIndex=0; LegIndex<6; LegIndex++ )
  {
    data += String("\"") + CoxaAngle1[LegIndex] + "\",";
    data += String("\"") + FemurAngle1[LegIndex] + "\",";
    data += String("\"") + TibiaAngle1[LegIndex] + ((LegIndex<5)?"\",":"\"");
  }
  data += "]";
  return data;
}

void Phoenix::SingleLegControl(void)
{

  //Check if all legs are down
    AllDown = (LegPosY[cRF]==(short)cInitPosY[cRF]) && 
              (LegPosY[cRM]==(short)cInitPosY[cRM]) && 
              (LegPosY[cRR]==(short)cInitPosY[cRR]) && 
              (LegPosY[cLR]==(short)cInitPosY[cLR]) && 
              (LegPosY[cLM]==(short)cInitPosY[cLM]) && 
              (LegPosY[cLF]==(short)cInitPosY[cLF]);

    if (g_InControlState.SelectedLeg<=5) {
        if (g_InControlState.SelectedLeg!=PrevSelectedLeg) {
            if (AllDown) { //Lift leg a bit when it got selected
                LegPosY[g_InControlState.SelectedLeg] = (short)(cInitPosY[g_InControlState.SelectedLeg]-20);
        
                //Store current status
                 PrevSelectedLeg = g_InControlState.SelectedLeg;
            } else {//Return prev leg back to the init position
                LegPosX[PrevSelectedLeg] = (short)(cInitPosX[PrevSelectedLeg]);
                LegPosY[PrevSelectedLeg] = (short)(cInitPosY[PrevSelectedLeg]);
                LegPosZ[PrevSelectedLeg] = (short)(cInitPosZ[PrevSelectedLeg]);
            }
        } else if (!g_InControlState.fSLHold) {
            LegPosY[g_InControlState.SelectedLeg] = LegPosY[g_InControlState.SelectedLeg]+g_InControlState.SLLeg.y;
            LegPosX[g_InControlState.SelectedLeg] = (short)(cInitPosX[g_InControlState.SelectedLeg])+g_InControlState.SLLeg.x;
            LegPosZ[g_InControlState.SelectedLeg] = (short)(cInitPosZ[g_InControlState.SelectedLeg])+g_InControlState.SLLeg.z;     
        }
    } else {//All legs to init position
        if (!AllDown) {
            for(LegIndex = 0; LegIndex <= 5;LegIndex++) {
                LegPosX[LegIndex] = (short)(cInitPosX[LegIndex]);
                LegPosY[LegIndex] = (short)(cInitPosY[LegIndex]);
                LegPosZ[LegIndex] = (short)(cInitPosZ[LegIndex]);
            }
        } 
        if (PrevSelectedLeg!=255)
           PrevSelectedLeg = 255;
    }
}


//--------------------------------------------------------------------
void Phoenix::GaitSelect(void)
{
    //Gait selector
    switch (g_InControlState.GaitType)  {
        case 0:
            //Ripple Gait 12 steps
            GaitLegNr[cLR] = 1;
            GaitLegNr[cRF] = 3;
            GaitLegNr[cLM] = 5;
            GaitLegNr[cRR] = 7;
            GaitLegNr[cLF] = 9;
            GaitLegNr[cRM] = 11;
            
            NrLiftedPos = 3;
            HalfLiftHeigth = 3;
            TLDivFactor = 8;      
            StepsInGait = 12;    
            NomGaitSpeed = 70;
            break;
        case 1:
            //Tripod 8 steps
            GaitLegNr[cLR] = 5;
            GaitLegNr[cRF] = 1;
            GaitLegNr[cLM] = 1;
            GaitLegNr[cRR] = 1;
            GaitLegNr[cLF] = 5;
            GaitLegNr[cRM] = 5;
                
            NrLiftedPos = 3;
            HalfLiftHeigth = 3;
            TLDivFactor = 4;
            StepsInGait = 8; 
            NomGaitSpeed = 70;
            break;
        case 2:
            //Triple Tripod 12 step
            GaitLegNr[cRF] = 3;
            GaitLegNr[cLM] = 4;
            GaitLegNr[cRR] = 5;
            GaitLegNr[cLF] = 9;
            GaitLegNr[cRM] = 10;
            GaitLegNr[cLR] = 11;
                
            NrLiftedPos = 3;
            HalfLiftHeigth = 3;
            TLDivFactor = 8;
            StepsInGait = 12; 
            NomGaitSpeed = 60;
            break;
        case 3:
            // Triple Tripod 16 steps, use 5 lifted positions
            GaitLegNr[cRF] = 4;
            GaitLegNr[cLM] = 5;
            GaitLegNr[cRR] = 6;
            GaitLegNr[cLF] = 12;
            GaitLegNr[cRM] = 13;
            GaitLegNr[cLR] = 14;
                
            NrLiftedPos = 5;
            HalfLiftHeigth = 1;
            TLDivFactor = 10;
            StepsInGait = 16; 
            NomGaitSpeed = 60;
            break;
        case 4:
            //Wave 24 steps
            GaitLegNr[cLR] = 1;
            GaitLegNr[cRF] = 21;
            GaitLegNr[cLM] = 5;
            
            GaitLegNr[cRR] = 13;
            GaitLegNr[cLF] = 9;
            GaitLegNr[cRM] = 17;
                
            NrLiftedPos = 3;
            HalfLiftHeigth = 3;
            TLDivFactor = 20;      
            StepsInGait = 24;        
            NomGaitSpeed = 70;
            break;
    }
}    

//--------------------------------------------------------------------
//[GAIT Sequence]
void Phoenix::GaitSeq(void)
{
    //Check if the Gait is in motion
    TravelRequest = ((abs(g_InControlState.TravelLength.x)>cTravelDeadZone) || (abs(g_InControlState.TravelLength.z)>cTravelDeadZone) || (abs(g_InControlState.TravelLength.y)>cTravelDeadZone));
    if (NrLiftedPos == 5)
    LiftDivFactor = 4;    
    else  
  LiftDivFactor = 2;

   //Calculate Gait sequence
    LastLeg = 0;
    for (LegIndex = 0; LegIndex <= 5; LegIndex++) { // for all legs
        if (LegIndex == 5) // last leg
            LastLeg = 1 ;
    
        Gait(LegIndex);
    }    // next leg
}


//--------------------------------------------------------------------
//[GAIT]
void Phoenix::Gait (byte GaitCurrentLegNr)
{

    
    //Clear values under the cTravelDeadZone
    if (!TravelRequest) {    
        g_InControlState.TravelLength.x=0;
        g_InControlState.TravelLength.z=0;
        g_InControlState.TravelLength.y=0;
    }
    //Leg middle up position
    //Gait in motion                                                Gait NOT in motion, return to home position
    if ((TravelRequest && (NrLiftedPos==1 || NrLiftedPos==3 || NrLiftedPos==5) && 
            GaitStep==GaitLegNr[GaitCurrentLegNr]) || (!TravelRequest && GaitStep==GaitLegNr[GaitCurrentLegNr] && ((abs(GaitPosX[GaitCurrentLegNr])>2) || 
                (abs(GaitPosZ[GaitCurrentLegNr])>2) || (abs(GaitRotY[GaitCurrentLegNr])>2)))) { //Up
        GaitPosX[GaitCurrentLegNr] = 0;
        GaitPosY[GaitCurrentLegNr] = -g_InControlState.LegLiftHeight;
        GaitPosZ[GaitCurrentLegNr] = 0;
        GaitRotY[GaitCurrentLegNr] = 0;
    }
    //Optional Half heigth Rear (2, 3, 5 lifted positions)
    else if (((NrLiftedPos==2 && GaitStep==GaitLegNr[GaitCurrentLegNr]) || (NrLiftedPos>=3 && 
            (GaitStep==GaitLegNr[GaitCurrentLegNr]-1 || GaitStep==GaitLegNr[GaitCurrentLegNr]+(StepsInGait-1))))
            && TravelRequest) {
        GaitPosX[GaitCurrentLegNr] = -g_InControlState.TravelLength.x/LiftDivFactor;
        GaitPosY[GaitCurrentLegNr] = -3*g_InControlState.LegLiftHeight/(3+HalfLiftHeigth);     //Easier to shift between div factor: /1 (3/3), /2 (3/6) and 3/4
        GaitPosZ[GaitCurrentLegNr] = -g_InControlState.TravelLength.z/LiftDivFactor;
        GaitRotY[GaitCurrentLegNr] = -g_InControlState.TravelLength.y/LiftDivFactor;
    }    
      
    // Optional Half heigth front (2, 3, 5 lifted positions)
    else if ((NrLiftedPos>=2) && (GaitStep==GaitLegNr[GaitCurrentLegNr]+1 || GaitStep==GaitLegNr[GaitCurrentLegNr]-(StepsInGait-1)) && TravelRequest) {
        GaitPosX[GaitCurrentLegNr] = g_InControlState.TravelLength.x/LiftDivFactor;
        GaitPosY[GaitCurrentLegNr] = -3*g_InControlState.LegLiftHeight/(3+HalfLiftHeigth); // Easier to shift between div factor: /1 (3/3), /2 (3/6) and 3/4
        GaitPosZ[GaitCurrentLegNr] = g_InControlState.TravelLength.z/LiftDivFactor;
        GaitRotY[GaitCurrentLegNr] = g_InControlState.TravelLength.y/LiftDivFactor;
    }

    //Optional Half heigth Rear 5 LiftedPos (5 lifted positions)
    else if (((NrLiftedPos==5 && (GaitStep==GaitLegNr[GaitCurrentLegNr]-2 ))) && TravelRequest) {
  GaitPosX[GaitCurrentLegNr] = -g_InControlState.TravelLength.x/2;
        GaitPosY[GaitCurrentLegNr] = -g_InControlState.LegLiftHeight/2;
        GaitPosZ[GaitCurrentLegNr] = -g_InControlState.TravelLength.z/2;
        GaitRotY[GaitCurrentLegNr] = -g_InControlState.TravelLength.y/2;
     }      

    //Optional Half heigth Front 5 LiftedPos (5 lifted positions)
    else if ((NrLiftedPos==5) && (GaitStep==GaitLegNr[GaitCurrentLegNr]+2 || GaitStep==GaitLegNr[GaitCurrentLegNr]-(StepsInGait-2)) && TravelRequest) {
        GaitPosX[GaitCurrentLegNr] = g_InControlState.TravelLength.x/2;
        GaitPosY[GaitCurrentLegNr] = -g_InControlState.LegLiftHeight/2;
        GaitPosZ[GaitCurrentLegNr] = g_InControlState.TravelLength.z/2;
        GaitRotY[GaitCurrentLegNr] = g_InControlState.TravelLength.y/2;
    }

  //Leg front down position
  else if ((GaitStep==GaitLegNr[GaitCurrentLegNr]+NrLiftedPos || GaitStep==GaitLegNr[GaitCurrentLegNr]-(StepsInGait-NrLiftedPos))
            && GaitPosY[GaitCurrentLegNr]<0) {
        GaitPosX[GaitCurrentLegNr] = g_InControlState.TravelLength.x/2;
        GaitPosZ[GaitCurrentLegNr] = g_InControlState.TravelLength.z/2;
        GaitRotY[GaitCurrentLegNr] = g_InControlState.TravelLength.y/2;       
        GaitPosY[GaitCurrentLegNr] = 0; //Only move leg down at once if terrain adaption is turned off
    }

    //Move body forward      
    else {
        GaitPosX[GaitCurrentLegNr] = GaitPosX[GaitCurrentLegNr] - (g_InControlState.TravelLength.x/TLDivFactor);
        GaitPosY[GaitCurrentLegNr] = 0; 
        GaitPosZ[GaitCurrentLegNr] = GaitPosZ[GaitCurrentLegNr] - (g_InControlState.TravelLength.z/TLDivFactor);
        GaitRotY[GaitCurrentLegNr] = GaitRotY[GaitCurrentLegNr] - (g_InControlState.TravelLength.y/TLDivFactor);
    }
   

    //Advance to the next step
    if (LastLeg)  {  //The last leg in this step
        GaitStep = GaitStep+1;
        if (GaitStep>StepsInGait)
              GaitStep = 1;
    }
}  


//--------------------------------------------------------------------
//[BalCalcOneLeg]
void Phoenix::BalCalcOneLeg (short PosX, short PosZ, short PosY, byte BalLegNr)
{
    short            CPR_X;            //Final X value for centerpoint of rotation
    short            CPR_Y;            //Final Y value for centerpoint of rotation
    short            CPR_Z;            //Final Z value for centerpoint of rotation
    long             lAtan;

    //Calculating totals from center of the body to the feet
    CPR_Z = (short)(cOffsetZ[BalLegNr]) + PosZ;
    CPR_X = (short)(cOffsetX[BalLegNr]) + PosX;
    CPR_Y = 150 + PosY;        // using the value 150 to lower the centerpoint of rotation 'g_InControlState.BodyPos.y +

    TotalTransY += (long)PosY;
    TotalTransZ += (long)CPR_Z;
    TotalTransX += (long)CPR_X;
    
    lAtan = GetATan2(CPR_X, CPR_Z);
    TotalYBal1 += (lAtan*1800) / 31415;
    
    lAtan = GetATan2 (CPR_X, CPR_Y);
    TotalZBal1 += ((lAtan*1800) / 31415) -900; //Rotate balance circle 90 deg
    
    lAtan = GetATan2 (CPR_Z, CPR_Y);
    TotalXBal1 += ((lAtan*1800) / 31415) - 900; //Rotate balance circle 90 deg

}  
//--------------------------------------------------------------------
//[BalanceBody]
void Phoenix::BalanceBody(void)
{
    TotalTransZ = TotalTransZ/BalanceDivFactor ;
    TotalTransX = TotalTransX/BalanceDivFactor;
    TotalTransY = TotalTransY/BalanceDivFactor;

    if (TotalYBal1 > 0)        //Rotate balance circle by +/- 180 deg
        TotalYBal1 -=  1800;
    else
        TotalYBal1 += 1800;
    
    if (TotalZBal1 < -1800)    //Compensate for extreme balance positions that causes owerflow
        TotalZBal1 += 3600;
    
    if (TotalXBal1 < -1800)    //Compensate for extreme balance positions that causes owerflow
        TotalXBal1 += 3600;
    
    //Balance rotation
    TotalYBal1 = -TotalYBal1/BalanceDivFactor;
    TotalXBal1 = -TotalXBal1/BalanceDivFactor;
    TotalZBal1 = TotalZBal1/BalanceDivFactor;
}


//--------------------------------------------------------------------
//[GETSINCOS] Get the sinus and cosinus from the angle +/- multiple circles
//AngleDeg1    - Input Angle in degrees
//sin4         - Output Sinus of AngleDeg
//cos4         - Output Cosinus of AngleDeg
void Phoenix::GetSinCos(short AngleDeg1)
{
    short        ABSAngleDeg1;    //Absolute value of the Angle in Degrees, decimals = 1
    //Get the absolute value of AngleDeg
    if (AngleDeg1 < 0)
        ABSAngleDeg1 = AngleDeg1 *-1;
    else
          ABSAngleDeg1 = AngleDeg1;
    
    //Shift rotation to a full circle of 360 deg -> AngleDeg // 360
    if (AngleDeg1 < 0)    //Negative values
        AngleDeg1 = 3600-(ABSAngleDeg1-(3600*(ABSAngleDeg1/3600)));
    else                //Positive values
        AngleDeg1 = ABSAngleDeg1-(3600*(ABSAngleDeg1/3600));
    
    if (AngleDeg1>=0 && AngleDeg1<=900)     // 0 to 90 deg
    {
        sin4 = (GetSin[AngleDeg1/5]);             // 5 is the presision (0.5) of the table
        cos4 = (GetSin[(900-(AngleDeg1))/5]);
    }     
        
    else if (AngleDeg1>900 && AngleDeg1<=1800)     // 90 to 180 deg
    {
        sin4 = pgm_read_word(&GetSin[(900-(AngleDeg1-900))/5]); // 5 is the presision (0.5) of the table    
        cos4 = -pgm_read_word(&GetSin[(AngleDeg1-900)/5]);            
    }    
    else if (AngleDeg1>1800 && AngleDeg1<=2700) // 180 to 270 deg
    {
        sin4 = -(GetSin[(AngleDeg1-1800)/5]);     // 5 is the presision (0.5) of the table
        cos4 = -(GetSin[(2700-AngleDeg1)/5]);
    }    

    else if(AngleDeg1>2700 && AngleDeg1<=3600) // 270 to 360 deg
    {
        sin4 = -(GetSin[(3600-AngleDeg1)/5]); // 5 is the presision (0.5) of the table    
        cos4 = (GetSin[(AngleDeg1-2700)/5]);            
    }
}    


//--------------------------------------------------------------------
//(GETARCCOS) Get the sinus and cosinus from the angle +/- multiple circles
//cos4        - Input Cosinus
//AngleRad4   - Output Angle in AngleRad4
long Phoenix::GetArcCos(short cos4)
{
    boolean NegativeValue/*:1*/;    //If the the value is Negative
    //Check for negative value
    if (cos4<0)
    {
        cos4 = -cos4;
        NegativeValue = 1;
    }
    else
        NegativeValue = 0;
    
    //Limit cos4 to his maximal value
    cos4 = min(cos4,c4DEC);
    
    if ((cos4>=0) && (cos4<9000))
    {
        AngleRad4 = (byte)(GetACos[cos4/79]);
        AngleRad4 = ((long)AngleRad4*616)/c1DEC;            //616=acos resolution (pi/2/255) ;
    }    
    else if ((cos4>=9000) && (cos4<9900))
    {
        AngleRad4 = (byte)(GetACos[(cos4-9000)/8+114]);
        AngleRad4 = (long)((long)AngleRad4*616)/c1DEC;             //616=acos resolution (pi/2/255) 
    }
    else if ((cos4>=9900) && (cos4<=10000))
    {
        AngleRad4 = (byte)(GetACos[(cos4-9900)/2+227]);
        AngleRad4 = (long)((long)AngleRad4*616)/c1DEC;             //616=acos resolution (pi/2/255) 
    }
       
    //Add negative sign
    if (NegativeValue)
        AngleRad4 = 31416 - AngleRad4;

    return AngleRad4;
}    

unsigned long Phoenix::isqrt32 (unsigned long n) //
{
        unsigned long root;
        unsigned long remainder;
        unsigned long  place;

        root = 0;
        remainder = n;
        place = 0x40000000; // OR place = 0x4000; OR place = 0x40; - respectively

        while (place > remainder)
        place = place >> 2;
        while (place)
        {
                if (remainder >= root + place)
                {
                        remainder = remainder - root - place;
                        root = root + (place << 1);
                }
                root = root >> 1;
                place = place >> 2;
        }
        return root;
}

//--------------------------------------------------------------------
//(GETATAN2) Simplyfied ArcTan2 function based on fixed point ArcCos
//ArcTanX         - Input X
//ArcTanY         - Input Y
//ArcTan4         - Output ARCTAN2(X/Y)
//XYhyp2          - Output presenting Hypotenuse of X and Y
short Phoenix::GetATan2 (short AtanX, short AtanY)
{
    XYhyp2 = isqrt32(((long)AtanX*AtanX*c4DEC) + ((long)AtanY*AtanY*c4DEC));
    GetArcCos (((long)AtanX*(long)c6DEC) /(long) XYhyp2);
    
    if (AtanY < 0)                // removed overhead... Atan4 = AngleRad4 * (AtanY/abs(AtanY));  
        Atan4 = -AngleRad4;
    else
        Atan4 = AngleRad4;
    return Atan4;
}    
    
//--------------------------------------------------------------------
//(BODY INVERSE KINEMATICS) 
//BodyRotX         - Global Input pitch of the body 
//BodyRotY         - Global Input rotation of the body 
//BodyRotZ         - Global Input roll of the body 
//RotationY        - Input Rotation for the gait 
//PosX             - Input position of the feet X 
//PosZ             - Input position of the feet Z 
//SinB             - Sin buffer for BodyRotX
//CosB             - Cos buffer for BodyRotX
//SinG             - Sin buffer for BodyRotZ
//CosG             - Cos buffer for BodyRotZ
//BodyFKPosX       - Output Position X of feet with Rotation 
//BodyFKPosY       - Output Position Y of feet with Rotation 
//BodyFKPosZ       - Output Position Z of feet with Rotation
void Phoenix::BodyFK (short PosX, short PosZ, short PosY, short RotationY, byte BodyIKLeg) 
{
    short            SinA4;          //Sin buffer for BodyRotX calculations
    short            CosA4;          //Cos buffer for BodyRotX calculations
    short            SinB4;          //Sin buffer for BodyRotX calculations
    short            CosB4;          //Cos buffer for BodyRotX calculations
    short            SinG4;          //Sin buffer for BodyRotZ calculations
    short            CosG4;          //Cos buffer for BodyRotZ calculations
    short            CPR_X;            //Final X value for centerpoint of rotation
    short            CPR_Y;            //Final Y value for centerpoint of rotation
    short            CPR_Z;            //Final Z value for centerpoint of rotation

    //Calculating totals from center of the body to the feet 
    CPR_X = (short)(cOffsetX[BodyIKLeg])+PosX + BodyRotOffsetX;
    CPR_Y = PosY + BodyRotOffsetY;         //Define centerpoint for rotation along the Y-axis
    CPR_Z = (short)(cOffsetZ[BodyIKLeg]) + PosZ + BodyRotOffsetZ;

    //Successive global rotation matrix: 
    //Math shorts for rotation: Alfa [A] = Xrotate, Beta [B] = Zrotate, Gamma [G] = Yrotate 
    //Sinus Alfa = SinA, cosinus Alfa = cosA. and so on... 
    
    //First calculate sinus and cosinus for each rotation: 
    GetSinCos (g_InControlState.BodyRot1.x+TotalXBal1);
    SinG4 = sin4;
    CosG4 = cos4;
    
    GetSinCos (g_InControlState.BodyRot1.z+TotalZBal1); 
    SinB4 = sin4;
    CosB4 = cos4;
    
    GetSinCos (g_InControlState.BodyRot1.y+(RotationY*c1DEC)+TotalYBal1) ;
    SinA4 = sin4;
    CosA4 = cos4;
    
    //Calcualtion of rotation matrix: 
      BodyFKPosX = ((long)CPR_X*c2DEC - ((long)CPR_X*c2DEC*CosA4/c4DEC*CosB4/c4DEC - (long)CPR_Z*c2DEC*CosB4/c4DEC*SinA4/c4DEC 
              + (long)CPR_Y*c2DEC*SinB4/c4DEC ))/c2DEC;
      BodyFKPosZ = ((long)CPR_Z*c2DEC - ( (long)CPR_X*c2DEC*CosG4/c4DEC*SinA4/c4DEC + (long)CPR_X*c2DEC*CosA4/c4DEC*SinB4/c4DEC*SinG4/c4DEC 
              + (long)CPR_Z*c2DEC*CosA4/c4DEC*CosG4/c4DEC - (long)CPR_Z*c2DEC*SinA4/c4DEC*SinB4/c4DEC*SinG4/c4DEC 
              - (long)CPR_Y*c2DEC*CosB4/c4DEC*SinG4/c4DEC ))/c2DEC;
      BodyFKPosY = ((long)CPR_Y  *c2DEC - ( (long)CPR_X*c2DEC*SinA4/c4DEC*SinG4/c4DEC - (long)CPR_X*c2DEC*CosA4/c4DEC*CosG4/c4DEC*SinB4/c4DEC 
              + (long)CPR_Z*c2DEC*CosA4/c4DEC*SinG4/c4DEC + (long)CPR_Z*c2DEC*CosG4/c4DEC*SinA4/c4DEC*SinB4/c4DEC 
              + (long)CPR_Y*c2DEC*CosB4/c4DEC*CosG4/c4DEC ))/c2DEC;
}  

//--------------------------------------------------------------------
//[LEG INVERSE KINEMATICS] Calculates the angles of the coxa, femur and tibia for the given position of the feet
//IKFeetPosX            - Input position of the Feet X
//IKFeetPosY            - Input position of the Feet Y
//IKFeetPosZ            - Input Position of the Feet Z
//IKSolution            - Output true if the solution is possible
//IKSolutionWarning     - Output true if the solution is NEARLY possible
//IKSolutionError       - Output true if the solution is NOT possible
//FemurAngle1           - Output Angle of Femur in degrees
//TibiaAngle1           - Output Angle of Tibia in degrees
//CoxaAngle1            - Output Angle of Coxa in degrees
//--------------------------------------------------------------------
void Phoenix::LegIK (short IKFeetPosX, short IKFeetPosY, short IKFeetPosZ, byte LegIKLegNr)
{
    unsigned long    IKSW2;            //Length between Shoulder and Wrist, decimals = 2
    unsigned long    IKA14;            //Angle of the line S>W with respect to the ground in radians, decimals = 4
    unsigned long    IKA24;            //Angle of the line S>W with respect to the femur in radians, decimals = 4
    short            IKFeetPosXZ;      //Diagonal direction from Input X and Z

    long            Temp1;            
    long            Temp2;            
    long            T3;
    
    //Calculate IKCoxaAngle and IKFeetPosXZ
    Atan4 = GetATan2 (IKFeetPosX, IKFeetPosZ); // AJ
    CoxaAngle1[LegIKLegNr] = (((long)Atan4*180) / 3141) + (short)(cCoxaAngle1[LegIKLegNr]);
    
    //Length between the Coxa and tars [foot]
    IKFeetPosXZ = XYhyp2/c2DEC;
    
    //Using GetAtan2 for solving IKA1 and IKSW
    //IKA14 - Angle between SW line and the ground in radians
    IKA14 = GetATan2 (IKFeetPosY, IKFeetPosXZ-(byte)(cCoxaLength[LegIKLegNr]));
    
    //IKSW2 - Length between femur axis and tars
    IKSW2 = XYhyp2;
    
    //IKA2 - Angle of the line S>W with respect to the femur in radians
    Temp1 = ((((long)(byte)(cFemurLength[LegIKLegNr])*(byte)(cFemurLength[LegIKLegNr])) - ((long)(byte)(cTibiaLength[LegIKLegNr])*(byte)(cTibiaLength[LegIKLegNr])))*c4DEC + ((long)IKSW2*IKSW2));
    Temp2 = (long)(2*(byte)(cFemurLength[LegIKLegNr]))*c2DEC * (unsigned long)IKSW2;
    T3 = Temp1 / (Temp2/c4DEC);
    IKA24 = GetArcCos (T3 );
    //IKFemurAngle
    FemurAngle1[LegIKLegNr] = -(long)(IKA14 + IKA24) * 180 / 3141 + 900 + CFEMURHORNOFFSET1(LegIKLegNr);

    //IKTibiaAngle
    Temp1 = ((((long)(byte)(cFemurLength[LegIKLegNr])*(byte)(cFemurLength[LegIKLegNr])) + ((long)(byte)(cTibiaLength[LegIKLegNr])*(byte)(cTibiaLength[LegIKLegNr])))*c4DEC - ((long)IKSW2*IKSW2));
    Temp2 = (2*(byte)(cFemurLength[LegIKLegNr])*(byte)(cTibiaLength[LegIKLegNr]));
    AngleRad4 = GetArcCos (Temp1 / Temp2); // AJ
    TibiaAngle1[LegIKLegNr] = -(900-(long)AngleRad4*180/3141);

    //Set the Solution quality    
    if(IKSW2 < ((byte)(cFemurLength[LegIKLegNr])+(byte)(cTibiaLength[LegIKLegNr])-30)*c2DEC)
        IKSolution = 1;
    else
    {
        if(IKSW2 < ((byte)(cFemurLength[LegIKLegNr])+(byte)(cTibiaLength[LegIKLegNr]))*c2DEC) 
            IKSolutionWarning = 1;
        else
            IKSolutionError = 1    ;
    }
}


//--------------------------------------------------------------------
//[CHECK ANGLES] Checks the mechanical limits of the servos
//--------------------------------------------------------------------
void Phoenix::CheckAngles(void)
{

    for (LegIndex = 0; LegIndex <6; LegIndex++)
    {
        CoxaAngle1[LegIndex]  = min(max(CoxaAngle1[LegIndex], (short)(cCoxaMin1[LegIndex])), 
                    (short)(cCoxaMax1[LegIndex]));
        FemurAngle1[LegIndex] = min(max(FemurAngle1[LegIndex], (short)(cFemurMin1[LegIndex])),
                    (short)(cFemurMax1[LegIndex]));
        TibiaAngle1[LegIndex] = min(max(TibiaAngle1[LegIndex], (short)(cTibiaMin1[LegIndex])),
                    (short)(cTibiaMax1[LegIndex]));
    }
}


//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void Phoenix::StartUpdateServos( void )
{        
    byte    i,LegIndex;

    //Serial.printf( "update servo's\n" );

    // Start the update 
    for (i = 0; i <= 2; i++)
    {   // Right
        LegIndex = i;
        g_ServoDriverR.setPWM(i*4+0, 0, CoxaAngle1[LegIndex] * cCoxaPWMD[LegIndex] /900 +  cCoxaPWMC[LegIndex] );         
        g_ServoDriverR.setPWM(i*4+1, 0, FemurAngle1[LegIndex]* cFemurPWMD[LegIndex]/900 + cFemurPWMC[LegIndex] );         
        g_ServoDriverR.setPWM(i*4+2, 0, TibiaAngle1[LegIndex]* cTibiaPWMD[LegIndex]/900 + cTibiaPWMC[LegIndex] );    

        // Left
        LegIndex = 3+i; // 15-xxxx  => Mirrored connected
        g_ServoDriverL.setPWM(15-(i*4+0), 0, CoxaAngle1[LegIndex] * cCoxaPWMD[LegIndex] /900 +  cCoxaPWMC[LegIndex] );     
        g_ServoDriverL.setPWM(15-(i*4+1), 0, FemurAngle1[LegIndex]* cFemurPWMD[LegIndex]/900 + cFemurPWMC[LegIndex] );     
        g_ServoDriverL.setPWM(15-(i*4+2), 0, TibiaAngle1[LegIndex]* cTibiaPWMD[LegIndex]/900 + cTibiaPWMC[LegIndex] );      
     
     
    } 
}

void Phoenix::loop(void)
{
      //Start time
    lTimerStart = millis(); 
    
    //Read input -> Moved to WebSocket Eventhandler
    
    //Single leg control
    SingleLegControl ();
            
    //Gait
    GaitSeq();
             
    //Balance calculations
    TotalTransX = 0;     //reset values used for calculation of balance
    TotalTransZ = 0;
    TotalTransY = 0;
    TotalXBal1 = 0;
    TotalYBal1 = 0;
    TotalZBal1 = 0;
    if (g_InControlState.BalanceMode) {
        for (LegIndex = 0; LegIndex <= 2; LegIndex++) {    // balance calculations for all Right legs

            BalCalcOneLeg (-LegPosX[LegIndex]+GaitPosX[LegIndex], 
                        LegPosZ[LegIndex]+GaitPosZ[LegIndex], 
                        (LegPosY[LegIndex]-(short)cInitPosY[LegIndex])+GaitPosY[LegIndex], LegIndex);
        }

        for (LegIndex = 3; LegIndex <= 5; LegIndex++) {    // balance calculations for all Right legs
            BalCalcOneLeg(LegPosX[LegIndex]+GaitPosX[LegIndex], 
                        LegPosZ[LegIndex]+GaitPosZ[LegIndex], 
                        (LegPosY[LegIndex]-(short)cInitPosY[LegIndex])+GaitPosY[LegIndex], LegIndex);
        }
        BalanceBody();
    }
  
 //Reset IKsolution indicators 
     IKSolution = 0 ;
     IKSolutionWarning = 0; 
     IKSolutionError = 0 ;
            
     //Do IK for all Right legs
     for (LegIndex = 0; LegIndex <=2; LegIndex++) {    
        BodyFK(-LegPosX[LegIndex]+g_InControlState.BodyPos.x+GaitPosX[LegIndex] - TotalTransX,
                LegPosZ[LegIndex]+g_InControlState.BodyPos.z+GaitPosZ[LegIndex] - TotalTransZ,
                LegPosY[LegIndex]+g_InControlState.BodyPos.y+GaitPosY[LegIndex] - TotalTransY,
                GaitRotY[LegIndex], LegIndex);
                               
        LegIK (LegPosX[LegIndex]-g_InControlState.BodyPos.x+BodyFKPosX-(GaitPosX[LegIndex] - TotalTransX), 
                LegPosY[LegIndex]+g_InControlState.BodyPos.y-BodyFKPosY+GaitPosY[LegIndex] - TotalTransY,
                LegPosZ[LegIndex]+g_InControlState.BodyPos.z-BodyFKPosZ+GaitPosZ[LegIndex] - TotalTransZ, LegIndex);
    }
          
    //Do IK for all Left legs  
    for (LegIndex = 3; LegIndex <=5; LegIndex++) {
        BodyFK(LegPosX[LegIndex]-g_InControlState.BodyPos.x+GaitPosX[LegIndex] - TotalTransX,
                LegPosZ[LegIndex]+g_InControlState.BodyPos.z+GaitPosZ[LegIndex] - TotalTransZ,
                LegPosY[LegIndex]+g_InControlState.BodyPos.y+GaitPosY[LegIndex] - TotalTransY,
                GaitRotY[LegIndex], LegIndex);
        LegIK (LegPosX[LegIndex]+g_InControlState.BodyPos.x-BodyFKPosX+GaitPosX[LegIndex] - TotalTransX,
                LegPosY[LegIndex]+g_InControlState.BodyPos.y-BodyFKPosY+GaitPosY[LegIndex] - TotalTransY,
                LegPosZ[LegIndex]+g_InControlState.BodyPos.z-BodyFKPosZ+GaitPosZ[LegIndex] - TotalTransZ, LegIndex);
    }
    
    //Check mechanical limits
    CheckAngles();
                

    //Drive Servos
    if (g_InControlState.fHexOn) {
        if (g_InControlState.fHexOn && !g_InControlState.fPrev_HexOn) {
           // Switch on Led
        }
        
        //Calculate Servo Move time
        if ((abs(g_InControlState.TravelLength.x)>cTravelDeadZone) || (abs(g_InControlState.TravelLength.z)>cTravelDeadZone) ||
                (abs(g_InControlState.TravelLength.y*2)>cTravelDeadZone)) {         
            ServoMoveTime = NomGaitSpeed + (g_InControlState.InputTimeDelay*2) + g_InControlState.SpeedControl;
                
            //Add aditional delay when Balance mode is on
            if (g_InControlState.BalanceMode)
                ServoMoveTime = ServoMoveTime + 100;
        } else //Movement speed excl. Walking
            ServoMoveTime = 200 + g_InControlState.SpeedControl;
        
        // note we broke up the servo driver into start/commit that way we can output all of the servo information
        // before we wait and only have the termination information to output after the wait.  That way we hopefully
        // be more accurate with our timings...
        StartUpdateServos();
        
        // See if we need to sync our processor with the servo driver while walking to ensure the prev is completed before sending the next one
                
        fContinueWalking = false;
            
        // Finding any incident of GaitPos/Rot <>0:
        for (LegIndex = 0; LegIndex <= 5; LegIndex++) {
            if ( (GaitPosX[LegIndex] > 2) || (GaitPosX[LegIndex] < -2)
                    || (GaitPosY[LegIndex] > 2) || (GaitPosY[LegIndex] < -2)
                    || (GaitPosZ[LegIndex] > 2) || (GaitPosZ[LegIndex] < -2)
                    || (GaitRotY[LegIndex] > 2) || (GaitRotY[LegIndex] < -2) )    {
                fContinueWalking = true;
                break;
            }
        }
        if (fWalking || fContinueWalking) {
            word  wDelayTime;
            fWalking = fContinueWalking;
                  
            //Get endtime and calculate wait time
            lTimerEnd = millis();
            if (lTimerEnd > lTimerStart)
                CycleTime = lTimerEnd-lTimerStart;
            else
                CycleTime = 0xffffffffL - lTimerEnd + lTimerStart + 1;
            
            // if it is less, use the last cycle time...
            //Wait for previous commands to be completed while walking
            wDelayTime = (min(max ((PrevServoMoveTime - CycleTime), 1), NomGaitSpeed));
           
            backgroundProcessing (wDelayTime); 
        }      
    } else {
        //Turn the bot off
        if (g_InControlState.fPrev_HexOn || (AllDown= 0)) {
            ServoMoveTime = 600;
            StartUpdateServos();
            backgroundProcessing(ServoMoveTime);
            backgroundProcessing(600);
        } else {
            
            g_ServoDriverR.reset();
            g_ServoDriverL.reset();          
        }
        backgroundProcessing(20);  // give a pause between times we call if nothing is happening
    }

    // Xan said Needed to be here...
    backgroundProcessing(ServoMoveTime);
    PrevServoMoveTime = ServoMoveTime;

    //Store previous g_InControlState.fHexOn State
    if (g_InControlState.fHexOn)
        g_InControlState.fPrev_HexOn = 1;
    else
        g_InControlState.fPrev_HexOn = 0;

}

