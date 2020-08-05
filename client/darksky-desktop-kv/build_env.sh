#!/bin/bash

# If rebuild is set, then delete the existing runtime
if [ "$1" = "rebuild" ]; then
    echo " ==> Destroying current runtime"
    rm -rf desktop_runtime
fi

# Generate a new venv if required
if [ ! -d "desktop_runtime" ]; then
    echo " ==> Creating new venv"
    python3 -m venv desktop_runtime
fi

# Enter the new python env
echo " ==> Activating venv"
source desktop_runtime/bin/activate

# Required modules
echo " ==> Installing required pip3 modules"
pip3 install coloredlogs python-socketio eventlet pyrtlsdr astropy kivy==2.0.0rc3

# Dev tools
echo " ==> Installing dev pip3 modules"
pip3 install autopep8

cp *.py desktop_runtime
pip3 freeze > requirements.txt
