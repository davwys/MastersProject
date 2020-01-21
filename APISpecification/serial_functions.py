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
            if element.product is not None or "GameBoard" in element.device or "Oracle" in element.device:
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
def select_port(self, port, oracle):

    if str(port) == 'No Devices Found':
        return

    try:
        self.selectedPort = port
        idx = self.ports.index(port)

        # Enable serial port (system dependent)
        if platform.system() is not "Windows":
            if platform.system() != "Darwin":
                self.ser = serial.Serial(str(self.selectedPort), 57600)  # open serial port
            else:
                self.ser = serial.Serial('/dev/cu.' + str(self.selectedPort), 57600)  # open serial port
        else:
            print("opening port: {}".format(self.selectedPort))
            self.ser = serial.Serial(str(self.selectedPort), 57600)  # open serial port

        if not oracle:
            time.sleep(1)
            # Change to playing mode
            self.ser.write(b'CHANGE_MODE=3')
            self.ser.flush()

        # Start serial read thread
        thread = threading.Thread(target=self.read_from_port, args=(self.ser, oracle))
        thread.start()

        self.load_current_format(False)

    except OSError:
        print('Error: Could not open port')


# Read and handle serial input data
def read_from_port(self, ser, oracle):
    while not self.stopThread:
        reading = ser.readline().decode()
        print('Got: ' + str(reading))
        if not oracle:
            self.handle_playing_message(reading)
        else:
            self.handle_oracle_message(reading)
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


def handle_playing_message(self, msg):
    start = "{"
    end = "}"
    s_start = "Area='"
    s_end = "'_"
    c_start = "CardID="
    message = msg[msg.find(start) + len(start):msg.rfind(end)]
    # message example: SensorID=1_CardID=072_RP
    if "Area='" not in message or "CardID=" not in message:
        return

    area_name = message[message.find(s_start) + len(s_start):message.rfind(s_end)]
    card_id_string = message[message.find(c_start) + len(c_start):len(message)-8]  # Last 8 digits reserved as area type
    card_id = int(card_id_string)
    type_string = message[len(message)-2:]

    # Print read data to log (disabled for now)
    print("area name: {}".format(area_name))
    print("Card ID: {}".format(card_id))
    print("Type: {}".format(type_string))

    self.areaName = str(area_name)
    self.cardId = str(card_id)
    self.type = str(type_string)

    self.ids['card'].text = self.cardId
    self.ids['area'].text = self.areaName
    self.ids['type'].text = self.type

    # Format preview call
    self.currentData = self.api_name_handler(self.callFormat)

    # Send actual API call
    self.send_api_call()


def handle_oracle_message(self, msg):

    if "ORACLE=" not in msg:
        return

    start = "{"
    end = "}"
    message = msg[msg.find(start):msg.rfind(end)+1]
    # message example: {12,52,72}

    self.array = str(message)
    self.ids['array'].text = self.array

    # Format preview call
    self.currentData = self.api_name_handler(self.callFormat)

    # Send actual API call
    self.send_api_call()
