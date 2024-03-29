# nbody
## Abstract

nbody is a simple gravitational n-body code written in C, parallelized with OpenMP and OpenCL
for shared-memory systems and GPUs. The program utilizes the SDL2 library for graphical output, 
making it a handy tool to run interactive n-body simulations for both scientific and 
educational purposes.

Current release: 0.5.1 ALPHA

## Changelog

04.01.2017:   	
- nbody 0.1 ALPHA released

18.01.2017:
- VSYNC switch added
- Integrator switch added
- Object icon (Sun, Mercury, Earth, Jupiter) can be specified
- Object icon size can be customized in the init_bodies procedure

20.01.2017:
- RKN5 implemented (stability tests pending)

22.01.2017:
- High CPU load in pause mode bug fixed

23.01.2017
- Numerical stats output implemented 
- Code cleanups

24.01.2017:
- RKN5 stability tests completed
- RKN5 performance optimized
- Right-click object info box deactivated (buggy)
- More rigerous truncation error calculation method (minimum fe)
- Timestep smoothing option added

31.01.2017:
- Minor code cleanups
- Interactive mode switch implemented
- Center-of-mass focus added

01.02.2017:
- Minor RKN5 optimizations
- RKN5 parallelized
- Number of threads can be specified
- Code cleanups 
- examples.c added

06.02.2017:
- New example setups added

09.02.2017:
- Barnes-Hut tree algorithm implementation started

11.02.2017:
- Benchmark mode added (calls a pre-defined standard setup)
- RKN6 implemented and parallelized

28.01.2018:
- Live view rotation implemented
- 3D rendering mode added

30.01.2018:
- RKN7 and RKN8 implemented (tests pending)
- Settings and object configuration files added
- New sprites added
- Code and working directory cleanups  

28.03.2018:
- RKN9 implemented
- New symplectic integrators: Velocity Verlet (2), Forest Ruthe (3), Position-Extended Forest Ruthe (4)
- GPU support via OpenCL for Velocity Verlet and Euler integrators
- Bugfixes and cleanups in 3D rendering routine
- Auto scale on startup implemented
- Various default setups accessible via settings.dat
- VSOP87 Series E solar system ephemeris implemented (setup 6)

07.03.2022:
- Conversion to a VS Code project
- Massive code refactorings and cleanups

## How to get started
Download the repository zip, unpack the archive, open a terminal and type "make".
The GNU C compiler gcc, SDL2, SDL2_ttf are required.
If you're using APT, just make sure that libsdl2-dev and libsdl2-ttf-dev are present.
Even if you don't want to make use of a GPU integrator, OpenCL librariers must be present (in APT: opencl-c-headers, nvidia-opencl-dev).

## A local SDL2 installation
In case you don't have root priviliges on your machine, the SDL2 library can be installed locally to your home directory.

1. Download the SDL2 source code from https://www.libsdl.org/release/SDL2-2.0.7.zip
2. Unzip it in your home directory
3. Open a console and change to the SDL2-2.0.7 directory
4. Run the configure script: ./configure --prefix=$HOME
5. Compile it: make
6. Install it: make install

Repeat the whole procedere for the SDL2_ttf library:

1. Download the SDL2_ttf source code from
https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.zip
2. Unzip it in your home directory
3. Open a console and change to the newly created directory SDL2_ttf-2.0.14
4. Run the configure script: ./configure --prefix=$HOME
5. Compile it: make
6. Install it: make install

You'll find three new folders in your home directory (bin, lib, share) which shouldn't mess up too much.
As a final step, you should add the new library directories to your .bashrc:

1. Open a console in your home directory
2. Type: gedit .bashrc
3. Append the following lines:

export LD_LIBRARY_PATH=$HOME/lib
export PATH="$HOME/bin:$PATH"

4. Save the file, close gedit and close the terminal.


## Setting up a simulation
Simulation settings are specified in the settings.dat file. The most straight-forward approach to run a simulation is changing line 3 
to initialize one of the hard-coded example setups:
1:	Asteroid belt and Jovian planet orbiting the sun
2:	Asteroids orbiting the sun
3:	A randomly-generated planetary system
4:	Filament of stars
5:	Sphere of stars
6:	Current solar system as given by VSOP87 filled up with asteroids between 3 and 5 au if n_bodies>9

Custom setups may be run via option 0. You are then required to provide a file named "input.dat" containing
kinematic and positional data of the system you want to simulate. Two example setups can be found in the "setups" folder.
The first three lines are reserved for the mass unit, spatial unit and the velocity unit. 
Supported physical units (case sensitive) are msun, mjup, pc, au, yrs, Myrs, kms (km per sec.) and ms (m per sec.).
The fourth line is reserved for the column headers.
After that, the first column contains the object mass, followed by the x, y and z coordinates, again followed by the vx, vy and vz
velocity components. The eigth column is a sprite index which can be one of the following:
0:	Sun
1:	Mercury
2:	Venus
3:	Earth
4:	Mars
5:	Jupiter
6:	Saturn
6:	Uranus
8:	Neptune
9:	Pluto
Sprite size (in pixels) can be defined in the last column.

In CPU mode, a total of nine different integrators are available:
2:	2nd order Velocity-Verlet
3:	3rd order Forest-Ruthe
4:	4th order Position-Extended Forest Ruthe
5:	5th order Adaptive Runge-Kutta-Nyström
6:	6th order Adaptive Runge-Kutta-Nyström
7:	7th order Adaptive Runge-Kutta-Nyström
8:	8th order Adaptive Runge-Kutta-Nyström
9:	9th order Adaptive Runge-Kutta-Nyström
Integrators 2, 3 and 4 are symplectic but require you to set a proper timestep in line 7 of settings.dat.
Integrators 5 to 9 are not symplectic, but automatically calculate appropriate timesteps to limit per-step
errors to the relative threshold specified in line 6.
If you set use_gpu to 1 (line 21), you can chose between option 1 which is a Semi-Implicit Euler integrator and option 2
which is a second-order Velocity Verlet. The Euler integrator is intended for debugging purposes and not optimized
in any way, so you're basically left with option 2.

The scale setting in line 13 accepts both au or pc but is only relevant if you set autoscale to 0.
Lines 18 to 20 specify the type and frequency of output. Note that the output images are bitmap files with roughly
4 MB per Megapixel per image. 


## Running a simulation
nbody starts in paused mode. The simulation window can be moved freely by left-clicking and dragging the mouse to the desired direction.
You can also zoom in and out using the mouse wheel or the PgUp and PgDown keys. 
Key bindings:
A:	Autoscale
O:	Center screen at origin of coordinate system
C:	Center screen at barycentre of the system
R:	Reset min/max timestep and scale bars
Down/Right/Left/Right:	Rotate
N:	Reset rotation setting
ESC:	Exit
SPACE:	Resume/Pause
I:	Set/unset interactive mode
S:	Screenshot

Remember that the code is still work-in-progress and was not thoroughly optimized yet. If you encounter any bugs,
feel free to contact me.
