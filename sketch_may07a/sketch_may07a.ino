#include <Wire.h>
#include <Arduino.h>
#include <VL53L1X.h>

#define tca1 0x70
#define tca2 0X71

VL53L1X sen;
uint8_t defaultadd = 0b0101001;
uint8_t ntca[] = {tca1,tca2};


const int num_tca = 2;

const int num_laser_data = 5;
uint16_t sensorread[num_laser_data*num_tca];

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
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  //closeall(ntca[0]);
  //closeall(ntca[1]);
  
  for (int k = 0;k < num_tca;k++){
    
    Serial.print("TCA number ");
    Serial.print(ntca[k]);
    Serial.print("\t");
    Serial.print(k);
    Serial.print("\t");

    
    for (int i=0; i < num_laser_data; i++){

      sensorread[i+num_laser_data*k] = 0;

      TCA9548A(ntca[k],i);
      sen.setTimeout(500);
      
      if (!sen.init())
      {
        
        Serial.print("Failed to detect and initialize sensor ");
        Serial.println(i);
        
        while (1);
      }
      
      sen.startContinuous(50);

    } //eof Inner loop

    Serial.println(" initialized");
  } // eof Outer loop

  closeall(ntca[0]);
  closeall(ntca[1]);

  Serial.println("start");
  
}

  
void loop() {
//  openall(ntca[0]);
  for (int k = 0;k < num_tca; k++){
    
    Serial.print(ntca[k]);
    Serial.print("\t");
    
    for (int i=0; i < num_laser_data; i++){
      
      TCA9548A(ntca[k],i);
      sensorread[i+num_laser_data*k] = sen.read();
      
      Serial.print(sensorread[i+num_laser_data*k]);
      Serial.print("\t");
      
      if (sen.timeoutOccurred()){ 
        Serial.print("TCA number ");
        Serial.println(ntca[k]);
        Serial.println("sen TIMEOUT"); 
      }
    } // eof Inner loop
    closeall(ntca[k]);
  } // eof Outer loop

  
  Serial.print("\n");
  delayMicroseconds(10);

}
