# QT_CadCam_rev0
CadCam program in development, for usage with cnc machines.

To download the program's appimage and latest source code, check the latest release, on the right of the screen.
The zip contains latest appimage + source code is in the zip file !

Example of making a dxf drawing in Librecad:
![alt text](https://github.com/grotius-cnc/QT_CadCam_rev0/blob/master/librecad_example.png)

Result after autonesting, jobs are pocket, drill hole (in blue), plasma contour cut.
![alt text](https://github.com/grotius-cnc/QT_CadCam_rev0/blob/master/result.png)


Program includes :

- set up tool lists for Mill, Drill, Plasma-laser and Oxyfuel.
- set up multiple heads for Z, A axis etc with tool offsets and custom M code's.
- set up user defined gcode output format, including gcode intro, outtro and custom M code command's.
- outside, inside- and no contour offsets.
- pocket offset with or without islands.
- can import librecad, inkscape and freecad dxf's.
- true spline implementation.
- bezier, ellipse, polygon, linestrip, line, arc, circle, spline support.
- nesting support with a java based interface, supports java 1.8. (using a downgrade compile with netbeans)
- keep parts together output.

Todo :
- insert cad tools, etc.
- tangential gcode output
- rotary axis gcode output, to cut shapes in metal profiles.
- auto nesting, currently not nesting the pocket islands, the nesting see this islands as inner contours at the moment.

Testers comment's:
- very responsive
- fast
- why not use enum for object.types?
- add object selection by mouse and then cut operation

See the deploy directory how to retrieve the appimage by yourself.
