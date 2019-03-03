/*
Author: Jon Shidal
Purpose: Declaration of DisplayableComponent. DisplayableComponent is the common base class of all displayable objects.
It declares and defines the interface that all objects in the composite share.
*/
#pragma once
#include "Enum.h"
#include<memory>
#include<vector>
#include<string>
using namespace std;
//#include "calendarcomponents.h" 



// forward declaration, needed for friend declaration
class FullCalendarBuilder;
class IncrementalCalendarBuilder;
class CalendarInterface;
class DisplayableMonth;

// abstract base class representing a composite object
class DisplayableComponent {
	friend FullCalendarBuilder;
	friend IncrementalCalendarBuilder;
	friend CalendarInterface;
	friend class DisplayableMonth;
//	friend void DisplayableMonth::display();
public:
	// defined by concrete base classes deriving from DisplayableComponent
	virtual void display() = 0;
	static bool full;
	//victoria
	DisplayableComponent() {};
virtual ~DisplayableComponent();

protected:
	// member variables
	std::vector<std::shared_ptr<DisplayableComponent> > children;
	std::weak_ptr<DisplayableComponent> parent; // weak_ptr to avoid cycles, does not contribute to reference count

	// construction, destruction
	DisplayableComponent(std::shared_ptr<DisplayableComponent>);
	

	// composite and traversal functions
	virtual std::weak_ptr<DisplayableComponent> getParent();
	virtual std::shared_ptr<DisplayableComponent> getChild(unsigned int);
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>);
	virtual std::shared_ptr<DisplayableComponent> removeComponent(unsigned int);
};
