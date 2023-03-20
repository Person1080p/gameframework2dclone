# gameframework2d (GF2D)
a collection of utlitity functions designed to facilitate creating 2D games with SDL2
This project is specifically intended to function as an educational tool for my students taking 2D Game Programming.

FOR FULL FEATURES CHECKOUT THE MASTER BRANCH
Students: Don't do that.  You are not ready.... yet.

The main branch is kept deliberately minimal to allow students to build out systems as they are learning.
Other branches have more complete system: Collisions, Windows, Armatures, Entities, Particles...

# Build Process

Before you can build the example code we are providing for you, you will need to obtain the libraries required
by the source code
 - SDL2
 - SDL2_image
 - SDL2_mixer
 - SDL2_ttf
There are additional sub modules that are needed for this project to work as well, but they can be pulled right from within the project.
Performable from the following steps from the root of the cloned git repository within a terminal. 

Make sure you fetch submodules: `git submodule update --init --recursive`
Go into each submodule's src directory and type:
`make`
`make static`

Once each submodule has been made you can go into the base project src folder anre simply type:
`make`

You should now have a `gf2d` binary within the root of your git repository. Executing this will start your game.

# Common Deliverables
- Functioning Entity System <!---Done-->
- Fully interact-able environment <!---Done-->
- User Interface (HUD, Stats, Messages, menus) <!---Done-->
- File read / write access (high score list, saved games, saved songs, etc)
- README for how to play and see all game features (this should be in the git repo) <!---Done, not acutally-->

# Assigned Deliverables
- Turn Based Combat System <!---Done-->
- 10 recruitable monsters
- Player Upgrade System (working)<!--Done, per level, hp multiplier-->
- Tile based world that is randomly generated <!---Done, pick from random existing tile map, and chest map-->
- 5 In-world Interactables (NPC's, Chests, etc.)<!---1/4-->

<!---6/10-->