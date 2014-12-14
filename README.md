Install instructions
----------------
### Mac OSX
	./install
	./run
Design and Approach
=========
Features Of Note
--------
### Exploding Zombies
To do this I went to the vertex shader, by using the direction of the normals,
I could project the vertices in that direction using some projectile motion
equations. What initially was planned to be normal projectile motion, I decided
I would make the vertices rise rather than fall to give a sort of ghost effect.
This whole mechanism was controlled by a uniform variable called "EXPLODE",
this variable essentially represented time in the projectile equations, so at
0, the mesh did not explode. After I had this to my liking, I decided to move
back up to the Zombie object and make him thrash and spin when he died. This
would combine with the explosion to give a stormish effect.

### Lighting
The first thing I did with the lighting was distance attenuation, this was a
trivial matter of dividing by the distance between the light source and the
fragment position squared.

In order to do the glow I needed to color fragments that were
in close proximity to the light source from the viewers POV, while I'm sure
I could have achieved this end by mapping the light position through the
perspective matrix and measuring their distance in the projection space and
using z to scale, I did it using distance between the line between the viewer
and the vertex, and the light source. Once I had this and attenuated the glow in
a fashion similar to the general light attenuation I started messing with
brightnesses being a more sinusoidal function of the distance, this resulted in
really cool concentric circles. Then I applied a similar approach to the
brightness on the walls but a sinusoidal function of the distance from the
light and also a dot product of the wall normal and the light to wall vector.
While this was initially meant for the wall it gave some interesting effects
on the gun and zombies.

### Running Bounce and Gun Recoil
The bounce as the player runs is a absolute sine wave as a function of time
from the beginning of the run

### Libraries:
* [OpenGL Extension Wrangler Library]
* [GLFW]
* [Assimp]
* [DevIL]
* [irrKlang]
* [GLM]

[OpenGL Extension Wrangler Library]:http://glew.sourceforge.net
[GLFW]: http://www.glfw.org
[Assimp]: http://assimp.sourceforge.net
[DevIL]: http://openil.sourceforge.net
[irrKlang]: http://www.ambiera.com/irrklang/
[GLM]: http://glm.g-truc.net/0.9.6/index.html
