// ------------------------------------------------------------ //
// TFMini.ino
// ------------------------------------------------------------ //

#define RXD2 21
#define TXD2 22

// ------------------------------------------------------------ //
//
// ------------------------------------------------------------ //

#include <TFMPlus.h>  // Include TFMini Plus Library v1.4.1

TFMPlus tfmP;         // Create a TFMini Plus object

#define DEG_TO_RAD 0.017453292519943295769236907684886

// Initialize variables
int16_t tfDist = 0;    // Distance to object in centimeters
int16_t tfFlux = 0;    // Strength or quality of return signal
int16_t tfTemp = 0;    // Internal temperature of Lidar sensor chip

void TFMini_setup() {
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(20);               // Give port time to initalize
  tfmP.begin( &Serial2);   // Initialize device library object and...
  // pass device serial port to the object.

  // Send some example commands to the TFMini-Plus
  // - - Perform a system reset - - - - - - - - - - -
  printf( "System reset: ");
  if ( tfmP.sendCommand( SYSTEM_RESET, 0))
  {
    printf( "passed.\r\n");
  }
  else tfmP.printReply();

  delay(500);  // added to allow the System Rest enough time to complete

  // - - Display the firmware version - - - - - - - - -
  printf( "Firmware version: ");
  if ( tfmP.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
  {
    printf( "%1u.", tfmP.version[ 0]); // print three single numbers
    printf( "%1u.", tfmP.version[ 1]); // each separated by a dot
    printf( "%1u\r\n", tfmP.version[ 2]);
  }
  else tfmP.printReply();
  // - - Set the data frame-rate to 20Hz - - - - - - - -
  printf( "Data-Frame rate: ");
  if ( tfmP.sendCommand( SET_FRAME_RATE, FRAME_20))
  {
    printf( "%2uHz.\r\n", FRAME_20);
  }
  else tfmP.printReply();
  // - - - - - - - - - - - - - - - - - - - - - - - -

  delay(500);            // And wait for half a second.
}

float TFMini_distance()
{
  float radius = 0;
  if ( tfmP.getData( tfDist, tfFlux, tfTemp)) // Get data from the device.
  {
    radius = tfDist * 10;
  }
  return radius;
}

String TFMini_calculate(float posX, float posY)
{
  float radius = TFMini_distance();
  float azimuth = posX * DEG_TO_RAD;
  float elevation = posY * DEG_TO_RAD;
  double x = - radius * sin(elevation) * cos(azimuth);
  double y = radius * sin(elevation) * sin(azimuth);
  double z = - radius * cos(elevation);
  
  String value = String(x,5) + "," + String(y,5) + "," + String(z,5);
  Serial.println(value);
  return value;
}
