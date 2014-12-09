#include <string>
#include <vector>
using namespace std;

#ifndef NOTIFICATION_CENTER
#define NOTIFICATION_CENTER

class INotifiable {
public:
	virtual void respond(string notificationString, int value) = 0;
};

class NotificationCenter {
private:
	vector<INotifiable*> _listeners;
public:
	static NotificationCenter* getNotificationCenter();

	void registerForNotifications(INotifiable* listener);
	void notify(string notificationString, int value);
};

#endif
