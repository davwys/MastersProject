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
import requests
import json

Window.clearcolor = (0.8, 0.8, 0.8, 1)


class MainWindow(Screen):
    # load external methods
    from serial_functions import get_serial_ports, read_from_port, update_ports, select_port, read_playing_input, handle_playing_message

    # Thread kill flag (kills input thread on program end)
    stopThread = False
    areaName = '[AreaName]'
    cardId = '[CardID]'
    type = '[Type]'

    currentData = ''

    # API Specification
    apiEndpoint = ""  # TODO http://pastebin.com/api/api_post.php
    apiKey = ""  # TODO provide some e52f5325453fe98ad5d774d67e046505
    callFormat = ''

    def load_current_format(self, replace):
        try:
            f = open("api_area_config.txt", "r")
            if f is not None:
                cnt = 0
                line = f.readline()
                while line:
                    line = line.strip("\n")

                    # Endpoint
                    if cnt == 0:
                        self.apiEndpoint = str(line)

                        if replace is True or len(self.ids.endpoint.text) == 0:
                            self.ids.endpoint.text = self.apiEndpoint
                    # Key
                    elif cnt == 1:
                        self.apiKey = str(line)

                        if replace is True or len( self.ids.key.text) == 0:
                            self.ids.key.text = self.apiKey
                    # Data Format
                    elif cnt == 2:
                        self.callFormat = str(line)
                        self.ids.current.text = self.callFormat

                        if replace is True or len(self.ids.input.text) == 0:
                            self.ids.input.text = self.callFormat

                    line = f.readline()
                    cnt += 1

        except FileNotFoundError:
            print('Error: no configuration found')

    # Auto-converts variables into real-time data
    def api_name_handler(self, name):
        txt = name.replace("[AreaName]", self.areaName)
        txt = txt.replace("[CardID]", self.cardId)
        txt = txt.replace("[Type]", self.type)
        return txt

    # Sends the current API call
    def send_api_call(self):

        if self.apiEndpoint is not None and self.apiKey is not None:
            api_endpoint = self.apiEndpoint

            print("Sending data: " + str(self.currentData))
            data = {'api_dev_key': self.apiKey,
                    'api_option': 'paste',
                    'api_paste_code': self.currentData,
                    'api_paste_format': 'python'
                    }

            self.stage(api_endpoint, data)

    # Stages an API call
    def stage(self, api_endpoint, data):
        r = requests.post(url=api_endpoint, data=data)

        url = r.text
        print("The URL is: %s" % url)

    # Adds a string to API call format input
    def add_to_call(self, name):
        self.ids.input.text += name

    def save_call_format(self):
        self.apiKey = self.ids.key.text
        self.apiEndpoint = self.ids.endpoint.text
        self.callFormat = self.ids.input.text

        txt = str(self.apiEndpoint + "\n" + self.apiKey + "\n" + self.callFormat)
        f = open("api_area_config.txt", "w+")
        f.write(txt)
        f.close()
        print('Saved format: ' + str(txt))
        self.load_current_format(True)

    def reset_call_format(self):
        self.load_current_format(True)
        self.ids.input.text = self.callFormat

#
#
# ORACLE
#
#

class SecondWindow(Screen):

    # load external methods
    from serial_functions import get_serial_ports, read_from_port, update_ports, select_port, read_playing_input, handle_oracle_message

    # Thread kill flag (kills input thread on program end)
    stopThread = False
    array = '[CardCombination]'
    cardCombination = '[CardCombination]'

    currentCallFormat = ''

    def load_current_format(self, replace):
        try:
            f = open("api_oracle_config.txt", "r")
            if f is not None:
                cnt = 0
                line = f.readline()
                while line:
                    line = line.strip("\n")

                    # Endpoint
                    if cnt == 0:
                        self.apiEndpoint = str(line)

                        if replace is True or len(self.ids.endpoint.text) == 0:
                            self.ids.endpoint.text = self.apiEndpoint
                    # Key
                    elif cnt == 1:
                        self.apiKey = str(line)

                        if replace is True or len( self.ids.key.text) == 0:
                            self.ids.key.text = self.apiKey
                    # Data Format
                    elif cnt == 2:
                        self.callFormat = str(line)
                        self.ids.current.text = self.callFormat

                        if replace is True or len(self.ids.input.text) == 0:
                            self.ids.input.text = self.callFormat

                    line = f.readline()
                    cnt += 1

        except FileNotFoundError:
            print('Error: no configuration found')

    # Auto-converts variables into real-time preview
    def api_name_handler(self, name):
        txt = name.replace("[CardCombination]", self.array)
        self.ids.current.text = txt
        self.callFormat = txt

    # Adds a string to API call format input
    def add_to_call(self, name):
        self.ids.input.text += name

    def save_call_format(self, text):
        f = open("api_oracle_config.txt", "w+")
        f.write(str(text))
        f.close()
        print('Saved format: ' + str(text))
        self.load_current_format(True)

    def reset_call_format(self):
        self.load_current_format(True)
        self.callFormat = self.currentCallFormat
        self.ids.input.text = self.callFormat


class WindowManager(ScreenManager):
    pass


kv = Builder.load_file("my.kv")


# Main App definition
class MyMainApp(App):
    title = "Game Board API Call Specification Dashboard"

    def build(self):
        return kv


if __name__ == "__main__":
    MyMainApp().run()
