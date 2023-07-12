//This is the one you need to test
#include <AccelStepper.h>
#include <ArduinoSTL.h>
//Set up pins
const int AIn1 = 8;
const int AIn2 = 9;
const int BIn1 = 10;
const int BIn2 = 11;

//Set up global variables
const int stepsPerRevolution = 2800; //
const int stepsPos = 210;  //steps between each position, approximately 27 degrees // double - check with setup

String command;
//String logStr;
String Cstr;
int stepsTotal = 0;
int currentPos = 3;  // this is the default home position.
int newPos = 0;
int steps = 0;
int stepCount;

AccelStepper Stepper1(AccelStepper::FULL4WIRE, AIn1, AIn2, BIn1, BIn2);

//Motor Class to operate all parts
class Motor {
public:
  Motor() = default;
  void SetupMotor(AccelStepper* MotorToControl);  //
  void MoveMotor(String, int);
  void Calibrate(AccelStepper* MotorToControl); //
private:
  AccelStepper* _Stepper1;
};
// Set up a log for changes in position
struct TripLog {
  String CurrentLoc;  //-
  std::vector<String> Logger;
  int run = 0;  //keeping track how many times it has moved.
};
//Custom commands for the positions
struct Positioner {
  String InputPosition[10];  //
  int StepAdv[10];           //
  const String Waypoints[10];      //-
  int WaypointAdv[10];       //-
};

Positioner SetupCommands() {
  //custom commands: might want to flip these
  Positioner PosCommands;
  int i;
  for (i = 1; i <= 10; i++) {
    PosCommands.InputPosition[i - 1] = "Position " + String(i);
    PosCommands.StepAdv[i - 1] = i;
  }
  //test again - probably uneccessary, but written to emulate original code
  const String points[10] = { "home", "posTest", "posTestNeg", "current position",
                        "nextPos", "prevPos", "reposition cw", "reposition ccw", "empty 1", "empty 2" };  // home needs to be its own thing. 360*Nsteps-Pos[i]*27 = Nsteps to go; check calibrate
  const int pointsC[10] = { 3, 360, -360, 0, 211, -211, 40, -40, 0, 0 }; //degrees // fix home
  for (i = 0; i <= 9; i++) {
    PosCommands.Waypoints[i] = points[i];
    PosCommands.WaypointAdv[i] = pointsC[i];
  }
  return PosCommands;
}

void Motor::SetupMotor(AccelStepper* MotorToControl) {
  _Stepper1 = MotorToControl;
  _Stepper1->setMaxSpeed(50);
  _Stepper1->setAcceleration(5);
}  

void Motor::MoveMotor(String motor, int stepCount) {
  _Stepper1->runToNewPosition(-1*stepCount);
  _Stepper1->setCurrentPosition(0);
}

void Motor::Calibrate(AccelStepper* MotorToControl) {
  // Calibration function:
  // Enter Calibrate into serial port and follow prompts
  // Change M - step size as needed for finer or coarse calibration movements
  int M = 280;
  _Stepper1->setCurrentPosition(M);
  Serial.print("Calibrate?\n");
  Serial.print("Y/N\n");
  bool endL = false;
  while(endL == false){
    if (Serial.available()) {
      Cstr = Serial.readStringUntil('\n');
      if (Cstr.equals("Y")) {
        bool Cflag = false;
        Serial.print("\nhit enter to advance; type STOP when done \n");
        while (Cflag == false){
          //int M = 10;
          if (Serial.available()){
            Cstr = Serial.readStringUntil('\n');
            if (Cstr.equals("STOP")){
              Cflag = true;
              break;
            } else {
              //M += 21;
            }
            MoveMotor("Motor A", M);
          }
        }
        endL = true;
      } else if (Cstr.equals("N")) {
        Serial.print("Calibration cancelled \n");
        endL = true;
      } else {
        Serial.print("invalid command ");
        endL = false;
      }
      
    }
  }
  Serial.print("Calibration complete\n");
  //currentPos = 3; you may want to uncomment this if you're resetting to position 3; the default position.
  //If not, just hit the reset button on the arduino board, and it will automatically reset all global variables
}

// Instantiate necessary objects: motor, custom commands, input log
Motor motorTest;
Positioner PosC = SetupCommands();
TripLog Log;

void setup() {
  //Arduino setup
  motorTest.SetupMotor(&Stepper1);
  Serial.begin(9600);
  delay(2000); 
}
//Stepper1 and motorTest are not the same
void loop() {
  //Possible commands for serial port:
  // - Position %; % u [1,10]
  // - Calibrate
  // - Show log
  // - Any waypoint string; probably only useful for testing setup
  if (Serial.available()) {
    Serial.print("Input command: ");
    command = Serial.readStringUntil('\n');
    bool validCommand = false;
    int i;
    for (i = 0; i <= 9; i++) {
      if (PosC.InputPosition[i].equals(command)) {
        newPos = i+1;
        stepCount = (newPos >= currentPos)?(newPos - currentPos) * stepsPos : stepsPerRevolution - (abs(newPos-currentPos)* stepsPos);
        motorTest.MoveMotor("Motor A", stepCount);
        currentPos = newPos;
        stepsTotal += stepCount;
        Log.CurrentLoc = PosC.InputPosition[i];
        Log.run += 1;
        Log.Logger.push_back(PosC.InputPosition[i]);
        Serial.print(PosC.InputPosition[i] + "\n");
        PosC.InputPosition;
        validCommand = true;
        break;
      } else if (PosC.Waypoints[i].equals(command)) {
        newPos = static_cast<int>(PosC.WaypointAdv[i] /1.8);
        motorTest.MoveMotor("Motor A", newPos);
        currentPos = newPos; //keep an eye on this
        stepsTotal += newPos;
        Log.CurrentLoc = PosC.Waypoints[i];
        Log.run += 1;
        Log.Logger.push_back(PosC.Waypoints[i]);
        Serial.print(Log.CurrentLoc +"\n");
        validCommand = true;
        break;
      } else if (command.equals("Show log")) {
        Serial.print("Show log\n");
        String logStr;
        for (int j = 0; j < Log.Logger.size(); j++) {
          logStr += Log.Logger[j] + " ";
        }
        Serial.print(logStr + "\n") ;
        validCommand = true;
        Serial.print("log");
        break;
      } else if (command.equals("Calibrate")) {
        Serial.print("\n");
        motorTest.Calibrate(&Stepper1);
        validCommand = true;
        currentPos = 3;
        break;
      }
    }
    if (!validCommand) {
      Serial.print("Invalid command\n");
    }
  }
}