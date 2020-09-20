from __future__ import print_function
from imutils.object_detection import non_max_suppression
from imutils import paths
import numpy as np
import argparse
import imutils
import cv2
from firebase import firebase

'''Utilizes OpenCV pre-trained HOG + Linear SVM'''

#firebase authentication (To Be Completed)
fb = firebase.FirebaseApplication("https://hackmit-df9ea.firebaseio.com/", None)

# construct argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--images", required=True, help="path to images directory")
args = vars(ap.parse_args())

# initialize the HOG descriptor/person detector
hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

# gstreamer for jetson nano, allows access to camera
GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width=3280, height=2464, format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width=960, height=616, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'

'''Function that takes video footage and detects people'''
def detectPeople():
    #video capturing class from OpenCV
    video_capture = cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)

    #open the camera
    if video_capture.isOpened():
        cv2.namedWindow("Face Detection Window", cv2.WINDOW_AUTOSIZE)

        #Video Footage that runs until ESC key is pressed
        while True:
            return_key, image = video_capture.read()
            if not return_key:
                break

            # resize it to (1) reduce detection time
            # and (2) improve detection accuracy
            image = cv2.imread(imagePath)
            image = imutils.resize(image, width=min(400, image.shape[1]))
            orig = image.copy()
            # detect people in the image
            (rects, weights) = hog.detectMultiScale(image, winStride=(4, 4),
                padding=(8, 8), scale=1.05)
            # draw the original bounding boxes
            for (x, y, w, h) in rects:
                cv2.rectangle(orig, (x, y), (x + w, y + h), (0, 0, 255), 2)
            # apply non-maxima suppression to the bounding boxes using a
            # fairly large overlap threshold to try to maintain overlapping
            # boxes that are still people
            rects = np.array([[x, y, x + w, y + h] for (x, y, w, h) in rects])
            pick = non_max_suppression(rects, probs=None, overlapThresh=0.65)
            # draw the final bounding boxes

            '''The number of people detected in the frame'''
            people = len(pick)

            for (xA, yA, xB, yB) in pick:
                cv2.rectangle(image, (xA, yA), (xB, yB), (0, 255, 0), 2)
            # show some information on the number of bounding boxes
            filename = imagePath[imagePath.rfind("/") + 1:]
            #print("[INFO] {}: {} original boxes, {} after suppression".format(
                #filename, len(rects), len(pick)))
            # show the output images
            #cv2.imshow("Before NMS", orig)
            #cv2.imshow("After NMS", image)
            key = cv2.waitKey(30) & 0xff
            if key == 27:
                break

            '''Send data to firebase'''
            fb.put('/https://hackmit-df9ea.firebaseio.com/people-trapped', 'people', people)
        video_capture.release()
        cv2.destroyAllWindows()
    else:
        print("Cannot open camera")

if __name__ == "__main__":
    detectPeople()


