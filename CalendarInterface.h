/*
Author: Jon Shidal
Purpose: declare the user interface for our calendar
*/
#pragma once
#include "calendarbuilder.h"
#include "calendarcomponents.h"
#include "Enum.h"
#include<memory>
#include<map>
using namespace std;


class CalendarInterface {
	std::shared_ptr<Calendar> cal; // the calendar
	std::shared_ptr<CalendarBuilder> builder; // builder for managing construction of the calendar
	std::shared_ptr<DisplayableComponent> currentDisplay; // which component the user is currently viewing
	multimap<string, pair<tm, shared_ptr<DisplayableComponent> > > event_store;
	bool validDate(int, int, int);
public:
	// constructor
	// arguments: 1 - what type of builder? 2 - how many years to hold? 
	CalendarInterface(std::string builderType, size_t years);


	static int depth;
	static tm y;
	static tm m;
	static tm d;

	// calendar traversal functions
	void zoomIn(unsigned int index); // zoom in on a child of the current_display
	void zoomOut(); // zoom out to the parent of the current_display
	void display(); // display the current view to the user
	static string current_file;
};
