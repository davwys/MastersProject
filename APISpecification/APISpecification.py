import kivy
from kivy.config import Config
Config.set('graphics', 'resizable', 1)
Config.set('graphics', 'width', '1000')
Config.set('graphics', 'height', '750')
Config.set('graphics', 'minimum_width', '1000')
Config.set('graphics', 'minimum_height', '750')
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
import threading
import time

Window.clearcolor = (0.8, 0.8, 0.8, 1)


class MyGrid(Widget):

    # load external methods
    #from serial_functions import get_serial_ports, read_from_port, update_ports, select_port, read_playing_input
    #from data_handling import handle_playing_message

    # Thread kill flag (kills input thread on program end)
    stopThread = False

    #TODO make it work
    def api_name_handler(self, input):
        self.ids.display.text = input


# Main App definition
class MyApp(App):
    title = "Game Board API Call Specification Dashboard"

    def build(self):
        return MyGrid()

    def process(self):
        text = self.root.ids.input.text
        print(text)


if __name__ == "__main__":
    MyApp().run()
