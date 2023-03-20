#!/bin/bash

# Get the name of the current project directory
CURRENT_DIRECTORY="${PWD##*/}"

# Change the name of the ino file to match the directory name
mv esp8266-mqtt-boilerplate.ino "$CURRENT_DIRECTORY".ino

# Commit the change
git add "$CURRENT_DIRECTORY".ino
git commit -m "initial commit"
