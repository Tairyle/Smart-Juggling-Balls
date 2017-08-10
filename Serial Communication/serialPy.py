import serial
import os


ser = serial.Serial('COM5', 9600) # Establish the connection on a specific port
print(ser.name)                     #print's the name of the port being used to show it's connected

siteswap = []                       #creates an empty list to be populated
while True:
    for x in range (0, 3):
        siteswap.append(ser.read())   
    print(siteswap)
    with open(os.path.join('C:\Users\Anton Pedruco\Documents\webserver','siteswap.txt'), 'w') as f: #location of the webserver's parent directory
        f.write("{\"siteswap\": \"")
        for item in siteswap:
        	f.write(item)
        f.write("\"}")	
        f.write('\n')
    siteswap = []                   #resets list to empty