from kivy.app import App
from kivy.lang import Builder
from kivy.uix.screenmanager import ScreenManager, Screen
import requests
import json

class MainWindow(Screen):

    API_ENDPOINT = "http://pastebin.com/api/api_post.php"
    API_KEY = "e52f5325453fe98ad5d774d67e046505"

    source_code = '''
    test
    '''

    data = {'api_dev_key': API_KEY,
            'api_option': 'paste',
            'api_paste_code': source_code,
            'api_paste_format': 'python'
    }

    def stage(self, call):
        r = requests.post(url = self.API_ENDPOINT, data = self.data)

        pastebin_url = r.text
        print("The pastebin URL is: %s"%pastebin_url)


class SecondWindow(Screen):
    pass


class ThirdWindow(Screen):
    pass


class WindowManager(ScreenManager):
    pass


kv = Builder.load_file("my.kv")


class MyMainApp(App):

    url = 'https://masterproject-3d9aa.firebaseio.com/.json'

    def patch(self, input):
        to_database = json.loads(input)
        requests.patch(url=self.url, json=to_database)

    auth_key = 'AxTJLpOrenvKeejzLPdIhtKvsi5tXJK4Kvz56E4p'

    def get_data(self):
        request = requests.get(url=self.url + '?auth=' + self.auth_key)
        print(request.json())

    def build(self):
        return kv


if __name__ == "__main__":
    MyMainApp().run()