import kivy
from kivy.config import Config
Config.set('graphics', 'resizable', 1)
Config.set('graphics', 'width', '1000')
Config.set('graphics', 'height', '750')
Config.set('graphics', 'minimum_width', '1000')
Config.set('graphics', 'minimum_height', '750')
from kivy.core.window import Window
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.spinner import Spinner
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
import sys
import glob
import serial
import serial.tools.list_ports
import platform
import threading

Window.clearcolor = (0.8, 0.8, 0.8, 1)


class MyGrid(Widget):

    log = ''
    ser = serial.Serial(None)
    connected = False
    ports = ['No Devices Found']
    selectedPort = None
    selectedPort_Windows = None

    # Temporary data storage
    tempData = [None, None, None]

    # Check for restarts
    restarted = False

    # Training input array:
    # =====================
    #
    # Format:
    # [[Area, SensorID, CardID]]
    #
    # Example:
    # [
    #   [Oracle, 3, 72],
    #   [Investor, 2, 32],
    #   [Company, 1, 32]
    # ]
    trainingInput = []

    # Read and handle serial input data
    def read_from_port(self, ser):
        while True:
            reading = ser.readline().decode()
            self.update_log('Received: ' + str(reading))
            self.handle_training_message(reading)

    def handle_training_message(self, msg):
        start = "{"
        end = "}"
        s_start = "SensorID="
        s_end = "_"
        c_start = "CardID="
        message = msg[msg.find(start)+len(start):msg.rfind(end)]
        # message example: SensorID=1_CardID=072
        if "SensorID=" not in message or "CardID=" not in message:
            return

        sensor_id_string = message[message.find(s_start) + len(s_start):message.rfind(s_end)]
        card_id_string = message[message.find(c_start) + len(c_start):len(message)]
        sensor_id = int(sensor_id_string)
        card_id = int(card_id_string)
        print("Sensor ID: {}".format(sensor_id))
        print("Card ID: {}".format(card_id))

        # Validate both IDs and save to temporary storage
        if sensor_id > 0 and 0 < card_id < 999:
            self.tempData = [None, sensor_id, card_id]
            self.request_area_name()

    # Get a list of available serial ports
    def serial_ports(self):
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
                cut_string = element.description[:index-1]
                names.append(cut_string)
                self.selectedPort_Windows = element.device

        if len(names) == 0:
            names.append('No Devices Found')

        result = names
        return result

    # Update the list of available ports
    def update_ports(self):
        self.update_log('Getting device list...')
        self.ports = self.serial_ports()
        self.ids.port_dropdown.values = self.ports

        if self.ids.port_dropdown.values[0] == 'No Devices Found':
            self.ids.port_dropdown.text = 'No Devices Found'
            self.ids.port_dropdown.values = ''

    # Start or finish the training process
    def start_training(self):
        self.ids.restart.disabled = False
        self.update_log('Started Training!')
        self.ids.instructions.text = '''1. Activate each area (in alphabetical order)'''

        self.ids.start_training.disabled = True
        self.ser.write(b'CHANGE_MODE=3')  # Change to training mode
        self.ser.flush()

        # Start training TODO kill after completion
        if not self.restarted:
            thread = threading.Thread(target=self.read_from_port, args=(self.ser,))
            thread.start()

    def request_area_name(self):
        self.ids.area_name.disabled = False
        self.ids.area_name.text = ''
        self.ids.submit_name.disabled = False

    def submit_area_name(self, name):
        if 0 < len(name) < 20:
            self.ids.area_name.disabled = True
            self.ids.area_name.text = ''
            self.ids.submit_name.disabled = True

            # Save name to temp data
            self.tempData[0] = name

            # Confirm received training data
            self.ser.write(b'TRAIN_OK')
            self.save_training_data(self.tempData)
        else:
            self.update_log('Error: Name too long')

    def save_training_data(self, data):
        self.trainingInput.append(data)
        print("Training data now at:")
        print(self.trainingInput)

    # Upload new training data
    def upload(self):
        self.ids.restart.disabled = True
        self.update_log('Upload starting...')
        self.ids.upload.disabled = True
        if self.selectedPort is not None:
            self.update_log("Connecting to port {}...".format(self.selectedPort))
            try:
                self.update_log('Connected to port\n{}, uploading...'.format(self.ser.name))
                self.ser.write(b'CHANGE_MODE=4')  # write data as bytes
                self.ids.upload.disabled = True
                # self.ser.close()  # close port TODO check need
            except OSError:
                self.update_log('Error occurred')  # TODO Error handling

        # TODO integrate into check if upload actually worked
        self.ids.instructions.text = '''All done! 
You can close the dashboard
and start playing'''

    # Restarts the training process
    def restart_training(self):

        self.restarted = True
        self.ids.start_training.disabled = False
        self.ids.upload.disabled = True
        self.ids.start_training.text = 'Start Training'
        self.ids.instructions.text = 'Press "Start Training"'
        self.update_log('Restarting training...')  # TODO disconnect as well
        # Send restart command
        self.ser.write(b'RESTART_TRAINING')  # write data as bytes
        self.ids.restart.disabled = True
        self.tempData = None
        self.trainingInput = []

        # disable name input
        self.ids.area_name.disabled = True
        self.ids.area_name.text = ''
        self.ids.submit_name.disabled = True

    # Toggle Log visibility
    def toggle_log(self, value):
        if not value:
            self.ids.log.height = 0
            self.ids.log.size_hint_y = None
            self.ids.log.text = ''
        else:
            self.ids.log.height = self.parent.height * 0.72
            self.ids.log.size_hint_y = None
            self.ids.log.text = 'Log:\n' + self.log

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

    # Update the integrated log
    def update_log(self, text):
        print(text)
        self.log += (text + '\n')

        # Update Log text only if visible
        if self.ids.chk.active:
            self.ids.log.text = 'Log:\n' + self.log

    def read_training_input(self):
        i = 0
        while i < 25:
            txt = self.ser.read(1)
            self.update_log(str(txt))
            i += 1


# Main App definition
class MyApp(App):
    title = "Game Board Training Dashboard"

    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()
