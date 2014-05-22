#!/usr/bin/env python

import serial,time,sys,os,datetime,multiprocessing

#initialization and open the port
#possible timeout values:
#    1. None: wait forever, block call
#    2. 0: non-blocking mode, return immediately
#    3. x, x is bigger than 0, float allowed, timeout block call
ser = serial.Serial()
ser.bytesize = serial.EIGHTBITS     #number of bits per bytes
ser.parity = serial.PARITY_NONE     #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE  #number of stop bits
ser.timeout = None                  #block read
#ser.timeout = 1                    #non-block read
#ser.timeout = 2                    #timeout block read

ser.xonxoff = False     #disable software flow control
ser.rtscts = False      #disable hardware (RTS/CTS) flow control
ser.dsrdtr = False      #disable hardware (DSR/DTR) flow control
ser.writeTimeout = 2    #timeout for write


#ser.baudrate = 230400
ser.baudrate = 460800
locations=['/dev/ttyUSB0',
    '/dev/ttyUSB1',
    '/dev/ttyUSB2',
    '/dev/ttyUSB3',
    '/dev/ttyUSB4' ]

for device in locations:
    try:
        ser.port = device
        ser.open()
        break
    except:
        pass

log_f='/tmp/serial.log'
def serial_read():
    while True:
        string = ser.readline()
        string = string.rstrip('\r\n')
        print string
        sys.stdout.flush()

def serial_write():
    cmdlist=["date","gettmpr","hello","help","led 1 on","led 1 off","clear"]
    while True:
        for x in cmdlist:
            time.sleep(0.05)
            # print date to log file
            now=datetime.datetime.now()
            print '\r\n' + \
                now.strftime('%Y-%m-%d %H:%M:%S') + ' to run command: ' + x
            sys.stdout.flush()
            ser.write(x+"\r");
            


if ser.isOpen():

    try:
        ser.flushInput()        #flush input buffer, discarding all its contents
        ser.flushOutput()       #flush output buffer, aborting current output 
                                #and discard all that is in buffer
        
#        # redirect stdout to a file
#        sys.stdout = open(log_f, 'w')

        s_r = multiprocessing.Process(name='serial_read',
                                           target=serial_read,
                                            args="")
        s_r.daemon=True
        s_r.start()

        s_w = multiprocessing.Process(name='serial_write',
                                           target=serial_write,
                                            args="")
        s_w.daemon=True
        s_w.start()

        s_r.join()
        s_w.join()

        ser.close()

    except Exception, e1:
        print "error communicating...: " + str(e1)
else:
    print "cannot open serial port "
