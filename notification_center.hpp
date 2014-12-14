#include <string>
#include <vector>
using namespace std;

#ifndef NOTIFICATION_CENTER
#define NOTIFICATION_CENTER

class INotifiable {
	// If an object wants to receive the notifications sent to the notification
	// center it must implement this protocol
public:
	// recieve a message from the notification center
	virtual void respond(string notificationString, int value) = 0;
};

class NotificationCenter {
	// singleton object that allows arbitrary message passing around the system,
	// objects can send strings that will be echos to all objects registered to
	// listen to them
private:
	vector<INotifiable*> _listeners;
public:
	static NotificationCenter* getNotificationCenter();

	void registerForNotifications(INotifiable* listener);
	
	// send a notification to all objects registered to listen for them
	void notify(string notificationString, int value);
};

#endif
