# AccelStepper-Scanner-Motor
Accelstepper implementation to move a stepper motor with input commands.

# Basic setup
1. Connect arduino to computer and upload file to board.
2. Make sure you are operating at 12v and no more than 2A
3. Logic board (L298N) red led should be on

# Commands
1. Open serial port
2. Type in any of the following commands
	1. `Position %` where %  can be any number from 1 to 10. This is for moving the motor to any position
	2. `Calibrate` will give you instructions in the serial port for calibrating the motor. You can change the step size within the program for finer or coarser adjustments. 
	3. `Show log` returns a vector containing the visited positions *
	4. Any command in `PosCommands.Waypoints`; this was for testing purposes, and will likely be unhelpful; it was kept to emulate original code

# Notes:
- Pressing reset button on Arduino will reset global variables. This will make the board think it's at position 3-the default home-again. This is useful when used in tandem with the `Calibrate` command.
- If too much current is going being fed to the L298N you run the risk of overheating the heatsink and/or burning the L298N. Adjusting current is useful for managing the motor's torque.
- In `void Motor::MoveMotor()` a negative sign was added to the step count, in order to move the motor according to the setup. This was a quick fix, but I suggest keeping an eye on the motor positioning during daq. If this fails, change the way the `PosCommands.InputPosition` is constructed; Also, make sure the ternary operation in `void loop()` is working appropriately, you want the motor only moving in one direction. Another fix could be swapping the pins to the stepper motor, to reverse the polarity, without changing the code.
- When moving the wires around the stepper motor, make sure you probe the pins, to make sure you're not shorting the internal coils.

# Useful links
- [AccelStepper documentation](http://www.airspayce.com/mikem/arduino/AccelStepper/)
- [Good guide for AccelStepper](https://hackaday.io/project/183279-accelstepper-the-missing-manual/details)
- [L298N setup](https://www.makerguides.com/l298n-stepper-motor-arduino-tutorial/)
