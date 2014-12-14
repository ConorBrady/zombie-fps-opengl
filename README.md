Install instructions
----------------
### Mac OSX
	./install
	./run
Design and Approach
=========
Overview
--------
### Interfaces
Most behaviour exists between the interfaces, so it would make sense to begin with them.
```c
class IAudible {
	// An object that makes sound in the 3D space
public:
	virtual glm::vec3 getLocation() = 0; // Needed for 3D sound positioning

	virtual const char* ambientSound() = 0; // filename for the Ambient sound
	virtual float getAmbientSoundVolume() = 0; // Volume at this moment in time
	virtual bool shouldPlayAmbientSound() = 0;
	virtual bool shouldContinueAmbientSound() = 0;

	virtual const char* spawnSound() = 0;
	virtual bool shouldContinueSpawnSound() = 0; // possibility to stop if dead

	virtual const char* destroySound() = 0;
	virtual bool shouldPlayDestroySound() = 0;
	virtual bool shouldContinueDestroySound() = 0;
};
```

```c
enum ControlSignal {
	// the set of control signals defined for passing in the game
	// these can be given a value for added expressiveness, this
	// value would be appropriately between -1 and 1.

	CONTROL_SIGNAL_PRIMARY_X, // used to strafe camera
	CONTROL_SIGNAL_PRIMARY_Y, // used to stride camera
	CONTROL_SIGNAL_ALT_X, // used to aim gun and alter camera yaw
	CONTROL_SIGNAL_ALT_Y, // used to aim gun and alter camera pitch
	CONTROL_SIGNAL_PRIMARY_ACTION // used to shoot gun
};
```

```c
class IControllable {
	// an object that will respond to some of the set out control signals
public:
	// recieve signal and value from controller
	virtual void signal(ControlSignal cs, float value) = 0;
};
```

```c
class IController{
	// an object that on occasion is capable of generating control signals
public:
	virtual void addControllable(IControllable* controllable) = 0;
};

```

```c
class ITrackable {
	// an object capable of being tracked in the 3D space
	public:
	virtual glm::vec3 getLocation() = 0;
};
```

```c
class IFollower {
	// an object interested in following 1 or more objects
	public:
	virtual void addFollowable(ITrackable* followable) = 0;
};
```

```c
enum CollisionProperty {
	// objects will exhibit various properties that effect the objects they
	// collide with, for example if a bullet hits a zombie, it should die.
	// Therefore the bullet should give a "KILL_ZOMBIE" property, so once the
	// bullet is passed to the zombie on collision it can deduce how it should
	// react without caring if it is a bullet, so any object then can easily
	// kill a zombie. This is applied to all situations.

	NONE 				 		= 0,
	KILL_ZOMBIE 			  	= 1 << 0, // kill zombies on impact
	KILL_HUMAN 			   	= 1 << 1, // kill humans on impact
	STOP_MOVING_BULLET			= 1 << 2, // stop a moving bullet
	ADD_BULLET					= 1 << 3, // gift a bullet on impact
	DESTROY_STATIONARY_BULLET 	= 1 << 4  // destroy a bullet if stationary
};
```

```c
class ICollidable : public ITrackable {
	// an object than may be involved in a collision in the 3D space, note at
	// this point it is only defined to be a point based on it being Trackable
	// This interface should not be directly implemented as the collision space
	// will only accept Lines and Cylinders
public:
	// method to call when a collision happens, each objects collide method is
	// called with the other object being passed as a parameter
	virtual void collided(ICollidable* collided) = 0;

	// see "enum CollisionProperty"
	virtual int getCollisionProperties() = 0;

	// required as the object collide one after another, resulting in the state
	// of the first object changing before it can be "collided" the second,
	// therefore a copy must be made of each before the collision
	virtual ICollidable* clone() = 0;
};
```

```c
class ICollidableCylinder : public ICollidable {
	// A collidable object with a cylindrical shape examples include zombies and
	// the player
public:
	virtual float getCollidableHeight() = 0;
	virtual float getCollidableRadius() = 0;
};

```
```c
class ICollidableLine : public ICollidable {
	// A line segment that can be checked for intersection with a 3D shape such
	// as a cyclinder, an example is the bullets the player shoots. Their
	// trajectories are broken into discrete line segments.
public:
	virtual LineSegment* getLineSegment() = 0;
};```
```c
class AudibleSpace {
	// The singleton object that takes care of all the audio in the game in 3D
	// space
public:
	static AudibleSpace* sharedAudibleSpace();

	void update(float time); // stop or start sounds if needed with time
	void setCamera(Camera* camera); // camera to orientate 3D sound
	void addAudible(IAudible* audible); // add audible object to the space

	void setSoundTrack(const char* filename); // Looped background audio
};

```



Controllers and Controllables
----------
I began with specifying a pair of interfaces:

```c
class IControllable {
public:
	virtual void signal(ControlSignal cs, float value) = 0;
};

class IController{
public:
	virtual void addControllable(IControllable* controllable) = 0;
};
```

Along with an enumeration of valid control signals:
```c
enum ControlSignal {
	CONTROL_SIGNAL_PRIMARY_X,
	CONTROL_SIGNAL_PRIMARY_Y,
	CONTROL_SIGNAL_ALT_X,
	CONTROL_SIGNAL_ALT_Y,
	CONTROL_SIGNAL_PRIMARY_ACTION
};
```
This enabled me to decouple the decoding of the controller input, in this case GLFW keyboard and mouse inputs from the messages they were conveying to the controllable object, this would allow for arbitrary connections of controllable objects to controllers rather than having to concern with the type of controller it was. The only controller class implemented was ```Window``` and the main controllable was ```Player``` which was composed of a ```Gun``` and ```Camera``` controllable and it forwarded all received control signals to both of them.

scoring and winning/losing
3-dimensional objects and views
at least one hierarchical, animated creature or object
at least one texture-mapped object
some form of lighting calculation that suits your game style, programmed in shaders
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
