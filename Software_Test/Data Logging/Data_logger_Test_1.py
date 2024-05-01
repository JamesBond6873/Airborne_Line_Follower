import serial
import keyboard
import numpy as np
import datetime


def create_file(file_path):
    with open(file_path, "w") as file:
        file.write("Created" + "\n")


def append_to_file(filename, message):
    with open(filename, "a") as file:
        file.write(message, "\n")


print("Starting")

current_time = str(datetime.datetime.now()).replace(" ", "_").replace("-", "_").replace(":", "")


print(current_time)

filePath = current_time + ".txt"

print(filePath)