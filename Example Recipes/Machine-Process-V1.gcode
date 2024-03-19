!M42,P8; TODO: This sets the alarm, but we only want to do this if we aren't homed

!G0,R1000000,P100; Start a non-blocking mandrel rotatation and let it go for a long time
!G4,T1000; Wait one second
TODO: Set linear motor deceration very high here
!G1,X10,F100; Move to spraying start position 
!M42,P8,S1; Turn on Y9
!G1,X1000,R3600,F100; Move to X1000 and rotate the R axis 3600 degrees
!M42,P8,S0; Turn off Y9
TODO: Set linear motor deceration back to default here
!G1,X10000,F100; Move the X axis far enough so that it's guaranteed to hit the far endstop and stop

Clean up Step
!G4,T60000; Delay for 60 seconds
!M42,P9,S1; Turn on Y10
!G4,T3000; Delay 3 seconds
!M42,P9,S0;

Now begin the process but in reverse
!G0,R1000000,P100; Start a non-blocking mandrel rotatation and let it go for a long time
TODO: Set linear motor deceration very high here
!G1,X1010,F100; Move to spraying start position
!M42,P8,S1; Turn on Y9
!G1,X10,R3600,F100; Move to X10 and rotate the R axis 3600 degrees
!M42,P8,S0; Turn off Y9
TODO: Set linear motor deceration back to default here
!G28; Re-home

TODO: Repeat
