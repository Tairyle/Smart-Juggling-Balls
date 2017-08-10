import serial
import os
# import simplejson

ser = serial.Serial('COM5', 9600) # Establish the connection on a specific port
print(ser.name)

siteswap = []
while True:
    for x in range (0, 3):
        siteswap.append(ser.read())
    print(siteswap)
    with open(os.path.join('C:\Users\Anton Pedruco\Documents\webserver','siteswap.txt'), 'w') as f:
        f.write("{\"siteswap\": \"")
        for item in siteswap:
        	f.write(item)
        f.write("\"}")	
        f.write('\n')
    siteswap = []        