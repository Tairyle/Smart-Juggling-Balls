#include <SimbleeGZLL.h>


  device_t role = DEVICE2;
  char* array;
  
void setup() 
{
  Serial.begin(9600); // set the baud rate
  SimbleeGZLL.begin(role);
//  Serial.print("starting cpu link");
  
//  Serial.println("Ready"); // print "Ready" once

}
void loop() {
  // char in1 = random(10, 20);
  // char in2 = random(20, 30);
  // char in3 = random(30, 40);
  char ping = 0;
  
//  int siteswap[3] = {in1, in2, in3};

  // char siteswap[] = {in1 + in2 + in3};

//    Serial.println(siteswap[0]);
//    Serial.println(siteswap[1]);
//    Serial.println(siteswap[2]);
//    Serial.write(siteswap);
  SimbleeGZLL.sendToHost(ping);
  delay(6000); // delay for 4 seconds
}


void SimbleeGZLL_onReceive(device_t device, int resi, char *data, int len)
{
  if(device == HOST && len!= 0 )
  {
    array = data;
//    Serial.write(uint32_t(array));
//    Serial.print("Received data");
    Serial.print(uint8_t(array[0]));
    Serial.print(uint8_t(array[1]));
    Serial.print(uint8_t(array[2]));
  }
  if(data == "196360")
  {
    Serial.print("Garbage");
  }
  
}

