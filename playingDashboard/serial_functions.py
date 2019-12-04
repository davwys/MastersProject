import sys
import glob
import serial
import serial.tools.list_ports
import platform
import numpy as np
import time
import threading

# This file contains all functions related to serial communications,
# e.g. communications between the playing dashboard and the game
# board's main controller.


# Get a list of available serial ports
def get_serial_ports(self):
    names = []
    comlist = serial.tools.list_ports.comports()
    for element in comlist:
        # macOS: filter device
        if platform.system() is not "Windows":
            # Filter out ports with no product description - add all that contain "GameBoard" (for Bluetooth ports)
            if element.product is not None or "GameBoard" in element.device:
                name = str(element.device).replace('/dev/cu.', '').replace('/dev/tty.', '')
                names.append(name)
        # Windows: get nice name for COM ports
        else:
            names.append(element.device)
            self.selectedPort = element.device

    if len(names) == 0:
        names.append('No Devices Found')

    result = names
    return result


# Update the list of available ports
def update_ports(self):
    print('Getting device list...')
    self.ports = self.get_serial_ports()
    self.ids.port_dropdown.values = self.ports

    if self.ids.port_dropdown.values[0] == 'No Devices Found':
        self.ids.port_dropdown.text = 'No Devices Found'
        self.ids.port_dropdown.values = ''


# Selects a given port to connect to
def select_port(self, port):

    if str(port) == 'No Devices Found':
        return

    try:
        self.selectedPort = port
        idx = self.ports.index(port)

        # Enable serial port (system dependent)
        if platform.system() is not "Windows":
            if platform.system() is not "Darwin":
                self.ser = serial.Serial(str(self.selectedPort), 57600)  # open serial port
            else:
                self.ser = serial.Serial('/dev/cu.' + str(self.selectedPort), 57600)  # open serial port
        else:
            print("opening port: {}".format(self.selectedPort))
            self.ser = serial.Serial(str(self.selectedPort), 57600)  # open serial port

        time.sleep(1)
        # Change to playing mode
        self.ser.write(b'CHANGE_MODE=3')
        self.ser.flush()

        # Start serial read thread
        thread = threading.Thread(target=self.read_from_port, args=(self.ser,))
        thread.start()

    except OSError:
        print('Error: Could not open port')


# Read and handle serial input data
def read_from_port(self, ser):
    while not self.stopThread:
        reading = ser.readline().decode()
        print('Got: ' + str(reading))
        self.handle_playing_message(reading)
        # Kills the thread when stop flag is set
        if self.stopThread:
            break


# Reads input (called via dedicated serial thread)
def read_playing_input(self):
    i = 0
    while i < 25:
        txt = self.ser.read(1)
        print(str(txt))
        i += 1
