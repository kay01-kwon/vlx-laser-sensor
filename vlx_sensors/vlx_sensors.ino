#define USE_USBCON  // for using ros with due

#include <ros.h>
#include <Wire.h>
#include <Arduino.h>
#include <VL53L1X.h>
#include <vehicle_control/vl53l1xMsg.h>

#define tca1 0x70
#define tca2 0X71

VL53L1X sen;
uint8_t defaultadd = 0b0101001;
uint8_t ntca[] = {tca1,tca2};


const int num_tca = 2;

const int num_laser_data = 7;
uint16_t sensorread[num_laser_data*num_tca];

ros::NodeHandle nh;
vehicle_control::vl53l1xMsg laser_data;

ros::Publisher laser_chatter("laser",&laser_data);

void TCA9548A(uint8_t tca, uint8_t bus)
{
  
  Wire.beginTransmission(tca);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

void closeall(uint8_t tca){
  Wire.beginTransmission(tca);  
  Wire.write(0b00000000);          
  Wire.endTransmission();
}


void setup() {

  nh.initNode();
  nh.advertise(laser_chatter);
  
  Wire.begin();
  Wire.setClock(400000);

  closeall(ntca[0]);
  closeall(ntca[1]);
  
  for (int k = 0;k < num_tca;k++){
    for (int i=0; i < num_laser_data; i++){

      sensorread[i+num_laser_data*k] = 0;

      TCA9548A(ntca[k],i);
      sen.setTimeout(500);
      
      if (!sen.init())
      {        
        while (1);
      }
      
      sen.startContinuous(50);

    } //eof Inner loop

  } // eof Outer loop

  closeall(ntca[0]);
  closeall(ntca[1]);
  
}

  
void loop() {

for (int k = 0;k < num_tca; k++){
  for (int i=0; i < num_laser_data; i++){
    TCA9548A(ntca[k],i);
    laser_data.laser[k*7+i] = sen.read();
  } // eof Inner loop
  closeall(ntca[k]);
} // eof Outer loop
  
  laser_chatter.publish(&laser_data);
  
  delayMicroseconds(10);

  nh.spinOnce(); 
}
