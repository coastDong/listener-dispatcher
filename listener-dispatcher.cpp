// listener-dispatcher.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <functional>
#include <iostream>
#include <map>
#include <vector>


// Test 1: AS3.0 style

//class Event
//{
//public:
//	Event() 
//	{
//		type = 0; 
//		data = nullptr;
//	}
//	virtual ~Event(){}
//
//	void *data;
//	int type;
//};
//
//class KeybordEvent : public Event
//{
//public:
//	KeybordEvent(){};
//	virtual ~KeybordEvent(){};
//public:
//	const static int KEY_DOWN = 1001;
//	const static int KEY_UP = 1002;
//	//...
//};
//
//class MouseEvent : public Event
//{
//public:
//	MouseEvent(){};
//	virtual ~MouseEvent(){};
//
//public:
//	const static int MOUSE_DOWN = 2001;
//	const static int MOUSE_UP =  2002;
//};
//
//class EventDispatcher
//{
//public:
//	EventDispatcher() {};
//	virtual ~EventDispatcher(){};
//
//	virtual void addEventListener(const int eventType,std::function<void(Event*)> func)
//	{
//		m_listners[eventType] = func;
//	}
//	virtual void removeEventListener(const int eventType)
//	{
//		ListenerMap::iterator it = m_listners.find(eventType);
//		if (it != m_listners.end())
//		{
//			m_listners.erase(it);
//		}
//	}
//
//	void dispatch(Event* evt)
//	{
//		ListenerMap::iterator it = m_listners.find(evt->type);
//		if (it == m_listners.end())
//		{
//			return;
//		}
//
//		//proc
//		(it->second)(evt);
//
//		//remove it;
//		m_listners.erase(it);
//	}
//
//protected:
//	typedef std::map<const int , std::function<void(Event*)> > ListenerMap;
//	ListenerMap	m_listners;
//};
//
//class Test : public EventDispatcher
//{
//public:
//	Test() {};
//	virtual ~Test() {};
//};
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	Test test;
//	test.addEventListener(KeybordEvent::KEY_DOWN,[](Event *evt){
//		std::cout<<evt->type<<std::endl;
//	});
//
//	KeybordEvent evt;
//	evt.type = KeybordEvent::KEY_DOWN;
//	evt.data = (void *)"key down";
//	test.dispatch(&evt);
//
//
//	MouseEvent e;
//	e.type = MouseEvent::MOUSE_DOWN;
//	e.data = (void *)"mouse down";
//	test.dispatch(&evt);
//
//	system("pause");
//	return 0;
//}


//Test 2: cocos 3.x style. just a shit.

class Event
{
public:
	enum class Type:unsigned int
	{
		TOUCH,
		KEYBOARD,
		ACCELERATION,
		MOUSE,
		CUSTOM,
	};

	Event(Type type) 
	{
		this->type = type;
		data = nullptr;
	}

	virtual ~Event(){}

	
	Type  type;
	void *data;
	unsigned int  eventCode;
};

class KeybordEvent : public Event
{
public:
	KeybordEvent()
		:Event(Event::Type::KEYBOARD)
	{}
	virtual ~KeybordEvent(){}

	enum class EventCode:unsigned int
	{
		KEY_DOWN,
		KEY_UP,
	};

};

class MouseEvent : public Event
{
public:
	MouseEvent()
		:Event(Event::Type::MOUSE)
	{}
	virtual ~MouseEvent(){}

	enum class EventCode:unsigned int
	{
		MOUSE_DOWN,
		MOUSE_UP,
	};
};

class CustomEvent : public Event
{
public:
	CustomEvent(const std::string& evtName)
		:Event(Event::Type::CUSTOM)
	{
		this->eventName = eventName;
	}

	virtual ~CustomEvent() {}

	std::string  eventName;
};

class EventListener
{
public:
	EventListener() {}
	virtual ~EventListener() {}

	virtual void onEvent(Event *evt) = 0;
};

class KeyEventListener : public EventListener
{
public:
	KeyEventListener()
	{

	}
	virtual ~KeyEventListener() {}

	std::function<void (Event*)> keyDown;
	std::function<void (Event*)> keyUp;

	void onEvent(Event *evt)
	{
		if ((KeybordEvent::EventCode)evt->eventCode == KeybordEvent::EventCode::KEY_DOWN)
		{
			this->keyDown(evt);
		}
		else if ((KeybordEvent::EventCode)evt->eventCode == KeybordEvent::EventCode::KEY_UP)
		{
			this->keyUp(evt);
		}
	}

	const static std::string listenerId;
};

const std::string KeyEventListener::listenerId = "_event_key_";

class MouseEventListener : public EventListener
{
public:
	MouseEventListener()
	{

	}
	virtual ~MouseEventListener() {}

	std::function<void (Event*)> MouseDown;
	std::function<void (Event*)> MouseUp;

	void onEvent(Event *evt)
	{
		if ((MouseEvent::EventCode)evt->eventCode == MouseEvent::EventCode::MOUSE_DOWN)
		{
			this->MouseDown(evt);
		}
		else if ((MouseEvent::EventCode)evt->eventCode == MouseEvent::EventCode::MOUSE_UP)
		{
			this->MouseUp(evt);
		}
	}

	const static std::string listenerId;
};

const std::string MouseEventListener::listenerId = "_event_mouse_" ;

class CustomEventListener : public EventListener
{
public:
	CustomEventListener() {}
	virtual ~CustomEventListener() {}

	std::string eventName;
	std::function<void (Event*)> callback;

	void onEvent(Event *evt)
	{
		if (static_cast<CustomEvent*>(evt)->eventName != this->eventName)
		{
			return;
		}
		this->callback(evt);
	}
};

const std::string& getListenerId(Event *evt)
{
	if (evt->type == Event::Type::KEYBOARD)
	{
		return KeyEventListener::listenerId;
	}
	else if (evt->type == Event::Type::MOUSE)
	{
		return MouseEventListener::listenerId;
	}
	else if (evt->type == Event::Type::CUSTOM)
	{
		return static_cast<CustomEvent *>(evt)->eventName;
	}
	return "";
}

class EventDispatcher
{
public:
	static EventDispatcher& getInstance()
	{
		static EventDispatcher dispatcher;
		return dispatcher;
	}

	virtual void addEventListener(EventListener* listener)
	{
		 m_listners.push_back(listener);
	}

	virtual void removeEventListener(EventListener* listener)
	{
		for (ListenerVec::iterator it = m_listners.begin(); it != m_listners.end();)
		{
			if (*it == listener)
			{
				it = m_listners.erase(it);
				break;
			}
		}
	}

	void dispatch(Event* evt)
	{
		if (evt->type == Event::Type::TOUCH)
		{
			//special proc, so ugly
			//dispatchTouchEvent(static_cast<EventTouch*>(event));
		}
		else
		{
			for (ListenerVec::iterator it = m_listners.begin(); it != m_listners.end(); ++it)
			{
				(*it)->onEvent(evt);
			}
		}

	}

protected:
	EventDispatcher() {};
	virtual ~EventDispatcher(){};
	typedef std::vector< EventListener* > ListenerVec;
	ListenerVec		m_listners;
};



int _tmain(int argc, _TCHAR* argv[])
{
	KeyEventListener *keyListener = new KeyEventListener();//where is it delete?
	keyListener->keyDown = [](Event *evt){
		std::cout<<(const char*)(evt->data)<<std::endl;
	};
	keyListener->keyUp = [](Event *evt){
		std::cout<<(const char*)(evt->data)<<std::endl;
	};

	EventDispatcher::getInstance().addEventListener(keyListener);


	KeybordEvent e;
	e.eventCode = (unsigned int)KeybordEvent::EventCode::KEY_DOWN;
	e.data = (void *)("key down");
	EventDispatcher::getInstance().dispatch(&e);


	CustomEventListener *customListener = new CustomEventListener();
	customListener->callback = [](Event *evt){
		std::cout<<*(int *)evt->data<<std::endl;
	};

	EventDispatcher::getInstance().addEventListener(customListener);//where is it delete?

	CustomEvent customEvent("_custom_event_");
	int a = 10;
	customEvent.data = (void*)&a; 

	EventDispatcher::getInstance().dispatch(&customEvent);


	system("pause");
	return 0;
}

