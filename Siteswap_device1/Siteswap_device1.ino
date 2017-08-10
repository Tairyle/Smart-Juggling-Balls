#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SimbleeGZLL.h>

//#include <SimbleeForMobile.h>

/* 
 *  Code based on Adafruit BNO055 sensor drivers
*/

//Conditional code blocks
#define Wireless false //For wireless communication via SimbleeGZLL
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
double half_sec_accel[3];
double sec_pos[4];
char siteswap_num;

//SimbleeGZLL variables
device_t role = DEVICE1;

void setup(void)
{
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  Serial.begin(9600);
  Serial.println("Device 0 Ball");
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

  half_counter = 0;
  //half_sec_pos[0] = 0;

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
//  imu::Vector<3> acceler = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

/*
  // Display the floating point data 
  Serial.print("X: ");
  Serial.print(acceler.x());
  Serial.print(" Y: ");
  Serial.print(acceler.y());
  Serial.print(" Z: ");
  Serial.print(acceler.z());
  Serial.print("\t\t");
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
      //Take siteswaps number and send to master.
      //number is in character: siteswap_num.
      //SimbleeGZLL code
      if(siteswap_num == 2 || siteswap_num == 4){
        siteswap_num = 3;
      }
      Serial.print("\nSiteswap_num:");
      Serial.println(uint8_t(siteswap_num));
      SimbleeGZLL.sendToHost(siteswap_num);
      Serial.print("In Send\n");
      
      ns = CALIBRATION;
      break;
    case MISHAP:
      //Understand what went wrong, go back to calibration
      break;
  }

  //Next State
  cs = ns;
  

  delay(BNO055_SAMPLERATE_DELAY_MS);
}

