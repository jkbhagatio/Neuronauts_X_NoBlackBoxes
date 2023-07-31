# Imports and notebook settings
from collections import deque
import time
import argparse
import os
import pathlib
from pathlib import Path

import utils
import imutils
from imutils.video import VideoStream
import numpy as np
from matplotlib import pyplot as plt
import cv2

# Set some constants
# deque buffer length (time points to track contrail)
DEQUE_BUFF_LEN = 32
deque_buffer = deque(maxlen=DEQUE_BUFF_LEN)
# minimum pixels to draw around tracked ball
MIN_PX_BALL_RAD = 10
GAUSS_FILT_PX_SZ = (5, 5)
GAUSS_FILT_SIGMA = 1
D_E_IT = 2;  # dilation/erosion iterations

# After finding HSV ranges of ball colors (use some range detector tool),
# and set a video stream pointer
# Cyan:
#hsv_lower = (95, 80, 115)
#hsv_upper = (255, 255, 255)
#vs = cv2.VideoCapture('/home/pi/swc_bootcamp_2021/media/blue_ball_move.mp4')
# Pink:
hsv_lower = (160, 160, 140)
hsv_upper = (255, 255, 255)
vs = cv2.VideoCapture('/home/pi/swc_bootcamp_2021/media/pink_ball_move.mp4')
img = cv2.imread('/home/pi/swc_bootcamp_2021/media/pink_ball.jpg')

while True:
    frame = vs.read()
    if not frame[0]:  # end of video
        break
    frame = frame[1]
    gauss_blurred_frame = cv2.GaussianBlur(frame, GAUSS_FILT_PX_SZ, GAUSS_FILT_SIGMA)
    hsv_frame = cv2.cvtColor(gauss_blurred_frame, cv2.COLOR_BGR2HSV)
    masked_frame = cv2.inRange(hsv_frame, hsv_lower, hsv_upper)
    dilated_frame = cv2.dilate(masked_frame, None, iterations=D_E_IT)
    eroded_frame = cv2.erode(dilated_frame, None, iterations=D_E_IT)
    contours = cv2.findContours(eroded_frame.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)
    center = None
    radius = 0
    # Only proceed with tracking if contours found in this frame
    if len(contours) > 0:
        c = max(contours, key=cv2.contourArea)
        ((x, y), radius) = cv2.minEnclosingCircle(c)
        center = np.round((x,y)).astype(int)
    # Draw bounding circle and contrails
    if radius > MIN_PX_BALL_RAD:
        cv2.circle(frame, (int(x), int(y)), int(radius), (0, 255, 255), 2)
        cv2.circle(frame, center, 5, (0, 0, 255), -1)
    deque_buffer.appendleft(center)
    for i in range(1, len(deque_buffer)):
        if deque_buffer[i - 1] is None or deque_buffer[i] is None:
            continue
        else:
            thickness = int(np.sqrt(DEQUE_BUFF_LEN / float(i + 1)) * 2.5)
            cv2.line(frame, deque_buffer[i - 1], deque_buffer[i], (0, 0, 255), thickness)
    cv2.imshow("Frame", frame)
    #plt.imshow(frame)

# At end of video, stop video stream, close all windows.
vs.release()
cv2.destroyAllWindows()
