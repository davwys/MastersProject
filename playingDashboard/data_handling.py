import time


# Handles an individual training message, checks its validity
# and (if valid) saves the relevant values to the trainingInput array
# Example: TRAIN={SensorID=3_CardID=52}
def handle_playing_message(self, msg):
    start = "{"
    end = "}"
    s_start = "Area='"
    s_end = "'_"
    c_start = "CardID="
    message = msg[msg.find(start) + len(start):msg.rfind(end)]
    # message example: SensorID=1_CardID=072
    if "Area='" not in message or "CardID=" not in message:
        return

    area_name = message[message.find(s_start) + len(s_start):message.rfind(s_end)]
    card_id_string = message[message.find(c_start) + len(c_start):len(message)]
    card_id = int(card_id_string)
    # Print read data to log (disabled for now)
    print("area name: {}".format(area_name))
    print("Card ID: {}".format(card_id))

    id = area_name.lower()

    txt = "User played card " + str(card_id) + " on area " + area_name
    self.ids[id].text = txt
    time.sleep(1)
    self.ids[id].text = ''


