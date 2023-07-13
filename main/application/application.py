import serial

arduino_port= "COM8"
baud_rate= 9600

ser = serial.Serial(arduino_port, baud_rate)


while True:
    data = ser.readline().decode().rstrip()  # Read a line of data and decode it to a string
    print(data)

ser.close()