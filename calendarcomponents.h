#pragma once
/*
Author: Jon Shidal
Purpose:
This file contains declarations for various components used in a Calendar, as well as the Calendar itself. 
All components inherit from DisplayableComponent and have a member variable that is a std::tm object, representing
its date and time. 
*/
#include "displayablecomponent.h"
#include "Enum.h"
#include<ctime>
#include<string>
#include<algorithm>
#include<iostream>



// here is the layout of the tm struct, it is declared in <ctime>

//struct tm {
//	int tm_sec;   // seconds of minutes from 0 to 61
//	int tm_min;   // minutes of hour from 0 to 59
//	int tm_hour;  // hours of day from 0 to 24
//	int tm_mday;  // day of month from 1 to 31
//	int tm_mon;   // month of year from 0 to 11
//	int tm_year;  // year since 1900
//	int tm_wday;  // days since sunday
//	int tm_yday;  // days since January 1st
//	int tm_isdst; // hours of daylight savings time
//}

// forward declarations needed to avoid circular includes, used to declare friends only
class FullCalendarBuilder;
class CalendarInterface;
class IncrementalCalendarBuilder;

class CalendarComponent : public DisplayableComponent {
	friend FullCalendarBuilder;
	friend IncrementalCalendarBuilder;
	friend CalendarInterface;
public:
	// inherited
	virtual void display() = 0;
	//victoria
	CalendarComponent() {};
protected:
	// stores date/time associated with this component
	// see struct tm details above
	std::tm dateInfo;

	// constructor, protected is ok. Builder class is a friend
	CalendarComponent(std::tm, std::shared_ptr<DisplayableComponent>);

	// some static constants to use in place of hardcoded calendar values
	static const std::vector<std::string> daysoftheweek;
	static const std::vector<std::string> months;
	static const std::vector<int> days;
	static const int DAYSINAWEEK;
	static const int MONTHS;
	static const int BASEYEAR;
	static const int DAYSINAYEAR;
};

class DisplayableMonth;
class TodoList;
class DisplayableComponent;
class DisplayableDay : public CalendarComponent {
	friend DisplayableMonth;
	friend FullCalendarBuilder;
	friend CalendarInterface;
	friend IncrementalCalendarBuilder;
public:
	// 1st argument = start date/timeof the day, 2nd argument = its parent
	DisplayableDay(std::tm, std::shared_ptr<DisplayableComponent>);
	virtual void display() override;
	// currently a leaf class, so no need to override addComponent()
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
    bool accessToDoList(tm t, shared_ptr<DisplayableComponent> p);
};

class DisplayableMonth : public CalendarComponent {
	friend class DisplayableDay;
	friend class DisplayableYear;
	friend FullCalendarBuilder;
	friend IncrementalCalendarBuilder;
	friend CalendarInterface;
public: 
	// arguments = date/time info, its parent, name of the month, days in the month
	DisplayableMonth(std::tm, std::shared_ptr<DisplayableComponent>, std::string monthname, unsigned int numdays);
	virtual void display() override;
protected:
	std::string name;
	unsigned int numberOfDays;
	// Month contains days, so it is a composite object. override addComponent accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
};

class Calendar;

class DisplayableYear : public CalendarComponent {
	friend Calendar;
	friend FullCalendarBuilder;
	friend IncrementalCalendarBuilder;
	friend CalendarInterface;
public:
	// arguments: date/time info, parent, leap year or no?
	DisplayableYear(std::tm, std::shared_ptr<DisplayableComponent>, bool);
	virtual void display() override;
protected:
	bool leap;
	// year contains months - override accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
};

class Calendar : public CalendarComponent {
	// friends
	friend FullCalendarBuilder;
	friend IncrementalCalendarBuilder;
	friend CalendarInterface;
protected:
	std::string name;
	size_t yearsToHold;
	std::tm currentDate; // current date and time
	// dateInfo is the start date and time

	// Calendar contains years, so override accordingly
	virtual std::shared_ptr<DisplayableComponent> addComponent(std::shared_ptr<DisplayableComponent>) override;
public:
	// arguments: name of the calendar, length of the calendar in years
	Calendar(std::string n, size_t y);
	// inherited methods
	virtual void display() override;

};

class Event :public CalendarComponent{
	friend class DisplayableDay;
	friend FullCalendarBuilder;
	friend IncrementalCalendarBuilder;
	friend CalendarInterface;
protected:
	int recurrEvery; 
	int recurrFor;
public:
	Event(std::string n, std::tm t, std::shared_ptr<DisplayableComponent> p, int recurrEvery, int recurrFor);
	virtual void display() override;
	std::string event_name;
	std::tm date_time;
	static bool myfunction(shared_ptr<DisplayableComponent> a, shared_ptr<DisplayableComponent> b);  
};

class Task;
class DisplayableDay;
class TodoList :public CalendarComponent {
	friend class DisplayableDay;
	friend DisplayableComponent;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	TodoList( std::tm t, shared_ptr<DisplayableComponent> p);
	shared_ptr<DisplayableComponent> addComponent(shared_ptr<DisplayableComponent> task_to_add);
	virtual void display() override;
	std::tm list_time;
	shared_ptr<DisplayableComponent> MarkComplete(string completed);
};

class Task : public CalendarComponent {
	friend class DisplayableDay;
	friend FullCalendarBuilder;
	friend CalendarInterface;
public:
	Task(std::string s, std::tm t, shared_ptr<DisplayableComponent> p);
	virtual void display() override;
	bool isCompleted;
	static string status;
	std::tm task_time;
	std::string taskName;
};



class mergedecorator :public DisplayableComponent {
	friend class displayableday;
public:
	Event* current_cal;
	string cal_name;
	int event_time;
	string event_n;
	virtual void display() override;
	mergedecorator(string n, shared_ptr<DisplayableComponent> e);
};



