#include "WPILib.h"

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
		shoot_motor_2(4)
	{
	}

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
		float driveStraight = (controller.GetRawButton(1)) / 2;
		float driveBackwards = -(controller.GetRawButton(2)) / 2;

		right_motor.Set(driveStraight + driveBackwards);
		left_motor.Set(driveStraight + driveBackwards);
	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
