import sys
import glob
import serial
import serial.tools.list_ports
import platform
import numpy as np
import time

# This file contains all functions related to serial communications,
# e.g. communications between the training dashboard and the game
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
            index = element.description.find('(')
            cut_string = element.description[:index - 1]
            names.append(cut_string)
            self.selectedPort_Windows = element.device

    if len(names) == 0:
        names.append('No Devices Found')

    result = names
    return result


# Update the list of available ports
def update_ports(self):
    self.update_log('Getting device list...')
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
        self.update_log("Port selected: {}".format(port))
        self.selectedPort = port
        idx = self.ports.index(port)

        # Enable serial port (system dependent)
        if platform.system() is not "Windows":
            self.ser = serial.Serial('/dev/cu.' + str(self.selectedPort), 57600)  # open serial port
        else:
            self.ser = serial.Serial(str(self.selectedPort_Windows), 57600)  # open serial port

        # Enable start button
        self.ids.start_training.disabled = False
    except OSError:
        self.update_log('Error: Could not open port')
        self.toggle_log(True)


# Read and handle serial input data
def read_from_port(self, ser):
    while not self.stopThread:
        reading = ser.readline().decode()
        self.update_log('Got: ' + str(reading))
        self.handle_training_message(reading)
        # Kills the thread when stop flag is set
        if self.stopThread:
            break


# Reads input (called via dedicated serial thread)
def read_training_input(self):
    i = 0
    while i < 25:
        txt = self.ser.read(1)
        self.update_log(str(txt))
        i += 1


# Uploads a given training data set (only Area names and Sensor IDs) to the main board
# Main board HAS TO BE in UPLOAD mode for this!
def upload_training_data(self, data):
    print("Data to upload: ")
    print(str(data))
    self.update_log('Beginning upload...')

    # Convert to Numpy array
    data = np.array(data)

    # Sort by sensor ID
    data = data[np.argsort(data[:, 1])]

    # For each row (example: ['AreaA', 3, 123])
    for value in data:
        # Extract first two values (sensor ID, area name)
        dat = [value[1], value[0]]
        print('Uploading: {}'.format(dat))

        # Format for upload (example: {1=AreaA}
        upl = "{" + str(dat[0]) + "=" + str(dat[1]) + "}"
        print('Data is: {}'.format(upl))

        # Write to serial port (encoded as bytes)
        self.ser.write(upl.encode('utf-8'))

    # Wait for 2s before sending end command to prevent errors
    time.sleep(2)
    self.ser.write(b'UPLOAD_END')

    time.sleep(2)
    # Close serial port
    self.ser.close()
    # Switch to Ready mode happens on controller side
