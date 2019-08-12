/*************************************************** 
  This is a library example for the MLX90614 Temp Sensor
  Designed specifically to work with the MLX90614 sensors in the Keyestudio shop

  These sensors use I2C to communicate, 2 pins are required to 
 ****************************************************/

#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(9600);

  Serial.println("Adafruit MLX90614 test");  

  mlx.begin();  
}

void loop() {
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  Serial.println();
  delay(500);
}
