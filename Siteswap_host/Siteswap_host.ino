#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SimbleeGZLL.h>

/* 
 *  Code based on Adafruit BNO055 sensor drivers
*/

//Conditional code blocks
/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (250)
#define W_RANGE (0.3)
#define X_RANGE (0.5)
#define Y_RANGE (0.5)
#define Z_RANGE (0.5)

Adafruit_BNO055 bno = Adafruit_BNO055(); 
//Siteswaps variables
int half_counter;
double og_pos[4];
double half_sec_pos[4];
double sec_pos[4];
char siteswap_num;

//SimbleeGZLL variables
device_t role = HOST;
char d0_siteswap;
char d1_siteswap;
bool send_flag;
bool nothing;

void setup(void)
{
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  Serial.begin(9600);
  Serial.println("Host Ball");
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  bno.setExtCrystalUse(true);
  //Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");

  half_counter = 0;
  //half_sec_pos[0] = 0;
  d0_siteswap = 0;
  d1_siteswap = 0;
  nothing = 0;
  
  //Begin GZLL Host
  SimbleeGZLL.begin(role);
}

typedef enum
{
  CALIBRATION = 0,
  TRAVEL = 1,
  SEND = 2,
  MISHAP = 3
} state_machine_t;

state_machine_t cs;
state_machine_t ns;



void loop(void)
{
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  //imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);


  // Display the floating point data 
  /*
   * //Serial Printing
  Serial.print("X: ");
  Serial.print(euler.x());
  Serial.print(" Y: ");
  Serial.print(euler.y());
  Serial.print(" Z: ");
  Serial.print(euler.z());
  Serial.print("\t\t");
  Serial.print("StartSt: ");
  Serial.print(cs);
  Serial.print("\t");
  */
  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  //Start Siteswaps calcs
  switch(cs)
  {
    case CALIBRATION:
      if (half_counter < 4){
        if (half_sec_pos[0] == 0){
          half_sec_pos[0] = quat.w();
          half_sec_pos[1] = quat.x();
          half_sec_pos[2] = quat.y();
          half_sec_pos[3] = quat.z();
          half_counter += 1;
        }
        else if((half_sec_pos[0]-0.04<quat.w() && quat.w()<half_sec_pos[0]+0.04) &&
                (half_sec_pos[1]-0.06<quat.x() && quat.x()<half_sec_pos[1]+0.06) &&
                (half_sec_pos[2]-0.06<quat.y() && quat.y()<half_sec_pos[2]+0.06) &&
                (half_sec_pos[3]-0.06<quat.z() && quat.z()<half_sec_pos[3]+0.06) &&
                (half_counter % 2 == 0))
        {
          //Serial.print("even");
          sec_pos[0] = quat.w();
          sec_pos[1] = quat.x();
          sec_pos[2] = quat.y();
          sec_pos[3] = quat.z();
          half_counter += 1;
        }
        else if((sec_pos[0]-0.04<quat.w() && quat.w()<sec_pos[0]+0.04) &&
                (sec_pos[1]-0.06<quat.x() && quat.x()<sec_pos[1]+0.06) &&
                (sec_pos[2]-0.06<quat.y() && quat.y()<sec_pos[2]+0.06) &&
                (sec_pos[3]-0.06<quat.z() && quat.z()<sec_pos[3]+0.06) &&
                (half_counter % 2 == 1))
        {
          //Serial.print("odd");
          half_sec_pos[0] = quat.w();
          half_sec_pos[1] = quat.x();
          half_sec_pos[2] = quat.y();
          half_sec_pos[3] = quat.z();
          half_counter += 1;
        }
        else{
          half_sec_pos[0] = quat.w();
          half_sec_pos[1] = quat.x();
          half_sec_pos[2] = quat.y();
          half_sec_pos[3] = quat.z();
          half_counter = 0;
        }
        ns = CALIBRATION;
      }
      else if (half_counter >= 4){
        og_pos[0] = quat.w();
        og_pos[1] = quat.x();
        og_pos[2] = quat.y();
        og_pos[3] = quat.z();
        half_sec_pos[0] = quat.w();
        half_sec_pos[1] = quat.x();
        half_sec_pos[2] = quat.y();
        half_sec_pos[3] = quat.z();

        half_counter = 0;
        Serial.println("Travel");
        nothing = 1;
        ns = TRAVEL;
      }
      break;
    case TRAVEL:
      //if values stabilized, check for even counter. assign siteswaps = half_counter/2 
      if((half_sec_pos[0]-W_RANGE<quat.w() && quat.w()<half_sec_pos[0]+W_RANGE) &&
         (half_sec_pos[1]-X_RANGE<quat.x() && quat.x()<half_sec_pos[1]+X_RANGE) &&
         (half_sec_pos[2]-Y_RANGE<quat.y() && quat.y()<half_sec_pos[2]+Y_RANGE) &&
         (half_sec_pos[3]-Z_RANGE<quat.z() && quat.z()<half_sec_pos[3]+Z_RANGE) &&
//         (half_sec_accel[0]-1<acceler.x() && acceler.x()<half_sec_accel[0]+1) &&
//         (half_sec_accel[1]-1<acceler.y() && acceler.y()<half_sec_accel[1]+1) &&
//         (half_sec_accel[2]-1<acceler.z() && acceler.z()<half_sec_accel[2]+1) &&
         (half_counter > 1)){
          siteswap_num = (char)(half_counter);
          half_counter = 0;
          ns = SEND;
      }
      //if values not stabilized, counter up and continue
      else if((half_sec_pos[0]-W_RANGE>quat.w() || quat.w()>half_sec_pos[0]+W_RANGE) ||
              (half_sec_pos[1]-X_RANGE>quat.x() || quat.x()>half_sec_pos[1]+X_RANGE) ||
              (half_sec_pos[2]-Y_RANGE>quat.y() || quat.y()>half_sec_pos[2]+Y_RANGE) ||
              (half_sec_pos[3]-Z_RANGE>quat.z() || quat.z()>half_sec_pos[3]+Z_RANGE)
//              (half_sec_accel[0]-1>acceler.x() || acceler.x()>half_sec_accel[0]+1) ||
//              (half_sec_accel[1]-1>acceler.y() || acceler.y()>half_sec_accel[1]+1) ||
//              (half_sec_accel[2]-1>acceler.z() || acceler.z()>half_sec_accel[2]+1)
              ){
        half_sec_pos[0] = quat.w();
        half_sec_pos[1] = quat.x();
        half_sec_pos[2] = quat.y();
        half_sec_pos[3] = quat.z();
//        half_sec_accel[0] = acceler.x();
//        half_sec_accel[1] = acceler.y();
//        half_sec_accel[2] = acceler.z();
        half_counter += 1;
        ns = TRAVEL;
      }
      else{
        half_sec_pos[0] = quat.w();
        half_sec_pos[1] = quat.x();
        half_sec_pos[2] = quat.y();
        half_sec_pos[3] = quat.z();
//        half_sec_accel[0] = acceler.x();
//        half_sec_accel[1] = acceler.y();
//        half_sec_accel[2] = acceler.z();
        ns = TRAVEL;
      }
      

      //if values stabilized but not even, wait for next half second cycle. Maybe not?

      //if values drastically change, go to mishap. Worry about error checking later
      
      break;
    case SEND:
      
      //number is in character: siteswap_num.
      //SimbleeGZLL code
      if(siteswap_num == 2 || siteswap_num == 4){
        siteswap_num = 3;
      }
      Serial.print("In Send\n");
      send_flag = true;
      /*
      SimbleeGZLL.end();
      SimbleeGZLL.begin(role);
      */
      ns = CALIBRATION;
      break;
    case MISHAP:
      //Understand what went wrong, go back to calibration
      break;
  }

  //Sensorless debugging
  /*
  if(d0_siteswap != 0)
  {
    d1_siteswap = 3;
    siteswap_num = 3;
    send_flag = 1;
  }
  */
  if(send_flag)
  {
    if(d0_siteswap != 0 && d1_siteswap != 0 && siteswap_num != 0)
    {
      char buff[3];
      sprintf(buff, "%c%c%c", siteswap_num, d0_siteswap, d1_siteswap);
      SimbleeGZLL.sendToDevice(DEVICE2, buff);
      Serial.print("Siteswap complete. sending: ");
      Serial.print(uint8_t(buff[0]));
      Serial.print(uint8_t(buff[1]));
      Serial.print(uint8_t(buff[2]));
      d0_siteswap = 0;
      d1_siteswap = 0;
      siteswap_num = 0;
      send_flag = 0;
      nothing = 0;
    }
    else
    {
      Serial.println("Not all data received");
    }
  }
  else if (nothing == 1)
  {
    Serial.println("Sent nothing");
    char buffr[3];
    buffr[0] = char(1);
    buffr[1] = char(1);
    buffr[2] = char(1);
    SimbleeGZLL.sendToDevice(DEVICE2, buffr);
    nothing = 0;
  }

  /*Serial comms
  //Move to next state
  Serial.print("Counter: ");
  Serial.print(half_counter);
  Serial.print("\tState: ");
  Serial.print(cs);
  Serial.print("\t");
  Serial.println(siteswap_num);
  */
  cs = ns;
  

  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void SimbleeGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  //identify incoming data as from either device0 or device1
  if (device == DEVICE0) {
    Serial.println("D0 Received");
    d0_siteswap = data[0];
  }
  else if (device == DEVICE1){
    Serial.println("D1 Received");
    d1_siteswap = data[0];
  }
  else{
    //code to send master lilypad siteswap number to host CPU
    Serial.println("Not useful");
  }
}
