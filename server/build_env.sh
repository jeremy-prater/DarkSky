#!/bin/bash

rm -rf server_runtime
python -m venv server_runtime
source server_runtime/bin/activate

pip3 install coloredlogs pyserial python-socketio eventlet
cp *.py server_runtime
pip3 freeze > requirements.txt

if [ $1 == "generate" ]; then
    rm -rf /work/yoctoland/meta-darksky/recipes-darksky/darksky-server-python/*
    pipoe --python python3 -r requirements.txt  -o /work/yoctoland/meta-darksky/recipes-darksky/darksky-server-python
fi