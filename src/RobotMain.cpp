#include "WPILib.h"

#define A_BUTTON 1
#define B_BUTTON 2
#define X_BUTTON 3
#define Y_BUTTON 4
#define L_BUMPER 5
#define R_BUMPER 6
#define SELECT_BUTTON 7
#define START_BUTTON 8

bool IS_DEBUGGING;

class Robot: public IterativeRobot
{
	Joystick controller;
	Talon right_motor;
	Talon left_motor;
	Talon raise_motor;
	Victor shoot_motor_1;
	Victor shoot_motor_2;


public:
	Robot ():
		controller(0),
		right_motor(0),
		left_motor(1),
		raise_motor(2),
		shoot_motor_1(3),
		shoot_motor_2(4){}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{

	}

	void TeleopInit()
	{

	}
	
	void TeleopPeriodic()
	{
		
		if(IS_DEBUGGING == true) {
		
			float driveStraight = (controller.GetRawButton(X_BUTTON)) / 2;
			float driveBackwards = (controller.GetRawButton(B_BUTTON)) / 2;
			float driveLeft = (controller.GetRawButton(Y_BUTTON)) / 2;
			float driveRight = (controller.GetRawButton(A_BUTTON)) / 2;

			right_motor.Set(driveStraight - driveBackwards + driveLeft - driveRight);
			left_motor.Set(driveStraight - driveBackwards + driveLeft - driveRight);
		}
		if(controller.GetRawButton(SELECT_BUTTON)) {
			IS_DEBUGGING = !IS_DEBUGGING;
		}
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
