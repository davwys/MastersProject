import kivy
from kivy.app import App
from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty


class MyGrid(Widget):
    connection = 'Your laptop'

    def btn(self):
        print("Pressed")

    def start_finish_training(self):
        if self.saf.text == 'Start Training':
            self.saf.text = 'Finish Training'
        else:
            self.saf.text = 'Start Training'

    def upload(self):
        print("upload")


class MyApp(App):
    def build(self):
        return MyGrid()


if __name__ == "__main__":
    MyApp().run()