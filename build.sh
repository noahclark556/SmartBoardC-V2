#!/bin/bash

# Compile the application
gcc -o sb_app $(find . -name "*.c") $(sdl2-config --cflags --libs) -lSDL2_ttf -lSDL2_gfx -lcurl -lcjson -lSDL2_image -v

# Check if compilation was successful
if [ $? -eq 0 ]; then
    # Run the application
    ./sb_app
else
    echo "Compilation failed."
fi