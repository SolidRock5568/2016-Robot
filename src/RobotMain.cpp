#include "WPILib.h"

#define A_BUTTON 1
#define B_BUTTON 2
#define X_BUTTON 3
#define Y_BUTTON 4
#define L_BUMPER 5
#define R_BUMPER 6
#define SELECT_BUTTON 7
#define START_BUTTON 8

class Robot: public IterativeRobot
{
	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;

	Joystick controller;
	Talon right_motor;
	Talon left_motor;
	Jaguar raise_motor;
	Talon shoot_motor_1;
	Talon shoot_motor_2;
	Victor kick_motor;
	bool isDebugging;


public:
	Robot ():
		controller(0),
		right_motor(0),
		left_motor(1),
		raise_motor(2),
		shoot_motor_1(3),
		shoot_motor_2(4),
		kick_motor(5),
		isDebugging(false)
		{
		}

	void RobotInit()override
	{
		 // create an image
		frame = imaqCreateImage(IMAQ_IMAGE_RGB, 0);
		//the camera name (ex "cam0") can be found through the roborio web interface
		imaqError = IMAQdxOpenCamera("cam0", IMAQdxCameraControlModeController, &session);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxOpenCamera error: " + std::to_string((long)imaqError) + "\n");
		}
		imaqError = IMAQdxConfigureGrab(session);
		if(imaqError != IMAQdxErrorSuccess) {
			DriverStation::ReportError("IMAQdxConfigureGrab error: " + std::to_string((long)imaqError) + "\n");
		}

	}

	void AutonomousInit()
	{

	}

	void AutonomousPeriodic()
	{
		raise_motor.Set(-0.75);
		Wait(1500);
		raise_motor.Set(0);
		Wait(500);
		right_motor.Set(0.5);
		left_motor.Set(0.5);
		Wait(3000);
		right_motor.Set(0);
		left_motor.Set(0);


	}

	void TeleopInit()
	{

	}

	
	void TeleopPeriodic()
	{
		// acquire images
		IMAQdxStartAcquisition(session);
		// grab an image, draw the circle, and provide it for the camera server which will
		// in turn send it to the dashboard.
		while(IsOperatorControl() && IsEnabled()) {
			IMAQdxGrab(session, frame, true, NULL);
			if(imaqError != IMAQdxErrorSuccess)
			{
				DriverStation::ReportError("IMAQdxGrab error: " + std::to_string((long)imaqError) + "\n");
			}
			else
			{
			}

			if(isDebugging == true)
			{
				float driveStraight = float(controller.GetRawButton(X_BUTTON));
				float driveBackwards = float(controller.GetRawButton(B_BUTTON));
				float driveLeft = float(controller.GetRawButton(Y_BUTTON));
				float driveRight = float(controller.GetRawButton(A_BUTTON));

				right_motor.Set(driveStraight - driveBackwards + driveLeft - driveRight);
				left_motor.Set(driveStraight - driveBackwards + driveLeft - driveRight);
			}
			else if(controller.GetRawButton(SELECT_BUTTON) == true)
			{
				isDebugging = !isDebugging;
			}

			if(controller.GetRawAxis(3) > 0 )
			{
				shoot_motor_1.Set(1);
				shoot_motor_2.Set(-1);
			}
			else if(controller.GetRawAxis(2) > 0 )
			{
				shoot_motor_1.Set(-0.4);
				shoot_motor_2.Set(0.4);
			}
			else
			{
				shoot_motor_1.Set(0);
				shoot_motor_2.Set(0);
			}

			if (controller.GetRawAxis(1) < -0.2)
			{
				float speed = controller.GetRawAxis(1) * controller.GetRawAxis(1);
				raise_motor.Set(-speed);
			}
			else if (controller.GetRawAxis(1) > 0.2)
			{
				float speed = controller.GetRawAxis(1) * controller.GetRawAxis(1);
				raise_motor.Set(speed);
			}
			else
			{
				raise_motor.Set(0);
			}

			if(controller.GetRawButton(B_BUTTON) == true)
			{
				kick_motor.Set(0.5);
			}
			else if(controller.GetRawButton(X_BUTTON) == true)
			{
				kick_motor.Set(-0.5);
			}
			else
			{
				kick_motor.Set(0);
			}



				float right_Stick_FB = controller.GetRawAxis(5);
				float right_Stick_LR = controller.GetRawAxis(4);
				right_Stick_FB = right_Stick_FB * right_Stick_FB * right_Stick_FB;
				right_Stick_LR = right_Stick_LR * right_Stick_LR * right_Stick_LR;

				right_motor.Set(right_Stick_FB + right_Stick_LR);
				left_motor.Set(right_Stick_LR - right_Stick_FB);


				Wait(0.005);				// wait for a motor update time
		}
		// stop image acquisition

		IMAQdxStopAcquisition(session);

	}

	void TestPeriodic()
	{

	}
};

START_ROBOT_CLASS(Robot)
