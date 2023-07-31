# Remote control robot via serial commands from rpi to arduino.
# 'w'' 'a' 'd' 's' for directional control, 'i', 'j' to increase or decrease speed
# 'q' to quit

import time
import os
import curses

import serial

# Initialize serial.
ser = serial.Serial('/dev/ttyUSB1', 19200, timeout=1)

# Initialize curses.
stdscr = curses.initscr()  # init curses screen
stdscr.keypad(True)  # enable curses keypad
curses.cbreak()  # respond immediately to keypress
curses.noecho()  # echo keypress to screen
curses.curs_set(True)  # make cursor visible

# Cursor position
cursor_y = 0
cursor_x = 0

while (True):
    char = chr(stdscr.getch())
    if char == 'q':  # quit
        # Quit curses.
        curses.nocbreak()
        stdscr.keypad(False)
        curses.endwin()
        # Close serial.
        ser.close()
        break
    elif (char == 'w' or char == 'a' or char == 's' or char == 'd' or char == 'i' or char == 'j' or char == 'r' or char == 'f'):
        ser.write(char.encode('utf-8'))
        print(f'{char}\n', end='\r', flush=True)
    else:
        print(f'{char} does nothing.\n', end='\r', flush=True)

