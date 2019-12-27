import kivy
from kivy.app import App
from kivy.lang import Builder
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.config import Config
Config.set('graphics', 'resizable', 1)
Config.set('graphics', 'width', '1000')
Config.set('graphics', 'height', '750')
Config.set('graphics', 'minimum_width', '1000')
Config.set('graphics', 'minimum_height', '750')
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


class MainWindow(Screen):
    # load external methods
    from serial_functions import get_serial_ports, read_from_port, update_ports, select_port, read_playing_input, handle_playing_message

    # Thread kill flag (kills input thread on program end)
    stopThread = False
    areaName = '[AreaName]'
    cardId = '[CardID]'
    callFormat = ''

    currentCallFormat = ''

    def load_current_format(self):
        f = open("api_config.txt", "r")
        if f is not None:
            self.currentCallFormat = f.read()
            self.ids.current.text = self.currentCallFormat

    # Auto-converts variables into real-time preview
    def api_name_handler(self, name):
        txt = name.replace("[AreaName]", self.areaName)
        txt = txt.replace("[CardID]", self.cardId)
        self.ids.preview.text = txt
        self.callFormat = txt

    # Adds a string to API call format input
    def add_to_call(self, name):
        self.ids.input.text += name

    def save_call_format(self, text):
        f = open("api_config.txt", "w+")
        f.write(str(text))
        f.close()
        print('Saved format: ' + str(text))
        self.load_current_format()

    def reset_call_format(self):
        self.load_current_format()
        self.callFormat = self.currentCallFormat
        self.ids.input.text = self.callFormat


class SecondWindow(Screen):
    array = '[Array]'

    # load external methods
    from serial_functions import get_serial_ports, read_from_port, update_ports, select_port, read_playing_input, \
        handle_playing_message

    # Thread kill flag (kills input thread on program end)
    stopThread = False
    areaName = '[AreaName]'
    cardId = '[CardID]'
    callFormat = ''

    currentCallFormat = ''

    def load_current_format(self):
        f = open("api_config.txt", "r")
        if f is not None:
            self.currentCallFormat = f.read()
            self.ids.current.text = self.currentCallFormat

    # Auto-converts variables into real-time preview
    def api_name_handler(self, name):
        txt = name.replace("[AreaName]", self.areaName)
        txt = txt.replace("[CardID]", self.cardId)
        self.ids.preview.text = txt
        self.callFormat = txt

    # Adds a string to API call format input
    def add_to_call(self, name):
        self.ids.input.text += name

    def save_call_format(self, text):
        f = open("api_config.txt", "w+")
        f.write(str(text))
        f.close()
        print('Saved format: ' + str(text))
        self.load_current_format()

    def reset_call_format(self):
        self.load_current_format()
        self.callFormat = self.currentCallFormat
        self.ids.input.text = self.callFormat


class WindowManager(ScreenManager):
    pass


kv = Builder.load_file("my.kv")


# Main App definition
class MyMainApp(App):
    title = "Game Board API Call Specification Dashboard"

    def build(self):
        #kv = MainWindow()
        #kv.load_current_format()
        return kv


if __name__ == "__main__":
    MyMainApp().run()
