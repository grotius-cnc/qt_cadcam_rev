# QT_CadCam_rev0
CadCam program in development, for usage with cnc machines.

Work in progress, but output's gcode.

To download the program's appimage Cadcam v1.0: https://github.com/grotius-cnc/QT_CadCam_rev0/releases/tag/v1.0

Program includes :

- set up tool lists for Mill, Drill, Plasma-laser and Oxyfuel.
- set up multiple heads for Z, A axis etc with tool offsets and custom M code's.
- set up user defined gcode output format, including gcode intro, outtro and custom M code command's.
- outside, inside- and no contour offsets.
- pocket offset with or without islands.
- can import librecad, inkscape and freecad dxf's.
- true spline implementation.
- bezier, ellipse, polygon, linestrip, line, arc, circle, spline support.
- nesting support with a java based interface, has to be compiled with lower java version to improve compatibility with linux.

Todo : 
- keep parts together option.
- insert cad tools, etc.
- program more in module style.
- tangential gcode output
- rotary axis gcode output, to cut shapes in metal profiles.

Testers comment's:
- very responsive
- fast
- why not use enum for object.types?
- add object selection by mouse and then cut operation

See the deploy directory how to retrieve the appimage.

![alt text](https://github.com/grotius-cnc/QT_CadCam_rev0/blob/master/result.png)


