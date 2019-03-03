/*
Author: Jon Shidal
Purpose: Define builder classes, responsible for building and managing the structure of the calendar
*/
#include "stdafx.h"
#include "calendarbuilder.h"
#include "calendarcomponents.h"
#include "Enum.h"

using namespace std;

shared_ptr<Calendar> FullCalendarBuilder::buildCalendar(string name, size_t years) {
	currentCalendar = make_shared<Calendar>(name, years);
	// construct each year in a recursive way, add each year as a child of the calendar
	for (int i = 0; i < years; ++i) {
		tm y = currentCalendar->dateInfo;
		y.tm_year += i;
		y.tm_wday = (y.tm_wday + CalendarComponent::DAYSINAYEAR * i) % CalendarComponent::DAYSINAWEEK; // calculate day of the week for first day of the year
		currentCalendar->addComponent(buildYear(y, currentCalendar));
	}
	return currentCalendar;
}

// you may decide to define this.
shared_ptr<DisplayableComponent> FullCalendarBuilder::buildEvent(shared_ptr<DisplayableComponent> cal, string name, tm when, int recurrEvery, int recurrFor) {
	shared_ptr<DisplayableComponent> event_ = make_shared<Event>(name, when, cal, recurrEvery, recurrFor);

	return event_;
}

// you may decide to define this.
shared_ptr<DisplayableComponent> FullCalendarBuilder::getComponentByDate(shared_ptr<DisplayableComponent> cal, tm d, string granularity) {
	if (granularity == "day") {
		return ((*(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon]).children[d.tm_mday]);
	}
	else if (granularity == "month") {
		return (*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon];
	}
	else if (granularity == "year") {
		return (*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year];
	}
	return nullptr;
}

shared_ptr<DisplayableComponent> FullCalendarBuilder::buildDay(std::tm d, std::shared_ptr<DisplayableComponent> p) {

	shared_ptr<DisplayableComponent> day = make_shared<DisplayableDay>(d, p);
	day->children[0] = nullptr;
	return day;
}

shared_ptr<DisplayableComponent> FullCalendarBuilder::buildMonth(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	int index = d.tm_mon;
	shared_ptr<DisplayableComponent> m = make_shared<DisplayableMonth>(d, p, CalendarComponent::months[index], CalendarComponent::days[index]);
	for (int i = 0; i < CalendarComponent::days[index]; ++i) { // for each day in the month
		m->addComponent(buildDay(d, m)); // construct day and add as a child of the month
		++(d.tm_mday); // increment day of the month
		d.tm_wday = (d.tm_wday + 1) % CalendarComponent::DAYSINAWEEK; // increment weekday, reset to 0 if needed
	}
	return m;
}
shared_ptr<DisplayableComponent> FullCalendarBuilder::buildYear(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	shared_ptr<DisplayableComponent> y = make_shared<DisplayableYear>(d,p,false);
	// construct each month and add it as a child of the year
	for (int i = 0; i < CalendarComponent::MONTHS; ++i) {
		d.tm_mon = i;
		y->addComponent(buildMonth(d, y));
		// set week day of first day of the next month
		d.tm_wday = (d.tm_wday + CalendarComponent::days[i]) % CalendarComponent::DAYSINAWEEK; 
	}
	return y;
}
//
////3. Incremental builder: Currently the FullCalendarBuilder constructs the entire calendar at once, 
////which allocates a lot of objects and uses a lot of memory. In this part, you will implement an incremental builder. 
////The goal is to construct components of the calendar on demand as they are needed. Your IncrementalCalendarBuilder class should inherit from CalendarBuilder. 
////Initially, only a Calendar object should be constructed along with the current year, month, and day(the day, month, and year associated with the current date). 
////As the user zooms in/out or jumps to a particular date and granularity, components of the calendar should be constructed and added to the calendar as needed by the builder. 
////A user should be able to zoom in to a child that does not exist yet, so you may need to update the zoom functions accordingly.
//
//


shared_ptr<Calendar> IncrementalCalendarBuilder::buildCalendar(std::string name, size_t years) {
	currentCalendar = make_shared<Calendar>(name, years);
	DisplayableComponent::full = false;
	for (int i = 0; i < years; ++i) {
		tm y = currentCalendar->dateInfo;
		y.tm_year += i;
		y.tm_wday = (y.tm_wday + CalendarComponent::DAYSINAYEAR * i) % CalendarComponent::DAYSINAWEEK; // calculate day of the week for first day of the year
		currentCalendar->addComponent(nullptr);
	}
	return currentCalendar;
}

shared_ptr<DisplayableComponent> IncrementalCalendarBuilder::buildDay(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	shared_ptr<DisplayableComponent> day = make_shared<DisplayableDay>(d, p);
	return day;
}

shared_ptr<DisplayableComponent> IncrementalCalendarBuilder::buildMonth(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	int index = d.tm_mon;
	shared_ptr<DisplayableComponent> m = make_shared<DisplayableMonth>(d, p, CalendarComponent::months[index], CalendarComponent::days[index]);

	for (int i = 0; i < CalendarComponent::days[index]; ++i) { // for each day in the month
		m->addComponent(nullptr); // construct day and add as a child of the month
		++(d.tm_mday); // increment day of the month
		d.tm_wday = (d.tm_wday + 1) % CalendarComponent::DAYSINAWEEK; // increment weekday, reset to 0 if needed
	}
	return m;
}

shared_ptr<DisplayableComponent> IncrementalCalendarBuilder::buildYear(std::tm d, std::shared_ptr<DisplayableComponent> p) {
	shared_ptr<DisplayableComponent> y = make_shared<DisplayableYear>(d, p, false);



	for (int i = 0; i < CalendarComponent::MONTHS; ++i) {
		d.tm_mon = i;
		y->addComponent(nullptr);
		d.tm_wday = (d.tm_wday + CalendarComponent::days[i]) % CalendarComponent::DAYSINAWEEK;
	}
	return y;
}


shared_ptr<DisplayableComponent> IncrementalCalendarBuilder::buildEvent(shared_ptr<DisplayableComponent> cal, string name, tm when, int recurrEvery, int recurrFor) {
	shared_ptr<DisplayableComponent> event_ = make_shared<Event>(name, when, cal, recurrEvery, recurrFor);
	return event_;
}

shared_ptr<DisplayableComponent> IncrementalCalendarBuilder::getComponentByDate(shared_ptr<DisplayableComponent> cal, tm d, string granularity) {

	if (granularity == "day") {
		if ((*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year] == nullptr) { // check if the year exists
			(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year] = buildYear(d, cal); // if not, build the specific year
		}
		if ((*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon] == nullptr) { //check if the month exists
			(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon] = buildMonth(d, cal); //if not, build the specific month

			if ((*(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon]).children[d.tm_mday] == nullptr) {//check if the day exists

				(*(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon]).children[d.tm_mday] = buildDay(d, cal);// if not, build the specific day
				return (*(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon]).children[d.tm_mday]; // return the day
			}

			else {
				return ((*(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon]).children[d.tm_mday]);// all the year/month/day on the path to the day exist
																																		  // return the day
			}


		}
	}
	else if (granularity == "month") {
		if ((*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year] == nullptr) {
			(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year] = buildYear(d, cal);
		}
		if ((*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon] == nullptr) {
			(*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon] = buildMonth(d, cal);
			return (*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon];
		}
		else {
			return (*(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year]).children[d.tm_mon];

		}
	}

	else if (granularity == "year") {
		if ((*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year] == nullptr) {
			(*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year] = buildYear(d, cal);
			return (*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year];
		}
		else {
			return (*cal).children[d.tm_year - (*currentCalendar).currentDate.tm_year];
		}

	}
	return nullptr;

}
