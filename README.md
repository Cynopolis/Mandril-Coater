Code for mandril coater control system.


# Installation
You will need to install [platformio](https://platformio.org/) to build and flash this code to your controller. Once that is installed, you can go to the Mandril-Coater github page and click Code>Download Zip. Unzip the file and open the folder in PlatformIO. Connect the controller via USB and ensure it is powered with a 12v supply. You can then build and flash the code to your controller by clicking the check mark in the bottom left corner of the VSCode window.

# Feedback
Feedback is highly encouraged! If you see any bugs, or if there are additional features you would like to see, please open an issue on the github page.

# Machine Parameters and Pinout

You can change some of the machine parameters by editing the `include/MACHINE-PARAMETERS.h` file. 

You can also edit the pinout by editing the `include/PINOUT.h` file.

# GCode Commands Supported
Here is a list of the GCode commands supported by this firmware and how to use them:

Gcode commands supported:

	- Architecture commands
	This command can be used as a handshake between the controller and a device sending it commands. The Machine simply responds back with !M2; and does nothing else.
		○ Ping
			§ !M2;
		○ Cancel current command
		Whatever command that is currently happening will stop and if there is another command available, it will move onto that command. This is only really helpful when manually controlling the machine.
			§ !M112;
		○ Wait
		The machine does nothing and executes no new GCode commands for a number of milliseconds.
			§ !G4,Tnnn;
				□ Tnnn - the time to wait in ms
		○ Estop
		Stops the motors for moving immediately. This command does not care about the GCode queue. If the machine recieves this command it will immediately stop regardless of what commands are ahead of it.
			§ !M0;
		○ Release Estop
		This command releases the EStop state.
			§ !M1;
		○ Pause/Resume
		The machine will pause further execution of commands until the resume button is pressed or until it recieves a resume command.
			§ !M24,Sn;
				□ S0 - Pause
				□ S1 resume
		○ Get motor positions
		The command will have the controller return a list of motor positions and speeds.
			§ M114
				□ Returns !M114,Xnnn,Rnnn,Fnnn,Snnn;
					® Xnnn - the current x position
					® Rnnn - the current R rotation in degrees
					® Fnnn - the current feed rate in mm/sec
					® Snnn - the current rotational feed rate in deg/sec
		○ Relative jump
		This command will cause the machine to go backwards by a certain number of commands. For example if you sent the commands:
		1. !G91;
		2. !G90;
		3. !M2;
		4. !M999,P3,T4;
		The machine will execute G91, G90, then M2, and the M999 command will cause it to go back to line (1). The T parameter says how many times we should repeat that command. In this case that would be 4 times.
		THe machine will refuse to jump backwards to a command that does not exist. If we sent:
		1. !G91;
		2. !G90;
		3. !M2;
		4. !M999,P4,T4;
		The M999 command would return "Invalid jump command. Index out of range." because there isn't a 4th command to jump back to.
			§ !M999,Pxxx,Txxx;
				□ Pxxx - The number of commands backwards to jump (The M999 command itself is not counted.)
				□ Txxx - The number of times to repeat the jump until you stop
					
	- Configure commands
		○ Relative move
		All moves are relative to the current position. A command like !G1,X100,F1000; WOuld move the machine forward 100mm, and not to the position 100mm. The R axis always does relative moves, so this command does not affect it.
			§ !G91;
		○ Absolute mode
		All moves are now in absolute positions. A command like !G1,X100,F1000; WOuld move the mahcine to the Position X100. The R axis is always relative so this command does not affect it.
			§ !G90;
		○ Set max travel
			§ !M208,Xnnn;
			THIS COMMAND IS DEPRECATED AND NO LONGER WORKS. (There isn't much of a reason to set this limit right now.)
			This command sets a limit on how far the X axis can move forward. !M208,X1000; Would mean that the X-axis couldn't move forward more than 1000mm. This is true even in relative mode, so trying to move the X-axis past X1000 still wouldn't work.
				□ Maximum travel in the X axis in mm
				□ No rotation max travel can be set
		○ Set step/unit
			THIS COMMAND IS DEPRECATED (If you want to set the steps/unit, you have to do it in MACHINE-PARAMETERS.h)
			§ !M92,Xnnn,Rnnn;
				□ Steps per mm for the x axis
				□ Steps per degree for the R axis
		○ Set axis acceleration
			Set the units/ min^2 for an axis.
			§ !M204,Xnnn,Rnnn;
				□ mm / min^2 for x axis
				□ deg / min^2 for r axis
			§ Calling M204 without any arguments will reset the acceleration back to the defaults
				
	- Actuate commands
		○ Linear move
		This move command is blocking and will stop any other commands from running until it completes. The linear move command typically takes in three arguments like: !G1,X1000,R2000,F1200; Sending this command would cause the X-axis to move to 1000mm and the R-axis to rotate 2000 degrees. The X-axis would move at 1200 mm/min and a feedrate would be automatically selected for the R-axis so that the to axes would finish their move at the same time. If the optional P parameter is specified, then the R-axis will mvoe at that rate instead and the two axes are no longer guaranteed to finish their move at the same time.
			§ !G1,Xnnn,Rnnn,Fnnn,Pnnn;
				□ Xnnn - the position to move linearly in mm
				□ Rnnn - the number of degrees to rotate
				□ Fnnn - the amount to move the x-axis in mm/min. The rotation axis will sync so it completes its move when the linear
				□ Pnnn - the amount to move the r-axis in deg/min. Setting this will override the rotational sync
		○ Linear move, non-blocking. 
		This is the same as G1, but it allows additional move commands to be executed immediately without the current G0 finishing. If another move command (either G0 or G1) is given before the current G0 command has finished its move, those commands will override the current move. When an override occurs, the position of the machine is assumed to be the position it was in before the previous G0 command was sent.
		An example:
		1. !G0,X100,R100,F1000; (We are starting at X0,R0)
		2. !G1,X150,R100,F1000; (This command will assume it's also starting at X0,R0 because the G0 command didn't have time to complete.)

			§ !G0,Xnnn,Rnnn,Fnnn,Pnnn; (I'm aware this isn't technically correct)
				□ Xnnn - the position to move linearly in mm
				□ Rnnn - the number of degrees to rotate
				□ Fnnn - the amount to move the x-axis in mm/min.
				□ Pnnn - the amount to move the r-axis in deg/min
				
		○ Home
		Causes the homing sequence to be run. That sequence is:
		1. Move the x-axis towards the home switch at full speed
		2. When the home switch triggers, immediately reverse the x-axis at a low speed and back off by 50mm. (During this time, the machine will not be listening to the home switch.)
		3. Move back towards the homing axis until the home switch is triggered. (this move is up to 60mm)
			§ !G28;
		○ Switch I/O pin
			§ !M42,Pnnn,Sn;
				□ Pnnn - pin number
				□ S0 - Pin on
				□ S1 - pin off


Other Non-GCode Messages Supported:
- !0;
	- This is sent when the GCode queue is empty on the controller
- !1;
	- This is sent when the GCode queue is full on the controller
- !2;
	- Acknowledge. The command has been recieved and put in the queue
		
	
	

