import cv2
import numpy as np
from firebase import firebase

#firebase authentication (To Be Completed)
fb = firebase.FirebaseApplication("https://hackmit-df9ea.firebaseio.com/", None)

HAAR_CASCADE_XML_FILE_FACE = "./haarcascade_frontalface_default.xml"
FIRE_MODEL = ''
GSTREAMER_PIPELINE = 'nvarguscamerasrc ! video/x-raw(memory:NVMM), width=3280, height=2464, format=(string)NV12, framerate=21/1 ! nvvidconv flip-method=0 ! video/x-raw, width=960, height=616, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink'

'''Jetson Nano Code detects if face is present'''
def faceDetect():
    # Obtain face detection Haar cascade XML files from OpenCV
    face_cascade = cv2.CascadeClassifier(HAAR_CASCADE_XML_FILE_FACE)

    #determine if fire is detected
    person_detected = False

    #determines the number of people present
    people_count = 0
    fire_present = False

    # Video Capturing class from OpenCV
    video_capture = cv2.VideoCapture(GSTREAMER_PIPELINE, cv2.CAP_GSTREAMER)
    if video_capture.isOpened():
        cv2.namedWindow("Face Detection Window", cv2.WINDOW_AUTOSIZE)

        while True:
            #initializes key that was pressed as well as frame captured from video footage
            return_key, image = video_capture.read()
            if not return_key:
                break

            #convert the captured image to grayscale
            grayscale_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            '''detected_faces = face_cascade.detectMultiScale(grayscale_image, 1.3, 5)

            for (x_pos, y_pos, width, height) in detected_faces:
                cv2.rectangle(image, (x_pos, y_pos), (x_pos + width, y_pos + height), (0, 0, 0), 2)'''
            
            #gets the number of people
            people_count = len(face_cascade.detectMultiScale(grayscale_image, 1.3, 5))

            #determines if fire present
            ###Add code here###

            #shows the resulting image
            cv2.imshow("Face Detection Window", image)

            #checks if exit key was pressed
            key = cv2.waitKey(30) & 0xff
            # Stop the program on the ESC key
            if key == 27:
                break

        video_capture.release()
        cv2.destroyAllWindows()
    else:
        print("Cannot open Camera")

if __name__ == "__main__":
    faceDetect()


