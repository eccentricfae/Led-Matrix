TODO: 
- edit the mani picture of the matrix to look nicer?
- take a better main picture of the matrix (without the "input device")?
- more intricate introduction?
- ideas behind the design od the matrix? (in INTRODUCTION or separate heading?)
- explaining some of the code (or nah cuz too long?)
-table of contents if the page here gets too long
# LED-MATRIX
## SUMMARY
Hi all! This repo is a project with which I wanted to test my knowledge of digital circuit design, following the end of one of my college classes. 
The "digital circuit design" part of this project is a design of 16x16 LEDs matrix, controlled by PNPs and NPNs BJTs, which in turn are controlled by shift registers, and lastly - RPI Pico.
The "software" side of the project are subprojects intended to be displayed on the built matrix:(very) simple animation and two basic games: "snake" and "pong".
### SEE [pics-and-videos](/docs/pics-and-videos) for pictures and videos of the project :smile:
![TEST](/docs/pics-and-videos/led_matrix.jpg)

## HARDWARE SIDE
The main part of the hardware-side of the project is the LED matrix. The matrix is a 16x16 matrix of green LED's. The way this matrix works is quite simple: the circuits light up selected LEDs in one row at a time, but since they are being lit up at a fast pace, this gives the illusion of all of the rows being lit up at the same time. Based on the software written, one "frame" (singular iteration of lighting up all rows of the matrix) takes about 16ms, which results in a 60Hz refresh rate, not that it matters really (re: my code). 

The cathodes of all 16 LEDs in one column are connected, and it us through the cathodes that is decided which LED will be lit up and which not. Each column is connected and controlled by a NPN BJT. The whole array of 16 NPNs needed to control the matrix are being controlled by two (cascaded) shift registers connected to Pico.

The anodes of the LEDs are connected in rows of 8 to PNP BJTs, and two PNP's (two PNPs that toghether make up a single 16 LED row) base pins are connected toghether. That one connection is what controls which row is being fed the power, meaning: which row is being allowed to light up. The connection of the two base pins of aforementioned PNPs are connected to NPN controlling them (see note (1) below), and the NPNs, controlling the PNPs (controlling rows of LEDs), are being controlled by two (cascaded) shift registers connected to RPI Pico. 

(1) The PNPs needed to be controlled by an additional NPN BJT because the operating voltage of shift registers was 3.3 V, to make it usable with Pico, but the voltage on the emitte pin of the PNPs was 5 V. The 3.3 V on the output of the registers would not be high enough to turn the PNP off, so the PNPs would always be on. See [here](/docs/aux/) for how a PNP can be controlled with NPN.

## SOFTWARE SIDE
//
