/*
 *  GamePadController.cpp
 *  glswarm
 *
 *  Created by Dan Swain on 3/16/08.
 *
 */

#include "GamePadController.h"

#pragma mark Constructors

MT_GamePadController::MT_GamePadController()
    : MT_HIDGamePad()
{
    common_init();
    myXYRobot = NULL;
    myWZRobot = NULL;
}
  
MT_GamePadController::MT_GamePadController(MT_SteeredRobot* setXYRobot)
{
    common_init();
    if(SetXYRobot(setXYRobot) == MT_ROBOT_ASSIGNMENT_ERROR)
        printf("Could not assign XY robot.\n");
    myWZRobot = NULL;
}

MT_GamePadController::MT_GamePadController(MT_SteeredRobot* setXYRobot, MT_SteeredRobot* setWZRobot)
{
    common_init();
  
    printf("Assigning Robot 1\n");
    if(SetXYRobot(setXYRobot) == MT_ROBOT_ASSIGNMENT_ERROR)
        printf("Could not assign XY robot.\n");
  
    printf("Assigning Robot 2\n");
    if(SetWZRobot(setWZRobot) == MT_ROBOT_ASSIGNMENT_ERROR)
        printf("Could not assign WZ robot.\n");
  
}


#pragma mark -
#pragma mark Initialization and Parameter Adjustment

void MT_GamePadController::common_init()
{
    // Do the gamepad's initializations
    connected = !(MT_HIDGamePad::Init());
  
    // Initially we have no robots
    AvailableRobots.resize(0);
  
    SetParameters(MT_DEFAULT_MAX_SPEED, MT_DEFAULT_MAX_TURNING_RATE, MT_DEFAULT_SPEED_DEADBAND, MT_DEFAULT_TURNING_DEADBAND);
  
}

void MT_GamePadController::SetParameters(float setMaxSpeed, float setMaxTurning, 
                                      unsigned int setSpeedDeadBand, unsigned int setTurningDeadBand)
{
    MaxForwardSpeed = setMaxSpeed;
    MaxTurningRate = setMaxTurning;
    SpeedDeadBand = setSpeedDeadBand;
    TurningDeadBand = setTurningDeadBand;
  
    ForwardSpeedNormalization = fabs(MT_MAX_STICK_VAL_FLOAT - SpeedDeadBand);
    TurningRateNormalization = fabs(MT_MAX_STICK_VAL_FLOAT - TurningDeadBand);
  
}

void MT_GamePadController::SetDeadBands(unsigned int setSpeedDeadBand, unsigned int setTurningDeadBand)
{
    SetParameters(MaxForwardSpeed, MaxTurningRate, setSpeedDeadBand, setTurningDeadBand);
}

void MT_GamePadController::SetMaxRates(float setMaxSpeed, float setMaxTurning)
{
    SetParameters(setMaxSpeed, setMaxTurning, SpeedDeadBand, TurningDeadBand);
}

void MT_GamePadController::SetMaxSpeedMPS(float setMaxSpeed)
{
    SetParameters(setMaxSpeed, MaxTurningRate, SpeedDeadBand, TurningDeadBand);
}

void MT_GamePadController::SetMaxTurningRateRADPS(float setMaxTurningRate)
{
    SetParameters(MaxForwardSpeed, setMaxTurningRate, SpeedDeadBand, TurningDeadBand);
}

float MT_GamePadController::GetMaxSpeedMPS() const
{
    return MaxForwardSpeed;
}

float MT_GamePadController::GetMaxTurningRateRADPS() const
{
    return MaxTurningRate;
}


#pragma mark -
#pragma mark Robot Posession

void MT_GamePadController::SeizeControl()
{
  
    // check to see if we have an XY robot
    if( myXYRobot )
    {
        // if so, make it non-autonomous
        myXYRobot->SetAutonomousOff();
    }
  
    // same for WZ robot
    if( myWZRobot )
    {
        myWZRobot->SetAutonomousOff();
    }
  
}


void MT_GamePadController::ReleaseControl()
{
  
    // check to see if we have an XY robot
    if( myXYRobot )
    {
        // if so, make it non-autonomous
        myXYRobot->SetAutonomousOn();
    }
  
    // same for WZ robot
    if( myWZRobot )
    {
        myWZRobot->SetAutonomousOn();
    }
  
}

#pragma mark -
#pragma mark Robot Assignment/Verification

unsigned int MT_GamePadController::AddRobot(MT_SteeredRobot* newRobot)
{
    int bot_index = -1;
  
    for(unsigned int i = 0; i < AvailableRobots.size(); i++)
    {
        if(newRobot == AvailableRobots[i] || AvailableRobots[i] == NULL)
        {
            bot_index = i;
        }
    }
  
    // still = -1 -> need to add robot
    if(bot_index < 0)
    {
        AvailableRobots.push_back(newRobot);
        bot_index = AvailableRobots.size() - 1;
    }
  
    return bot_index;
  
}

unsigned int MT_GamePadController::RemoveRobot(MT_SteeredRobot* BotToRemove)
{
    // if the robot is already assigned to an axis, don't give it up
    if( (BotToRemove == myXYRobot) || (BotToRemove == myWZRobot) )
        return MT_ROBOT_IN_USE;
  
    // if the robot is found in our list, free it (carefully) and let 
    //   the calling function know
    for(unsigned int i = 0; i < AvailableRobots.size(); i++){
        if(BotToRemove == AvailableRobots[i]){
            // Set the robot pointer to NULL so we don't clobber it with the erase!
            AvailableRobots[i] = NULL;
            AvailableRobots.erase(AvailableRobots.begin() + i);
            return MT_ROBOT_FREED;
        }
    }
  
    // The robot wasn't found in our list, but tell the calling function we freed it.
    return MT_ROBOT_FREED;
  
}

unsigned int MT_GamePadController::RemoveRobot(unsigned int i)
{

    // Check against Axes robots to make sure we don't try to move them
    if(AvailableRobots[i] == myXYRobot)
    {
        myXYRobot = NULL;
    }
    if(AvailableRobots[i] == myWZRobot)
    {
        myWZRobot = NULL;
    }
  
    // make sure the pointer is null
    AvailableRobots[i] = NULL;
  
    return MT_ROBOT_FREED;
  
}

unsigned int MT_GamePadController::robotID(MT_SteeredRobot* QueryBot)
{
    for(unsigned int i = 0; i < AvailableRobots.size(); i++){
        if(QueryBot == AvailableRobots[i])
            return i;
    }
    return AvailableRobots.size();
}

unsigned char MT_GamePadController::SetXYRobot(MT_SteeredRobot* setXYRobot)
{
  
    // make the robot non-autonomous
    setXYRobot->SetAutonomousOff();
    return AssignRobot(myXYRobot,setXYRobot);

}

unsigned char MT_GamePadController::SetWZRobot(MT_SteeredRobot* setWZRobot)
{
  
    // make the robot non-autonomous
    setWZRobot->SetAutonomousOff();
    return AssignRobot(myWZRobot,setWZRobot); 
  
}

unsigned char MT_GamePadController::AssignRobot(MT_SteeredRobot*& RobotToChange, MT_SteeredRobot* NewBot) 
{
    // This is the same robot, treat as an error
    if(NewBot == RobotToChange){
        //printf("C1\n");
        return MT_ROBOT_ASSIGNMENT_ERROR;
    }
  
    // This robot already has an axis, so just bail
    if(NewBot == myXYRobot || NewBot == myWZRobot){
        //printf("C2\n");
        return MT_ROBOT_ASSIGNMENT_ERROR;
    }
  
    // This robot is not connected, so tell the user and bail
    if( !(NewBot->IsConnected()) ){
        printf("Robot on %s is not connected.\n", NewBot->getComPort());
        return MT_ROBOT_ASSIGNMENT_ERROR;
    }  
  
    RobotToChange = NewBot;
    // Add it to the list if it's not already there
    AddRobot(NewBot);
    return MT_ROBOT_ASSIGNMENT_OK;
}

#pragma mark -
#pragma mark Input Handlers

void MT_GamePadController::PollAndUpdate(bool DoControl)
{
    Poll();
  
    // Be careful if both buttons are pressed.  Let 5 take precedence.
    if( Button5State() || (Button5State() && Button7State()) ){
        /*MT_SETBIT(ButtonStates,MT_BUTTON5,0);
          MT_SETBIT(ButtonStates,MT_BUTTON7,0);*/
        NextXYRobot(-1);
    }
  
    if(Button7State() && !Button5State() ){
        //MT_SETBIT(ButtonStates,MT_BUTTON7,0);
        NextXYRobot(1);
    }
  
    if( Button6State() || (Button6State() && Button8State()) ){
        /*MT_SETBIT(ButtonStates,MT_BUTTON6,0);
          MT_SETBIT(ButtonStates,MT_BUTTON8,0);*/
        NextWZRobot(-1);
    }
  
    if(Button8State() && !Button6State() ){
        //MT_SETBIT(ButtonStates,MT_BUTTON7,0);
        NextWZRobot(1);
    }
      
    if(DoControl && myXYRobot){
        myXYRobot->SetSpeedOmega( CalculateSpeed(Y), CalculateTurningRate(X) );
    }
  
    if(DoControl && myWZRobot){
        myWZRobot->SetSpeedOmega( CalculateSpeed(Z), CalculateTurningRate(W) );
    }
  
}

void MT_GamePadController::NextXYRobot(char direction)
{
    //printf("Trying to cycle XY robot in the %s direction\n", direction > 0 ? "+" : "-" );
    cycle_robot(myXYRobot,direction); 
}

void MT_GamePadController::NextWZRobot(char direction)
{
    //printf("Trying to cycle WZ robot in the %s direction\n", direction > 0 ? "+" : "-" );
    cycle_robot(myWZRobot,direction);
}

void MT_GamePadController::cycle_robot(MT_SteeredRobot*& botToChange, char direction)
{
  
    MT_SteeredRobot* original_bot = botToChange;
    unsigned int myID = robotID(botToChange);
    direction = MT_SGN(direction);
  
    for(unsigned int i = myID; i < AvailableRobots.size() && i >= 0; i = i + direction){
        if( (i != myID) && (AssignRobot(botToChange,AvailableRobots[i]) == MT_ROBOT_ASSIGNMENT_OK) ){
      
            DisplayAssignedRobots();
      
            original_bot->SetSpeedOmega(0,0);
      
            return;
        }
    }
  
}

void MT_GamePadController::DisplayAssignedRobots() const
{
    char Robots[8];
    char Assigned[8];
    char c[2];
  
    for(unsigned int i = 0; i < AvailableRobots.size(); i++){
        sprintf(c,"%1d",i+1);
        Robots[i] = c[0];
        if(AvailableRobots[i] == myXYRobot)
            Assigned[i] = 'X';
        else if(AvailableRobots[i] == myWZRobot)
            Assigned[i] = 'W';
        else
            Assigned[i] = ' ';
    }
  
    for(int i = AvailableRobots.size(); i < 8; i++){
        Robots[i] = ' ';
        Assigned[i] = ' ';
    }
  
    Robots[7] = '\0';
    Assigned[7] = '\0';
  
    printf("Robots Available to Joystick: [%s]\n",Robots);
    printf("Assigned to axis:             [%s]\n",Assigned);
  
}
