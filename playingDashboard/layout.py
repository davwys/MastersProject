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

Window.clearcolor = (0.8, 0.8, 0.8, 1)



class MyGrid(Widget):

    from areaTopLeft import areaTL
    from areaTopRight import areaTR
    from areaBottom import areaB

# Main App definition
class MyApp(App):
    title = "Game Board Playing Dashboard"

    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()
