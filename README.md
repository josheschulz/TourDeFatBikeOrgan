Tour De Fat Bike Organ
======================

The general idea is one bike is the master and it does something... plays music, lights up, breathes fire, whatever, based on where the bikes that are part of the network are in relation to it.

The first cut is going to be playing music.  I'll record some music, and split it out into tracks, as many as I Think I need (I'm guessing about 6) and whoever is riding their bike will have the remote arduinos (using moteino for the wireless bits) with them.  The further they get from the main bike the softer their track gets.

The result is changing music based on how people ride their bikes.  Once that's working I'd like to mix it up a little.

The Pieces
==========

Arduino Master - The arduino master is going to sit and collect data on how far away each channel is.  It will then update the track volume appropriately.

Arduino Slaves - The arduino slaves need to figure out how far away they are and let the master know.

Music Player - The music player needs to adjust the track volumes and play the music.  Raspberry pi attached to the master motino.  Using pygame mixer to control the audio.  It'll read off of serial.

Notes
=====

1/22/2014 - Cleaned up the output from the master in preparation for reading it from the pi

1/13/2014 - Played around with the code some today.  There's a delicate balance between responsiveness and smoothness.  
The place it's at now (average every 100ms, broadcast ever 250, take 50 samples) i get a pretty good mixture of both, but
i'm not entirely sure that it's going to work when we're moving faster.  We'll see.
