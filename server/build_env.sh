#!/bin/bash

python -m venv server_runtime
source server_runtime/bin/activate
pip3 install coloredlogs pyserial python-socketio eventlet