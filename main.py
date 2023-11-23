import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(14,GPIO.OUT)

# While loop
while True:
        # set GPIO14 pin to HIGH
        GPIO.output(14,GPIO.HIGH)
        # show message to Terminal
        print "LED is ON"
        # pause for one second
        time.sleep(1)


        # set GPIO14 pin to HIGH
        GPIO.output(14,GPIO.LOW)
        # show message to Terminal
        print "LED is OFF"
        # pause for one second
        time.sleep(1)