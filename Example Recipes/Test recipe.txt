!M555; Things outside the command are treated as comments. This command is invalid and should be ignored GOOD
!M2; Ping GOOD
!G0,X100,R100,F500,P200; move requiring ping. GOOD
!G4,T1000; wait for t ms
!M0; estop GOOD
!G1,X150,R0,F2000; Controlled move not needing ping GOOD
!M1; release ESTOP GOOD
!M24,S0; pause GOOD
!M24,S1; resume GOOD
!M114; get the motor's position and speed GOOD
!G91; relative positioning GOOD
!G90; absolute positioning GOOD
!M208,X120; // set x's max travel GOOD
!M92,X15,R1000; // set steps per unit FINE FOR NOW
!G28; // move to home GOOD
!M42,P6,S1; // set output pin 0 to on GOOD