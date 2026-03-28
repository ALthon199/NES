# Welcome to my C NES EMULATOR

This project is mainly for me to understand how multiple hardware are connected
in a emulated environment and especially the different interactions they have. Also, it seemed really engaging to figure
out how old technologies such as the NES worked, especially with how incredibly efficient they were about managing memory
and efficiency. 

I'm planning to document the whole process as I go, so perhaps people can see how to implement an emulator yourself.
Moreover, I wanted to share my thought process for figuring out the many technologies and techniques involved in making this.

## Table of contents
* [1: Introduction to NES](#1-introduction-to-nes)

## 1: Introduction to NES

### Basic Structure
So, before starting this project. I watched [Kevin Zurwal's guide to NES Emulator](https://www.youtube.com/watch?v=F8kx56OZQhg) which was built in C++, however, I found the information to be very important and use it as a guideline for how to make my own emulator. 


To summarize what I learned is that to make an emulator, first of all, it's best to simulate how the hardware actually functions. 
For example, how is the CPU connected to the RAM which is also connected to other components (APU, PPU, etc...). To emulate this, I decided to implement a Bus which is basically the data line that connects these components. So for example, if the CPU wants to read, the CPU technically doesn't read to the RAM, but it makes a call to the BUS which then gathers that data from the RAM.


You can think of it like this, the CPU simply calls for an address to read, this can be within bounds or out of bounds, but the BUS is whats responsible for actually sending that data back. On the hardware and circuitry level, the CPU just tells what pins to go down or up, but doesn't actually read it, just recieves it.


With this mental model in mind, I decided to implement my Bus struct as being connected to the CPU and CPU being connected to the Bus. Similarly, this will apply to the other devices (APU, PPU, etc...). This is in part due to the mental model, but also physically the bus/pins are connected to the CPU and thus the CPU is also connected to the pins.

