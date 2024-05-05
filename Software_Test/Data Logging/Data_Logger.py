import serial
import keyboard
import numpy as np
import datetime

ser = serial.Serial('COM13', 9600) # Change the port name to the correct one


def create_file(file_path):
    with open(file_path, "w") as file:
        file.write("Created" + "\n")


def append_to_file(filename, message):
    with open(filename, "a") as file:
        file.write(message + "\n")


print("Starting")

path = "C:\\Users\\ffpa0\\Desktop\\Code\\Line Follower\\Software_Test\\Data Logging"
current_time = str(datetime.datetime.now()).replace(" ", "_").replace("-", "_").replace(":", "")
filePath = path + current_time + ".txt"
print(filePath)


create_file(filePath)


while True:
    if ser.in_waiting > 0:
        message = ser.readline().decode().strip()
        print(message)

        append_to_file(filePath, message)

    if keyboard.is_pressed("q"):
        print("Finishing")
        quit()

