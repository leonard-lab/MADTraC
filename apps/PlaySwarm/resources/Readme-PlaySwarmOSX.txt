PlaySwarm - Trajectory Data Playback

Dan Swain, dan.t.swain@gmail.com, 12/12/09

QUICK START:

Copy PlaySwarm.app to your Applications folder.  Double
click to open.  Select "Cheat Sheet" from the "Help" menu.

Contents:
I.      REQUIREMENTS
II.     PACKAGE CONTENTS
III.    INSTALLATION
IV.     USE
V.      FILE FORMATS

I. REQUIREMENTS:

Should work "out of the box" on any OS X 10.5 system.
I haven't been able to test it on any other versions. 
PlaySwarm probably won't work on OS X 10.4 and older, and
it might work on 10.6.  If you're able to verify any
of this, please let me know.

II. CONTENTS:

1. PlaySwarm.app - The PlaySwarm application bundle.
2. showinPlaySwarm.m - Function to export trajectory data
                        from MATLAB.
3. Readme-PlaySwarmOSX.txt - This readme file.

III. INSTALLATION:

1. Drag and Drop PlaySwarm.app into you Applications folder
        (/Applications).  It should also work in ~/Applications
        or anywhere else, but you'll need to change the
        path in showinPlaySwarm.m to use it in MATLAB.
2. Drag and drop showinPlaySwarm.m to somewhere in your MATLAB
        path.  If you're not sure what your MATLAB path is
        you can type "path" in MATLAB to find out.  
        ~/Documents/MATLAB should be in your MATLAB path
        (where ~ is your home directory).

IV. USE:

PlaySwarm should act like most regular applications.  It's
set up to open .dat files, though any file with the right
format (see below) should work.  You can start it by 
double-clicking the icon and then selecting a file, you can 
drag and drop a file onto its icon, and once you've
opened at least one .dat file you should be able to right
click and choose PlaySwarm from the "Open With" menu.  
Once you've opened a specific file with it you should be
able to double-click that file and have it open in PlaySwarm.

You can also start PlaySwarm from the command line.  If you're
not familiar with the OS X application bundle framework,
the "PlaySwarm" that you see in you Applications folder
is actually a folder called "PlaySwarm.app".  You can view it
in Finder by right clicking and choosing "Show Package Contents"
(this works for any application - try it).  You can access
the executable from the command line by calling
/Applications/PlaySwarm.app/Contents/MacOS/PlaySwarm (this is
what showinPlaySwarm.m does).  If you plan to do this a lot
you may want to symlink it - type

sudo ln -s /Applications/PlaySwarm.app/Contents/MacOS/PlaySwarm 
 /usr/local/bin/PlaySwarm

(All on one line.)  Then you should simply be able to run it
by typing "PlaySwarm" at the command line.  You most likely
want to run it in the background by appending a "&" to the
command line.

For command line arguments, run the program and select
"Cheatsheet" from the "Help" menu.  Or run the program
with the "-h" or "--help" from the command line.

V. FILE FORMATS

PlaySwarm can currently "handle" two different file formats.
It's not too hard to add support for other formats, so if
you have a format you'd like to use just let me know.  It's
also possible to convert pretty much any format to the
"simple" format below by loading it into MATLAB.  The 
showinPlaySwarm.m script uses the "simple" format.

The format of a file is determined automatically.  Currently
the program simply counts the number of rows and columns
in the file.  If there are more columns than rows, the
"Simple" format is assumed.

A. Simple Format:

Each row of the file corresponds to one agent/particle/fish.
All of the "x" data rows come first and then the "y" rows.
Assuming there are N agents and NT time steps.  There are
2N rows.  The data should look like

x1[1] x1[2] ... x1[NT]
x2[1] x2[2] ... x2[NT]
...
xN[1] xN[2} ... xN[NT]
y1[1] y1[2] ... y1[NT]
y2[1] y2[2] ... y2[NT]
...
yN[1] yN[2} ... yN[NT]

where x1[1] is the x position of agent 1 at time 1, etc.

B. "Killi" Format:

Each row of the file corresponds to a time index.  All
of the data for agent 1 is listed, then all of the data
for agent 2, etc.  The first column is the agent index,
the second column is the time, the third column is the
x position, the fourth column is the y position, and
any extra columns are ignored.  Data looks like

index1 t1[1] x1[1] y1[1] ... (other data) ...
index1 t1[2] x1[2] y1[2] ... (other data) ...
....  (as many times as there are)
index2 t2[1] x2[1] y2[1] ... (other data) ...
index2 t2[2] x2[2] y2[2] ... (other data) ...
....  (as many times as there are)
....  (as many fish as there are)
indexN tN[1] xN[1] yN[1] ... (other data) ...
indexN tN[2] xN[2] yN[2] ... (other data) ...
....  (as many times as there are)
   
