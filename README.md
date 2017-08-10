# Smart-Juggling-Balls

Code for SJSU CMPE senior project group: Smart Juggling Balls

A set of juggling balls with Simblee Lilypads, and an Adafruit BNO055 IMU

## Getting Started

The following states our software requirements for function. Hardware description is not included here.

### Prerequisites

* Arduino IDE 1.6.5
* Python 2.7
* Internet access
* CORS
* Web server
* 3 Smart Juggling Balls
* 1 Lilypad Arduino or equivalent

### Code Structure

* Siteswap_device0, Siteswap_device1, and Siteswap_host includes the Arduino sketches for the ball code. These include the state machine and algorithm for individually determining the ball's Siteswap number. The host has additional code to send the all ball's Siteswap number when it has received an entire set.

* Serial Communication includes the code to interface with the host computer. The Arduino code receives data from the host via Gazell, and prints the data via Serial. The python script catches this data and writes it to a file in a certain directory.

* juggle_with_me includes the code for the web ui and connection to the python script for data. Pythonanywhere is used to run the python script for the backend, Flask is used for the web ui, and SQLAlechemy is used for the database connection. Amazon Web Services hosts the database.

### Installing

Install Arduino IDE 1.6.5 and download the Simblee libraries by following steps on sparkfun's page:
https://learn.sparkfun.com/tutorials/simblee-concepts/setting-up-arduino

Upload each ball with one of the 3 Siteswap_xxxx folders. (One ball per folder)

Change the file paths under serialPy.py and webserver file location to be the same.

Install CORS add-on and local web server

Go to www.smartjuggling.com to check that it is working.

## Running the code

### Juggling Balls

1. Start Arduino IDE and open ino files in Siteswap labeled folders.
2. Program each ball with different code.
3. Begin juggling.

### Website

1. Start CORS and your local web server of choice.
2. Go to www.smartjuggling.com and view the trick attack page.
3. Wait for juggling response to be received, and for the website to update.


## Authors

Alexander Jea - Siteswap determining algorithm, sensor work, compiling all ball software parts.

Anton Pedruco - Communication between Computer and host ball.

Justin Phan - Web UI and all related code.

Jose Quan - All hardware components from ball design to interior circuitry.

Jason Smith - Communication between Simblee Lilypad (SimbleeGZLL).

## Acknowledgments

* Dr. Frank Lin for advising us.
* Our class professors: Hyeran Jeon and Kaikai Liu.
* Adafruit BNO055 IMU Library.
* Simblee for Simblee libraries .
* Sparkfun for the Simblee library installation guide.
