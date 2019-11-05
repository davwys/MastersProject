import kivy
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.spinner import Spinner
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty


class MyGrid(Widget):
    connection = 'Your laptop'
    ports = ['COM1', 'COM2', 'COM3']

    def btn(self):
        print("Pressed")

    # Update the list of available ports
    def update_ports(self):
        print('update ports...')
        self.ids.port_dropdown.values = self.ports

    def start_finish_training(self):
        if self.saf.text == 'Start Training':
            self.saf.text = 'Finish Training'
        else:
            self.saf.text = 'Start Training'

    def upload(self):
        print("upload")

    def select_port(self, port):
        print("Port selected: " + str(port))


# Main App definition
class MyApp(App):
    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()