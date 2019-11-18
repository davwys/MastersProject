# Toggle Log visibility
def toggle_log(self, value):
    if not value:
        self.ids.log.height = 0
        self.ids.log.size_hint_y = None
        self.ids.log.text = ''
    else:
        self.ids.log.height = self.parent.height * 0.72
        self.ids.log.size_hint_y = None
        self.ids.log.text = 'Log:\n' + self.log


# Update the integrated log
def update_log(self, text):
    print(text)
    self.log += (text + '\n')

    # Update Log text only if visible
    if self.ids.chk.active:
        self.ids.log.text = 'Log:\n' + self.log
