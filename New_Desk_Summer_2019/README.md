# Using the New Desks

These are the files for the NEW Desks in the 1701 Building. This also includes files for the radio transmitters and recievers.

# Remote Controlled Desk

The remote controlled desk uses an rf arduino with the new_desk_controlled_by_remote.ino file. Pins 9, 10, 11, 12, and 13 are all used for radio communication.

## Connecting arduino to the desks

Below is the 7-Pin DIN pinout that the desks use for connection (with pin 1 on the male end connecting to pin 7 on the female end if looking straight on at each).
<p align="center">
  <img src="Images/Screen Shot 2019-08-13 at 2.49.53 PM.png" height="100" title="Smart Rocking Chair">
</p>
Looking at the male end, pin 2 of the connector needs to connect to pin 5 on the arduino as an INPUT_PULLUP in order to read button presses from the UP button. Pin 2 of the male connector needs to connect to pin 4 on the arduino as an INPUT_PULLUP in order to read button presses from the DOWN button. Pin 1 of the male connector must connect to GND.

Looking at the female end, pin 6 of the connector needs to connect to pin 3 on the arduino as an OUTPUT pin which is set to HIGH in order to move the desk UP. Pin 5 of the female connector needs to connect to pin 2 as an OUTPUT pin, which is also set to HIGH in order to move the desk DOWN. Both of these pins MUST be set to LOW to STOP the desk. Also, pin 4 of the female connector must connect to GND on the arduino.
