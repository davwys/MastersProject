<img src="https://github.com/davwys/MastersProject/raw/master/trainingDashboard/assets/uzh_logo.png" alt="UZHLogo" width="300"/>

# Masters Project Repository

Masters Project by Vincent Rüegge and David Wyss


This repository holds all code related to our Masters Project, which is part of the Post-Fossil Cities board game project.
The aim of our subproject is to create a trainable, dynamic game board that is capable of recognizing played actions (cards) via RFID/NFC and return the corresponding API calls.

## Structure
This repository holds two main software components, called:
- ESP32 Game Board: The software running on our game board's main controller (an ESP32). Written in C++.
- trainingDashboard: The dashboard software for training the game board; this software collects sensor input data during training, to which it then automatically maps the corresponding correct API calls (in progress). Written in Python, using Kivy as the UI framework.

## Work in progress...
