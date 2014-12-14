
#ifndef CONTROL_INTERFACE
#define CONTROL_INTERFACE

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

class IControllable {
	// an object that will respond to some of the set out control signals
public:
	// recieve signal and value from controller
	virtual void signal(ControlSignal cs, float value) = 0;
};

class IController{
	// an object that on occasion is capable of generating control signals
public:
	virtual void addControllable(IControllable* controllable) = 0;
};

#endif
