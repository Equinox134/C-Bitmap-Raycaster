# C-Bitmap-Raycaster
A C++ implementation of a raycaster using bitmap images

## Overview
This is a project of creating a raycaster similar to the one used in Wolfenstein 3D. All of the code(main.cpp, bmputil.h) and the folders(input, output, screenshots, etc) must be included in the same relative positions for the code to properly work. Please note that all explenation here assumes your using windows.

## Input
The raycaster will draw a 3D environment based on an image hat is pre-drawn.

The code is written to read bmp files inside the input folder, however this can change by modifying the code.

In line 228 of `main.cpp`, there is a variable called `inStr`. To change the input file, change `inStr` to the file path, with quotation marks. Variables `outStr` and `screenStr` each holds the file path of the output image and the location where screenshots are stored. Changing these variables changes the location of each file.

For the input image, it must be a 24-bit bitmap image and use the BITMAPINFOHEADER as the DIB header(read [this Wikipidea article](https://en.wikipedia.org/wiki/BMP_file_format#DIB_header_(bitmap_information_header)) for more detail). An easy way to meet all the requirements is to draw with MS Paint, and save as a 24-bit bitmap image. If your using a Mac, good luck.

Here are a few things to keep in mind when drawing the input image :
* The color with an rgb value of `(255,0,0)` will not be drawn. Instead, the pixel that color is in will be the initial camera location
* The border of the map will contain walls even if you dont draw them. However, they will be all white
* The color with an rgb value of `(0,0,0)` will be drawn as white in the final output
* Any size of the input image works, however it is recommended to make it at least 100 pixels * 100 pixels, as color seems to break for smaller sizes(reasons unknown)

## Output
The output of the raycaster is a 3D view of the input image. The output image constantly updates as you move around, but this won't be shown if you open the image using a seperate viewer. Instead, make the icon shown inside the file explorer as large as possible, so that you can see the image without opening it. There, the image will be updated live, although it is a little slow.

The size of the output image is adjustable, however it is recommended to use a size larger than 500 pixels * 500 pixels. If the output image is too small, the code can return an error.

## Controls
Here are all the controls available in the program. Remember to type the controls into the console. Also, type a control and wait for the image to update before typing in a new key.
* `w`, `a`, `s`, `d`: Move forward, left, backward, right, respecively
* `q`, `e`: Turn left, right, respectively
* `SHIFT`+ `q`, `e`: Turn left, right three times more
* `1`, `2`, `3`: Change view mode, more explanation below
* `c`: Enable/disabel noclip, more explanation below
* `r`: Enable/disable run, more explanation below
* `f`: Take screenshot, saved in the screenshot folder by default
* `SPACE`: Turn the camera y 180 degrees
* `/`: Enable command console, types of commands written below
* `x`: Exit program

View modes:
* 1: The default view mode. The walls have their original color
* 2: The walls have their original color, but is darker the farther you are away from that wall
* 3: The walls have a greyscale color depend on the distance to the camera, white being the closest, and black being the farthest

Noclip: By default, you can't walk through walls, however by enabling noclip you can. There will be a small blue square in the bottom-left corner of the output if noclip is enabled.

Run: You move more every step. There will be a small green square in the bottom-left corner if running is enabled.

## Commands
Here are all the commands in the program. Curly brackets{} mean to type in words, square brackets[] mean to type in numbers. Commands with different modes have the modes listed beneath them.
* `help`: prints list of controls
* `list`: prints list of commands
* `camloc {mode} [x] [y]`: changes camera location, `[x]` and `[y]` are floats
	* add: adds `[x]`, `[y]` to current cameras x and y positions
	* set: sets camera positions to `([x], [y])`
* `camrot {mode} [r]`: changes camera rotation, `[r]` is a float
	* add: adds `[r]` degrees to current camera rotation
	* set: sets camera rotation to `[r]` degrees
* `floorcl [r] [g] [b]`: change floor color to `{[r], [g], [b]}`, `[r]`~`[b]` are integers
* `skycl [r1] [g1] [b1] [r2] [g2] [b2]`: sets sky color to a gradient, color changes from {[r1], [g1], [b1]} to {[r2], [g2], [b2]}, [r1]~[b2] are integers
* `fov {mode} [f]`: changes camera FOV, `[f]` is an integer
	* add: adds `[f]` to current camera FOV
	* set: sets camera FOV to `[f]`
* `res [x] [y]`: sets output image size to `[x]` pixels by `[y]` pixels, `[x]` and `[y]` are integers

## How it Works
To be added
