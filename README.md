# Mandelbrot Project

I wrote this for my Advanced Programming class
It uses C++ and OpenGL shaders to perform rendering of mandelbrot and julia sets 

## Building 

Dependencies include GLEW, GLUT, and OpenGL. Honestly I don't remember everything I did to get the dependencies installed, but it was a pain. It definitely works on ubuntu though. It used to work on windows, so if you look back in my git revision history or something, you might the correct CMakeLists. But I distinctly remember that installing the dependencies was really hard. 
If you're on mac... god help you. 

After that it's your normal cmake process of building
```bash
mkdir build && cd build
cmake ..
make 
```
Then get upset when it fails, fix a dependency, and try again. 

Before embarking...keep in mind that openGL doesn't cooperate with VMs.

## Using

Here are some handy-dandy keybindings that I built in

ESC - Exit the program

W/A/S/D - Move around 

-/+ - Change the number of iterations used before the program decides that an expression diverges or not

M/J - Change between mandelbrot and julia sets

0-9 - Change the exponent between 2 and 11 (where the 1 key is 2 and the 0 key is 11... it's just the way the keys are laid out on the keyboard... try swiping your finger across all the numbers, that's fun)

[/] - Change the angle of the constant, c, in the julia expression (c = magnitude\*cos(angle)+i\*magnitude\*sin(angle))

;/' - Semicolon and apostraphe (I know, super intuitive controls, Paul) change the magnitude of the constant, c, in the julia expression (c = magnitude\*cos(angle)+i\*magnitude\*sin(angle))

Scrolling the mouse in and out changes the zoom level. Clicking the mouse is supposed to move you around the image, but it doesn't work very well right now, so if anybody actually goes through the effort of fixing that, they should hit me with a merge request. 
