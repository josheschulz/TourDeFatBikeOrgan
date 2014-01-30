Organ Remote
============

The initial run of this code pulled 20mA.  That is very high.  Going to start trying to fix this.

https://github.com/josheschulz/TourDeFatBikeOrgan/commit/93dae63a33d55357f6f05535ff3068f32e6c03a5
This commit got it down to 8mA, but it didn't work.

https://github.com/josheschulz/TourDeFatBikeOrgan/commit/45956068543eb7bd3b6e1a7a6c2e174ae3b6394f
This works, but it's at 18mA.  It's the transmission that's killing us

Interestingly turning off the master doesn't affect the current much.  It's the transmitting that's killing us

No... Something is wrong with my measurements, because stopping all transmiting and recieving did nothing.

Turning the radio off though... drops me down to 7mA.

Making that loop nothing but an 8second powerdown sleep and usage goes to nothing (.08mA).

So 10mA to operate the radio.  7mA to run the loop.
