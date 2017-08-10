# Smart-Juggling-Balls
Code for SJSU CMPE senior project group: Smart Juggling Balls
A set of juggling balls with Simblee Lilypads, and an Adafruit BNO055 IMU

## Getting Started

Download the Arduino IDE, Python 2.7, CORS, and a local web server.

### Prerequisites

Arduino IDE 1.6.5
Python 2.7
Internet access
CORS
Web server
3 Smart Juggling Balls
1 Lilypad Arduino or equivalent


### Installing

Install Arduino IDE 1.6.5 and download the Simblee libraries by following steps on sparkfun's page:
https://learn.sparkfun.com/tutorials/simblee-concepts/setting-up-arduino

Upload each ball with one of the 3 Siteswap_xxxx folders. (One ball per folder)

Change the file paths under serialPy.py and webserver file location to be the same.

Install CORS add-on and local web server

Go to www.smartjuggling.com to check that it is working.

## Running the tests




## Authors

Alexander Jea - Siteswap determining algorithm, sensor work, compiling all ball software parts.
Anton Pedruco - Communication between Computer and host ball.
Justin Phan - Computer UI and all related code.
Jose Quan - All hardware components from ball design to interior circuitry.
Jason Smith - Communication between Simblee Lilypad (SimbleeGZLL).

##License

This project is licensed under the MIT license

## Acknowledgments

*Dr. Frank Lin for advising us.
*Our class professors: Hyeran Jeon and Kaikai Liu.
*Adafruit BNO055 IMU Library.
*Simblee for Simblee libraries .
*Sparkfun for the Simblee library installation guide.