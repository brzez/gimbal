#include <Wire.h> //I2C Arduino Library
#include <Servo.h>

#define addr 0x1E //I2C Address for The HMC5883
#define DEBUG true
#define DEBUG_MESSAGE_INTERVAL 1000



struct Axis {
  int val;
  int val_min;
  int val_max;
  int mapped;
};

void update_axis(Axis *a, int value) {
  a->val = value;
  a->val_min = min(a->val_min, value);
  a->val_max = max(a->val_max, value);
  a->mapped = map(value, a->val_min, a->val_max, 0, 360);
}

void debug_axis (Axis a) {
  Serial.print(" value: ");
  Serial.print(a.val);
  Serial.print(" val_min: ");
  Serial.print(a.val_min);
  Serial.print(" val_max: ");
  Serial.print(a.val_max);
  Serial.print(" mapped: ");
  Serial.print(a.mapped);
  Serial.println("");
}


Servo servo_x;
Servo servo_z;

Axis x_axis = {10,10,10,10};
Axis y_axis;
Axis z_axis;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  servo_x.attach(5);
  servo_z.attach(9);
  
  
  Wire.beginTransmission(addr); //start talking
  Wire.write(0x02); // Set the Register
  Wire.write(0x00); // Tell the HMC5883 to Continuously Measure
  Wire.endTransmission();
}

int x,y,z;
unsigned long next_debug_print = 0;

void read_acc () {
  //Tell the HMC what regist to begin writing data into
  Wire.beginTransmission(addr);
  Wire.write(0x03); //start with register 3.
  Wire.endTransmission();
  
 
 //Read the data.. 2 bytes for each axis.. 6 total bytes
  Wire.requestFrom(addr, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //MSB  x 
    x |= Wire.read(); //LSB  x
    z = Wire.read()<<8; //MSB  z
    z |= Wire.read(); //LSB z
    y = Wire.read()<<8; //MSB y
    y |= Wire.read(); //LSB y
  }
}


void loop(){
  read_acc();

  servo_z.write(z_axis.mapped);
  servo_x.write(x_axis.mapped);
  
  update_axis(&x_axis, x);
  update_axis(&y_axis, y);
  update_axis(&z_axis, z);

  if (DEBUG) {
    if (millis() > next_debug_print) {
      next_debug_print = millis() + DEBUG_MESSAGE_INTERVAL;
      Serial.println("x_axis: ");
      debug_axis(x_axis);
      Serial.println("y_axis: ");
      debug_axis(y_axis);
      Serial.println("z_axis: ");
      debug_axis(z_axis);
    }
  }
}
