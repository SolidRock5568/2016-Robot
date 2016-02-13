#include "WPILib.h"
#include "Timer.h"

#define A_BUTTON 1
#define B_BUTTON 2
#define X_BUTTON 3
#define Y_BUTTON 4
#define L_BUMPER 5
#define R_BUMPER 6
#define SELECT_BUTTON 7
#define START_BUTTON 8

float squareDrive(float);

class Robot: public IterativeRobot
{
	IMAQdxSession session;
	Image *frame;
	IMAQdxError imaqError;

	BCGOptions_struct options;

	Joystick controller;
	Talon right_motor;
	Talon left_motor;
	Jaguar raise_motor;
	Talon shoot_motor_1;
	Talon shoot_motor_2;
	Victor kick_motor;
	Encoder encoder_right;
	Encoder encoder_left;
	bool isDebugging;
	bool armDown;


public:
	Robot ():
		controller(0),
		right_motor(0),
		left_motor(1),
		raise_motor(2),
		shoot_motor_1(3),
		shoot_motor_2(4),
		kick_motor(5),
		encoder_right(0, 1, false, Encoder::k4X),
		encoder_left(2, 3, false, Encoder::k4X),
		isDebugging(false)
		{
			encoder_right.SetSamplesToAverage(5);
			encoder_left.SetSamplesToAverage(5);
			// Defines how far the mechanism attached to the encoder moves per pulse.
			// In this case, we assume that a 360 count encoder is directly attached
			//   to a 3 inch diameter (1.5inch radius) wheel, and that we want to
			//   measure distance in inches.
			encoder_right.SetDistancePerPulse((1.0 / 497.0 * 3.1415 * 4.0) / 100.0);
			encoder_left.SetDistancePerPulse((1.0 / 497.0 * 3.1415 * 4.0) / 100.0);

			encoder_right.SetMinRate(1.0);
			encoder_left.SetMinRate(1.0);

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
		encoder_right.Reset();
		armDown = false;
	}

	void AutonomousPeriodic()
	{
		if(armDown == false)
		{
			raise_motor.Set(-0.2);
			Wait(2);
			raise_motor.Set(0);
			armDown = true;
		}
		if(encoder_right.GetDistance() < 2.5)
		{
			right_motor.Set(-0.3);
			left_motor.Set (0.3);
		}

		else
		{
			right_motor.Set(0);
			left_motor.Set(0);

		}

	}

	void TeleopInit()
	{
		encoder_right.Reset();
		encoder_left.Reset();
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
				//imaqDrawShapeOnImage(frame, frame, { 10, 10, 100, 100 }, DrawMode::IMAQ_DRAW_VALUE, ShapeMode::IMAQ_SHAPE_OVAL, 0.0f);

				//imaqColorEqualize(frame,frame,1);

				//options.contrast = 50; //75; //50
				//options.brightness = 0.5;//.50; // 10
				//options.gamma = 1; //5; // 1
				//imaqBCGTransform(frame,frame,&options,NULL);
				//imaqColorBCGTransform(frame,frame,&options,&options,&options,NULL);

				CameraServer::GetInstance()->SetImage(frame);
				CameraServer::GetInstance()->SetQuality(50);
				std::shared_ptr<USBCamera> camera(new USBCamera("cam0", true));
				camera->SetExposureManual(25); // 100 // change this value //this was 1
				//camera->SetBrightness(100); // change this value // this was 100
				//camera->SetWhiteBalanceManual(2);//this was 100
				//CameraServer::GetInstance()->StartAutomaticCapture(camera);
				camera->SetExposureHoldCurrent();
				//camera->SetWhiteBalanceHoldCurrent();
				//camera->UpdateSettings();

			}

			// Retrieve the net displacement of the Left Encoder since the last Reset.
			SmartDashboard::PutNumber("R Encoder Distance in ft: ", encoder_right.GetDistance());
			SmartDashboard::PutNumber("R Encoder in inches: ", (encoder_right.GetDistance()) * 12.0);
			// Retrieve the current rate of the encoder.
			SmartDashboard::PutNumber("R Encoder Rate", encoder_right.GetRate());
			// Retrieve the net displacement of the Encoder since the lsat Reset.
			SmartDashboard::PutNumber("L Encoder Distance in ft: ", encoder_left.GetDistance());
			SmartDashboard::PutNumber("L Encoder Distance in inches: ", (encoder_left.GetDistance()) * 12.0);
			// Retrieve the current rate of the encoder.
			SmartDashboard::PutNumber("LEncoder Rate", encoder_left.GetRate());


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
				raise_motor.Set(speed);
			}
			else if (controller.GetRawAxis(1) > 0.2)
			{
				float speed = controller.GetRawAxis(1) * controller.GetRawAxis(1);
				raise_motor.Set(-speed);
			}
			else
			{
				raise_motor.Set(0);
			}

			if(controller.GetRawButton(B_BUTTON) == true)
			{
				kick_motor.Set(1);
				Wait(0.1);
				kick_motor.Set(-0.5);
				Wait(0.15);
				kick_motor.Set(0);
			}
				float yIn = controller.GetRawAxis(5);
				float xIn = controller.GetRawAxis(4);

				right_motor.Set((squareDrive(yIn) + squareDrive(xIn)) / 2);
				left_motor.Set((squareDrive(xIn) - squareDrive(yIn)) / 2);

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

float squareDrive(float val) {
	if(val > 0.05) {
		val = val*val;
		return val;
	}
	else if(val < -0.05) {
		val = -(val*val);
		return val;
	}
	else {
		return 0;
	}
}
/*void DriveForward(float velocity, float time)
	{

		right_motor.Set(velocity * -1);
		left_motor.Set(velocity);

		Wait(time);

		rightmotor.Set(0);
		leftmotor.Set(0);

	}
	*/
