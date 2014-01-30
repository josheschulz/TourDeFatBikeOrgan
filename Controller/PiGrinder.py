import sys, pygame, serial


ser = serial.Serial('/dev/ttyUSB0', 115200, xonxoff=True,timeout=.01)
pygame.init()

## Initialize sound files
FRAMES_PER_SECOND = 10
clock = pygame.time.Clock()

#Initialize the mixer
pygame.mixer.init()
drums = pygame.mixer.Sound("./sounds/drums.wav")
bass = pygame.mixer.Sound("./sounds/bass.wav")
horns = pygame.mixer.Sound("./sounds/horns.wav")

drumChannel = drums.play(-1)
bassChannel = bass.play(-1)
hornChannel = horns.play(-1)

while 1:
   ## Wait until it's time to do something
   clock.tick(FRAMES_PER_SECOND)
   for event in pygame.event.get():
      if event.type == 0x8000:
         print "Found our serial event"
         if not hasattr(event, 'volume'): continue
         drumChannel.set_volume(volume)
         print "set drum volume to " + str(volume)
   ## Events are done
   command = ser.readline() 
   if command:
      ## Parse the command 
      ## 2014-01-24 11:38:30.746120|1|2|52
      ## Date|Type|Node|Value
      comparms = command.rsplit("|")
      if len(comparms) > 0:
         ## This code could be better.
         if comparms[1] == 1:
            ## We have ourselves a setting!
            ##    These are scaled 100-0, our sound values are set 0-1
            newVolume = comparms[3] / 100
            print "New Volume: " + str(newVolume)
            ## TODO: Map node to channel
            pygame.event.post(pygame.event.Event(0x8000, channel=comparms[2],volume=newVolume))

