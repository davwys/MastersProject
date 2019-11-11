import kivy
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.spinner import Spinner
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.core.window import Window
import sys
import glob
import serial
import serial.tools.list_ports

Window.clearcolor = (0.8, 0.8, 0.8, 1)


# Get a list of available serial ports TODO check on windows
def serial_ports():
    names = []
    descriptions = []
    comlist = serial.tools.list_ports.comports()
    for element in comlist:
        if element.product is not None: # Filter out non-applicable ports with no product description (non-connected)
            name = str(element.device).replace('/dev/cu.', '').replace('/dev/tty.', '')
            names.append(name)
            descriptions.append(element.description)
    if len(names) == 0:
        names.append('No Devices Found')
        descriptions.append('-')

    result = [names, descriptions]
    return result


class MyGrid(Widget):
    log = ''
    ser = serial.Serial(None)
    connected = False
    ports = ['No Devices Found']
    descriptions = ['-']
    selectedPort = None
    selectedPortDescription = None



    # Update the list of available ports
    def update_ports(self):
        self.update_log('Getting device list...')
        info = serial_ports()
        self.ports = info[0]
        self.descriptions = info[1]
        self.ids.port_dropdown.values = self.ports

        if self.ids.port_dropdown.values[0] == 'No Devices Found':
            self.spin.text = 'No Devices Found'
            self.ids.port_dropdown.values = ''

    def initialize_training(self):
        self.start_instruction = 'Press "Start Training"'
        return self.start_instruction




    # Start or finish the training process
    def start_finish_training(self):
        self.ids.reboot.disabled = False
        if self.saf.text == 'Start Training':
            self.update_log('Started Training!')
            self.saf.text = 'Finish Training'
            # Disable upload btn until finished
            self.ids.upload.disabled = True
            self.instructions.text = '''1. Activate each area 
(in alphabetical order)

2. Activate the Oracle 

3. Press "Finish Training"'''
        else:
            self.saf.text = 'Start Training'
            self.update_log('Finished Training!')
            # Enable upload button after finishing training
            self.ids.upload.disabled = False
            self.ids.start_and_finish.disabled = True
            self.instructions.text = 'Press "Upload to board"'



    def reboot(self):
        self.ids.start_and_finish.disabled = False
        self.ids.upload.disabled = True
        self.ids.start_and_finish.text = 'Start Training'
        self.ids.instructions.text = 'Press "Start Training"'
        self.update_log('reboot')
        self.ids.reboot.disabled = True



    # Toggle Log visibility
    def toggle_log(self, value):
        if not value:
            self.ids.log.height = 0
            self.ids.log.size_hint_y = None
            self.ids.log.text = ''
        else:
            self.ids.log.height = self.parent.height * 0.71
            self.ids.log.size_hint_y = None
            self.ids.log.text = 'Log:\n' + self.log



    # Upload new training data
    def upload(self):
        self.ids.reboot.disabled = True
        self.update_log('Upload starting...')
        if self.selectedPort is not None:
            self.update_log("Connecting to port {}...".format(self.selectedPort))
            try:
                self.update_log('Connected to port\n{}, uploading...'.format(self.ser.name))
                self.ser.write(b'CHANGE_MODE=1') #write data as bytes
                # self.ser.close()  # close port TODO check need
            except OSError:
                self.update_log('Error occurred')  # TODO Error handling

        self.ids.upload.disabled = True



    # Selects a given port to connect to
    def select_port(self, port):

        if str(port) == 'No Devices Found':
            return

        try:
            self.update_log("Port selected: {}".format(port))
            self.selectedPort = port
            idx = self.ports.index(port)
            self.selectedPortDescription = self.descriptions[idx]

            # Enable serial port
            self.ser = serial.Serial('/dev/cu.' + str(self.selectedPort), 57600)  # open serial port

            # Enable start button
            self.ids.start_and_finish.disabled = False
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


# Main App definition
class MyApp(App):
    title = "Game Board Training Dashboard"

    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()
