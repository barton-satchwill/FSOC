FSOC
====

Free-Space Optical Communication

During the fall of 2013, NASA's [LADEE][LLCD] robotic spacecraft demonstrated the use of lasers for high-speed, 2-way communication between the Earth and the Moon.  

Light has been used to transmit information for centuries.  Today, laser light transmits data through glass fibres that form the vast fibre-optic networks that underpin our modern telecommunication world.

In a sense, all that NASA did was to deploy a fibre optic network between the Earth and the Moon, but without the fibre.  They simply sent the laser light [through free space][FSOC].

If NASA can do it, why can't I?

## The Hardware
I used two Arduino Uno's for this project, with very simple circuits to drive a [36¢ laser][laser] as the transmitter and the phototransistor as a receiver.  For most of the development and testing, a bright LED replaced the laser, and allowed the two development boards to be conveniently placed within a few inches of each other.

![alt text][hardware]

## The Software
The software implements --in a *very* rudimentary and incomplete way-- layers 1, 2 and 4 of the OSI network communications protocol stack.  Let's face it: there's no need for layer 3, there are only two nodes in our setup.  The code is small and fast, not much more that 4 kilobytes.  Both the receiving and transmitting rates are governed by the 16-bit Arduino timer1, with a single bit being transmitted each timer cycle.  As it didn't make any sense to drive the timer faster than the rest of the system could send and receive a signal, I instrumented the code to measure how long it took to send or receive a single bit.  

| limiting factor               | Time per Bit | Bits per Second  |
|:------------------------------|-------------:|-----------------:|
| Arduino Uno 16 mHz processor  |  0.0625 μsec |       16,000,000 | 
| transmission code             |       8 μsec |          125,000 |
| receiver code                 |    13.5 μsec |           74,000 |
| laser response time           |   not a clue |       not a clue |
| phototransistor response time |  ~ 5-50 μsec | 200,000 - 20,000 |

The phototransistor and laser introduce a *lot* of uncertainty.  The table reports typical response time I gleaned from a bit of reading.  I tried to nail down exactly what the response time of *my* phototransistor was, but was never able to achieve convincing results.  Phototransistors are sensitive to saturation, and a pulse of light that is too bright can actually lengthen the response time.  Test results varied enormously depending on the position and intensity of the laser.


## Stability
Early iterations of the code were not stable, and suffered from bit slip as the clocks on the transmitter and receiver drifted apart.  This was addressed by periodically re-synchronising the receiver, and by taking multiple samples from the phototransistor each cycle.  Thus, the actual time to read a single bit is better expressed as 13.5+(4.5*n*) μsec for *n* samples.

Timer frequency, number of samples per bit, and re-synchronisation period became tuning parameters for the system.  

## Results
Ultimately, I was able to achieve a remarkably stable system transmitting data at a whopping 15 kilobits per second, beating the [14,400][14400] modems of the mid 90's .

## Next Steps
The next obvious evolution for this project is to use orbiting mirrors to send a signal from the Earth to my secret base on the far side of the Moon.


[FSOC]: https://en.wikipedia.org/wiki/Free-space_optical_communication
[LLCD]: http://www.nasa.gov/press/2013/october/nasa-laser-communication-system-sets-record-with-data-transmissions-to-and-from/#.VnW4tl6tg_t
[14400]: https://en.wikipedia.org/wiki/Modem#Echo_cancellation.2C_9600_and_14.2C400
[laser]:http://www.dx.com/p/12mm-5mw-red-laser-diode-modules-black-dc-4-5v-165078#.VoMDol6tg_s
[hardware]: images/arduino-fsoc.jpg "The Hardware.  Transmitter on left, receiver on right"