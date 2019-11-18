import serial
import serial.tools.list_ports


def request_area_name(self):
    self.ids.area_name.disabled = False
    self.ids.area_name.text = ''
    self.ids.submit_name.disabled = False


def submit_area_name(self, name):
    if 0 < len(name) < 20:
        self.ids.area_name.disabled = True
        self.ids.area_name.text = ''
        self.ids.submit_name.disabled = True

        # Save name to temp data
        self.tempData[0] = name

        # Confirm received training data
        self.ser.write(b'TRAIN_OK')
        self.save_training_data(self.tempData)
    else:
        self.update_log('Error: Name too long')