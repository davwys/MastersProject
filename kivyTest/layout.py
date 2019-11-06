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

    connected = False
    ports = ['No Devices Found']
    descriptions = ['-']
    selectedPort = None
    selectedPortDescription = None

    # Update the list of available ports
    def update_ports(self):
        info = serial_ports()
        self.ports = info[0]
        self.descriptions = info[1]
        self.ids.port_dropdown.values = self.ports

    def start_finish_training(self):
        if self.saf.text == 'Start Training':
            self.saf.text = 'Finish Training'
            # Disable upload btn until finished
            self.ids.upload.disabled = True

        else:
            self.saf.text = 'Start Training'
            # Enable upload button after finishing training
            self.ids.upload.disabled = False

    def upload(self):
        if self.selectedPort is not None:
            print("Connecting to port {}...".format(self.selectedPort))

            try:
                ser = serial.Serial('/dev/cu.' + str(self.selectedPort), 57600)  # open serial port
                print('Connected to port {}, uploading...'.format(ser.name))  # check which port was really used
                ser.write(b'test')  # write a string write(b'test)
                ser.close()  # close port
            except OSError:
                print('Error occurred')  # TODO Error handling

        else:
            print("can't upload")  # TODO this should never happen anyways

    # Selects a given port to connect to
    def select_port(self, port):
        if str(port) == 'No Devices Found':
            return
        print("Port selected: " + str(port))
        self.selectedPort = port
        idx = self.ports.index(port)
        self.selectedPortDescription = self.descriptions[idx]
        # enable start button
        self.ids.start_and_finish.disabled = False

# Main App definition
class MyApp(App):
    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()
