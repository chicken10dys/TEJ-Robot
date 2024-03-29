#pragma config(Sensor, dgtl5,  LimSwitch,      sensorDigitalIn)
#pragma config(Sensor, dgtl7,  LimSwitchArm,   sensorDigitalIn)
#pragma config(Sensor, dgtl11, LimSwitchArmBot, sensorDigitalIn)
#pragma config(Sensor, dgtl12, RelayN,         sensorDigitalOut)
#pragma config(Motor,  port1,           FR,            tmotorVex393_HBridge, openLoop, reversed, driveRight)
#pragma config(Motor,  port2,           BR,            tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           Arm,           tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           BL,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          FL,            tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Calculates which value from a pair of values is the minimum or maximum of the two
#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

//Stores the dead zone used for the controller
const short DEAD_ZONE = 5;

//Represents the max value for a motor
const short MAX_DIR = 127;
const short MIN_DIR = -127;

//Represents the high and low states of digital I/O devices
const int HIGH = 1;
const int LOW = 0;

//Stores the amount to reduce the arm joystic value by
const short ARM_SPEED_DIVISOR = 64;

//Stores the state of the program
bool running = true;

//Stores the movement value for the left and right motors
short dirx = 0;
short diry = 0;
short Ls = 0;
short Rs = 0;

//State of the relay
bool relayState = false;

//Stores the state and previous state of the button used for activating the relay
bool relayBtnState = false;
bool prevRelayBtnState = false;

//Represents if a new button input is required to activate the
bool needNewRelayInput = false;

//Stores the right joystick that controls the arm
short armValue;

//Moves the robot using the left joystick
void MoveRobot()
{
	//Gets the direction of the vertical and horizontal axis of the left joystick
	dirx = vexRT(Ch4);
	diry = vexRT(Ch3);

	//Checks where the joystic is and changes the speed of the motors accordingly
	if(-DEAD_ZONE <= dirx && dirx <= DEAD_ZONE && -DEAD_ZONE <= diry && diry <= DEAD_ZONE)
	{
		//Sets the values of the motors to 0 if the stick is within the range of the dead zone
		Rs = 0;
		Ls = 0;
	}
	else if (dirx == 0 && diry == MAX_DIR)
	{
		//Moves both sides of the robot forward at max speed
		Rs = MAX_DIR;
		Ls = MAX_DIR;
	}
	else if (dirx == MIN_DIR && diry == 0)// add deadzone
	{
		//Moves the right side forward at max speed and the left side backwards at max speed
		/*Rs = MAX_DIR;
		Ls = MIN_DIR;*/
		Rs = MAX_DIR;
		Ls = MIN_DIR;
	}
	else if (dirx == 0 && diry == MIN_DIR)
	{
		//Moves both sides of the robot bakcwards at max speed
		Rs = MIN_DIR;
		Ls = MIN_DIR;
	}
	else if (dirx == MAX_DIR && diry == 0)
	{
		//Moves the right side backwards at max speed and the left side forwards at max speed
		Rs = MAX_DIR;
		Ls = MIN_DIR;
	}
	else
	{
		//Calculates the speed of the motors based on the joystick being in quadrent 1
		if (diry > 0 && dirx > 0){
			Rs = diry - dirx;
			Ls = MAX(dirx, diry);
		}
		//Calculates the speed of the motors based on the joystick being in quadrent 2
		if (diry > 0 && dirx < 0){
			Rs = MAX(dirx, diry);
			Ls = diry + dirx;
		}
		//Calculates the speed of the motors based on the joystick being in quadrent 3
		if (diry < 0 && dirx < 0){
			Ls = diry - dirx;
			Rs = MIN(dirx, diry);
		}
		//Calculates the speed of the motors based on the joystick being in quadrent 4
		if (diry < 0 && dirx > 0){
			Rs = dirx + diry;
			Ls = MIN(dirx, diry);
		}
	}

	//Sets the speed of all four motors
	motor[FL] = Ls;
	motor[BL] = Ls;
	motor[FR] = Rs;
	motor[BR] = Rs;
}

//The entry point of the program
task main()
{
	//Loops until the program is not running any more
	while (running)
	{
		//Moves the robot using the joystick
		MoveRobot();

		//Updates the arm movement value
		armValue = vexRT(Ch2) / ARM_SPEED_DIVISOR;

		//Applies movment to the arm motor if it is not going past its maximum distance an either direction
		if(SensorValue[LimSwitchArm] == HIGH && SensorValue[LimSwitchArmBot] == HIGH)
		{
			motor[Arm] = vexRT(Ch2);
		}
		else if(armValue < 0 && SensorValue[LimSwitchArm] == LOW)
		{
			motor[Arm] = vexRT(Ch2);
		}
		else if(armValue > 0 && SensorValue[LimSwitchArmBot] == LOW)
		{
				motor[Arm] = vexRT(Ch2);
		}
		else
		{
			motor[Arm] = 0;
		}

		//Updates the current and previous state of the relay button
		prevRelayBtnState = relayBtnState;
		relayBtnState = vexRT(Btn8L);

		//Updates the relay state based on if it is already active
		if(relayState == false)
		{
			/*
			Turns on the relay if the button is held and it does not need a new input,
			or if a new button press was made and a new button press is required
			*/
			if((relayBtnState == true && needNewRelayInput == false) ||
				(needNewRelayInput == true && (relayBtnState == true && prevRelayBtnState == false)))
			{
				relayState = true;
			}
		}
		else
		{
			if(SensorValue[LimSwitch] == LOW)
			{
				relayState = false;
				needNewRelayInput = true;
			}
			else if(relayBtnState == false)
			{
				relayState = false;
			}
		}

		//Sets the value of the relay based on the state
		if(relayState == true)
		{
			SensorValue[RelayN] = LOW;
		}
		else
		{
			SensorValue[RelayN] = HIGH;
		}

		//Exits the program if both of the U sholder buttons are pressed
		if (vexRT(Btn5U) == true && vexRT(Btn6U) == true)
		{
			running = false;
		}
	}
}
