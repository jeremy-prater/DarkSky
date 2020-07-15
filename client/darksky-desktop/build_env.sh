#!/bin/bash

rm -rf desktop_runtime
python3 -m venv desktop_runtime
source desktop_runtime/bin/activate

# Required modules
pip3 install coloredlogs python-socketio eventlet pyrtlsdr astropy PyQt5

# Dev tools
pip3 install autopep8

cp *.py desktop_runtime
pip3 freeze > requirements.txt
