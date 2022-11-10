TODO: 
- take a better main picture of the matrix (without the "input device")?
- explaining some of the code (or nah cuz too long?)
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

(1) The PNPs needed to be controlled by an additional NPN BJT because the operating voltage of shift registers was 3.3 V, to make it usable with Pico, but the voltage on the emitte pin of the PNPs was 5 V. The 3.3 V on the output of the registers would not be high enough to turn the PNP off, so the PNPs would always be on. See [here](/docs/aux/explanation_NPN_PNP.pdf) for how a PNP can be controlled with NPN.

## SOFTWARE SIDE
- The 74HC595.h/.c library is resposible for functions that are used for writing specific (8 bit) words to the cascaded registers. There ain't anything interesting really.
- The board.h/.c lib has some functions and typedefs for easier writing data to the matrix; again, nothing really that interesting.
- The menu.h/.c lib is used for displaying "options" (subprojects) on the matrix (see pictures / videos), waits for input and returns the number describing what options was chosen.
- The utilities.h/.c is lib for getting pseudo-random numbers. Since we can't get the current time on Pico, I needed a different way to get a different seed each time the init_rand() function was called. So here's what I thought of: I'll write some data to the flash storage of the Pico, and each time I'll read that data I'll change it somewhat. That way this data can act as a seed, that will be different each time the init_rand() function is called. The specifics of how it works can be seen in code, it isn't complicated to understand. Now for the pseudorandom function rand_uint16() or rand_double(): basically how it works is something like this: we take the current "seed", modify it with some large numbers, save its new value and that's how we get a new pseudo random number (see code for better explanation).
- The snake.h/.c is a lib with all of the code with logic needed for the snake game. The snake itself is implemented as a linked list. The logic of the game is quite simple, everytime a callback of a timer (responsible for timing if the game needs to be "updated" - new frame generated) is called the game is updated and the snake moved. If the snake eats a food it grows; if it eats itself or hits a wall - Game Over. The input of the "input device" is handled by IRQs.
- The pong.h/.c TODO
