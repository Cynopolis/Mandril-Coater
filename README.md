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

Gcode commands necessary

	- Architecture commands
		○ Ping
			§ !M2;
		○ Queue is empty
			§ !QueueEmpty;
			§ This command is only sent by the controller when there are no more gcode commands in its queue
		○ Wait
			§ !G4,Pnnn;
				□ Pnnn - the time to wait in ms
		○ Estop
			§ !M0;
		○ Release Estop
			§ !M1;
		○ Pause/Resume
			§ !M24,Sn;
				□ S0 - Pause
				□ S1 resume
		○ Get motor positions
			§ M114
				□ Returns !M114,Xnnn,Rnnn,Fnnn,Snnn;
					® Xnnn - the current x position
					® Rnnn - the current R rotation in degrees
					® Fnnn - the current feed rate in mm/sec
					® Snnn - the current rotational feed rate in deg/sec
					
	- Configure commands
		○ Relative move
			§ !G91;
		○ Absolute mode
			§ !G90;
		○ Set max travel
			§ !M208,Xnnn;
				□ Maximum travel in the X axis in mm
				□ No rotation max travel can be set
		○ Set step/unit
			§ !M92,Xnnn,Rnnn;
				□ Steps per mm for the x axis
				□ Steps per degree for the R axis
				
	- Actuate commands
		○ Linear move
			§ !G1,Xnnn,Rnnn,Fnnn;
				□ Xnnn - the position to move linearly in mm
				□ Rnnn - the number of degrees to rotate
				□ Fnnn - the amount to move the x-axis in mm/min. The rotation axis will sync so it completes its move when the linear axis completes its move.
		○ Coast move
			§ !G0,Xnnn,Rnnn,Fnnn,Pnnn,Sn; (I'm aware this isn't technically correct
				□ Xnnn - the position to move linearly in mm
				□ Rnnn - the number of degrees to rotate
				□ Fnnn - the amount to move the x-axis in mm/min.
				□ Pnnn - the amount to move the r-axis in deg/min
				□ S0 - stop coasting
				□ Sx - any other S (or no S) will assume you want to start coasting
				□ This will time out after 1 second if a ping command isn't given every one second
				
		○ Home
			§ !G28;
		○ Switch I/O pin
			§ !M42,Pnnn,Sn;
				□ Pnnn - pin number
				□ S0 - Pin on
				□ S1 - pin off
		
		
	
	

