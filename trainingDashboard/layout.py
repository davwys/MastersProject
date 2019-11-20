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
from kivy.uix.scrollview import ScrollView

Window.clearcolor = (0.8, 0.8, 0.8, 1)


class MyGrid(Widget):

    # load external methods
    from serial_functions import get_serial_ports, read_from_port, update_ports, select_port, read_training_input
    from input import request_area_name, submit_area_name
    from data_handling import save_training_data, handle_training_message
    from log import update_log, toggle_log

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

    # Start or finish the training process
    def start_training(self):
        self.ids.restart.disabled = False
        self.update_log('Started Training!')
        self.ids.instructions.text = '''Activate an area and add a name'''

        self.ids.start_training.disabled = True
        self.ser.write(b'CHANGE_MODE=3')  # Change to training mode
        self.ser.flush()

        # Start training TODO kill after completion
        if not self.restarted:
            thread = threading.Thread(target=self.read_from_port, args=(self.ser,))
            thread.start()

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


# Main App definition
class MyApp(App):
    title = "Game Board Training Dashboard"

    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()
