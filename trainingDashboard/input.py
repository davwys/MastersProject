import serial
import serial.tools.list_ports
import re
import platform


# Returns a multiline String containing all area names and updates the displayed list
def get_area_names(self):
    if len(self.ids.areas.text) > 0:
        result = ""
        for element in self.trainingInput:
            type = "regular" if element[3] else "combinatorial"
            result += (element[0] + "\n(" + type + ", S" + str(element[1]) + ")\n")

        self.ids.areas.text = result
        if len(result) < 1:
            return "No areas activated yet"
        else:
            return result
    else:
        return ''


# Enables user input for an area name after a card has been played
def request_area_name(self):
    self.ids.area_name.disabled = False
    self.ids.area_name.text = ''
    self.ids.submit_regular.disabled = False
    self.ids.submit_combinatorial.disabled = False
    if platform.system() is not "Windows":
        self.ids.area_name.focus = True
    self.ids.area_name.hint_text = 'Enter your area name here'


# Validates a given area name
def validate_area_name(name):
    res = True
    for c in name:
        if not re.match('^[a-zA-Z0-9]', c):
            res = False

    return res


# Saves the area name given by the user (if length is within a valid range)
# and sends "TRAIN_OK" message to request new training data
def submit_area_name(self, name, regular):
    if 0 < len(name) < 20 and validate_area_name(name):
        self.ids.area_name.hint_text = ''
        self.ids.area_name.disabled = True
        self.ids.area_name.text = ''
        self.ids.submit_regular.disabled = True
        self.ids.submit_combinatorial.disabled = True

        # Save name to temp data
        self.tempData[0] = name
        # Save type to temp data
        self.tempData[3] = regular

        # Confirm received training data
        self.ser.write(b'TRAIN_OK')
        self.save_training_data(self.tempData)
        # Enable upload after we have at least one row of data
        self.ids.upload.disabled = False
        self.update_log('Area {} has been activated'.format(name))
        self.ids.undo.disabled = False
        self.get_area_names()
    elif not validate_area_name(name):
        self.update_log('Invalid area name')
        self.ids.submit_regular.disabled = False
        self.ids.submit_combinatorial.disabled = False

    else:
        self.update_log('Error: Name too long or too short')
        self.ids.submit_regular.disabled = False
        self.ids.submit_combinatorial.disabled = False


def toggle_areas(self, value):
    if not value:
        self.ids.areas.height = 0
        self.ids.areas.size_hint_y = None
        self.ids.areas.text = ''
    else:
        self.ids.areas.height = self.parent.height * 0.72
        self.ids.areas.size_hint_y = None
        self.ids.areas.text = 'Loading Areas...'
        self.ids.areas.text = self.get_area_names()
