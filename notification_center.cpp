#include "notification_center.hpp"

static NotificationCenter* _singleton;

NotificationCenter* NotificationCenter::getNotificationCenter() {
	if(_singleton == nullptr) {
		_singleton = new NotificationCenter();
	}
	return _singleton;
}

void NotificationCenter::registerForNotifications(INotifiable* listener) {
	_listeners.push_back(listener);
}

void NotificationCenter::notify(string notificationString, int value) {
	for(INotifiable* l : _listeners) {
		l->respond(notificationString,value);
	}
}