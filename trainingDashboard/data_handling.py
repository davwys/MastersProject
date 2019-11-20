from log import update_log


# Handles an individual training message, checks its validity
# and (if valid) saves the relevant values to the trainingInput array
# Example: TRAIN={SensorID=3_CardID=52}
def handle_training_message(self, msg):
    start = "{"
    end = "}"
    s_start = "SensorID="
    s_end = "_"
    c_start = "CardID="
    message = msg[msg.find(start) + len(start):msg.rfind(end)]
    # message example: SensorID=1_CardID=072
    if "SensorID=" not in message or "CardID=" not in message:
        return

    sensor_id_string = message[message.find(s_start) + len(s_start):message.rfind(s_end)]
    card_id_string = message[message.find(c_start) + len(c_start):len(message)]
    sensor_id = int(sensor_id_string)
    card_id = int(card_id_string)
    # Print read data to log (disabled for now)
    # self.update_log("Sensor ID: {}".format(sensor_id))
    # self.update_log("Card ID: {}".format(card_id))

    # Validate both IDs and save to temporary storage
    if sensor_id > 0 and 0 < card_id < 999:
        self.tempData = [None, sensor_id, card_id]
        self.request_area_name()


# Saves an input array with name, sensor ID and card ID to the training input array
# Example input: ['AreaA', 3, 123]
def save_training_data(self, data):
    # Append to array
    self.trainingInput.append(data)

    # Print current training data state to log
    self.update_log("Current training data:")
    self.update_log(str(self.trainingInput))
