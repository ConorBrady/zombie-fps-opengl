
#ifndef CONTROL_INTERFACE
#define CONTROL_INTERFACE

enum ControlSignal {
	CONTROL_SIGNAL_PRIMARY_X,
	CONTROL_SIGNAL_PRIMARY_Y,
	CONTROL_SIGNAL_ALT_X,
	CONTROL_SIGNAL_ALT_Y,
	CONTROL_SIGNAL_PRIMARY_ACTION
};

class IControllable {
public:
	virtual void signal(ControlSignal cs, float value) = 0;
};

class IController{
public:
	virtual void addControllable(IControllable* controllable) = 0;
};

#endif
