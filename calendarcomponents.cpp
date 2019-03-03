/*
Author: Jon Shidal
Purpose: Define Calendar component classes.
*/

#include "stdafx.h"
#include "calendarcomponents.h"
#include "displayablecomponent.h"
#include "Enum.h"
#include<iostream>
#include<string>
#include<iomanip>
#include <vector>
#include<ctime>
#include <algorithm>
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>

using namespace std;

// from <ctime>
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

// static class variables
const std::vector<string> CalendarComponent::daysoftheweek = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const std::vector<string> CalendarComponent::months = { "January", "February", "March", "April", "May", "June", "July", "August","September", "October", "November","December" };
const std::vector<int> CalendarComponent::days = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const int CalendarComponent::DAYSINAWEEK = 7;
const int CalendarComponent::MONTHS = 12;
const int CalendarComponent::BASEYEAR = 1900;
const int CalendarComponent::DAYSINAYEAR = 365;
string Task::status = "TODO";
// CalendarComponent
CalendarComponent::CalendarComponent(std::tm d, std::shared_ptr<DisplayableComponent> p) : DisplayableComponent(p), dateInfo(d){}

//event
Event::Event(std::string n, std::tm t, std::shared_ptr<DisplayableComponent> p, int recurrEvery, int recurrFor): CalendarComponent(t, p), event_name(n), date_time(t), recurrEvery(recurrEvery), recurrFor(recurrFor){}



void Event::display() {
	cout  << event_name;
	
}

bool Event::myfunction(shared_ptr<DisplayableComponent> a, shared_ptr<DisplayableComponent> b)
{
	Event* aa = dynamic_cast<Event*>(a.get());
	Event* bb = dynamic_cast<Event*>(b.get());
	return ((aa->dateInfo.tm_hour) < (bb->dateInfo.tm_hour));
}




// Day
DisplayableDay::DisplayableDay(std::tm d, std::shared_ptr<DisplayableComponent> p) : CalendarComponent(d, p){
	children.push_back(nullptr);
} 

void DisplayableDay::display() {
	if (full == true) {
		cout << daysoftheweek[dateInfo.tm_wday] << " ";
		cout << dateInfo.tm_mon + 1 << "/" << dateInfo.tm_mday << "/" << dateInfo.tm_year + CalendarComponent::BASEYEAR << endl;

		for (size_t i = 1; i < children.size(); ++i) {
			

			if (children[i] != nullptr) {
				Event* e = dynamic_cast<Event*>(children[i].get());
				
				children[i]->display();
				if (e != nullptr) {
					cout << " T:" << (e->dateInfo.tm_hour-sei+hourNum)% hourNum;
				}
				cout << endl;
			}
		}
		if (children[0] != nullptr)
		{
			children[0]->display();
			cout << endl;
		}
	}
	else {
		dateInfo.tm_wday = (dateInfo.tm_wday + weekdayNum) % weekdayNum;
		cout << "\t\t\t" << daysoftheweek[dateInfo.tm_wday] << " ";


		cout << dateInfo.tm_mon + 1 << "/" << dateInfo.tm_mday << "/" << dateInfo.tm_year + CalendarComponent::BASEYEAR << endl;
	}
}


class Event;
shared_ptr<DisplayableComponent> DisplayableDay::addComponent(shared_ptr<DisplayableComponent> comp) {
	Event* e = dynamic_cast<Event*> (comp.get());

	if (e == nullptr) {
		mergedecorator* m = dynamic_cast<mergedecorator*>(comp.get());
		if (m == nullptr) {
			return nullptr;
		}
		else
		{
			children.push_back(comp);
			return comp;
		}
		return nullptr;
	}
	
	children.push_back(comp);
	sort(children.begin() +1 , children.end(), Event::myfunction);
	return comp;
}

class TodoList;
class DisplayableDay;

bool DisplayableDay::accessToDoList(tm t, shared_ptr<DisplayableComponent> p)
{
	if (children[0] == nullptr) {
		return true;
	}

	return false;
		
}






// Month
DisplayableMonth::DisplayableMonth(std::tm d, std::shared_ptr<DisplayableComponent> p, string monthname, unsigned int numdays) : CalendarComponent(d, p), name(monthname), numberOfDays(numdays) {
	// initialize children vector, one spot for each day
	for (size_t i = 0; i < numberOfDays; ++i) {
		children.push_back(nullptr);
	}
}
class DisplayableComponent;
void DisplayableMonth::display() {
	if(full == true){
	cout << "\t\t" << name << ":" << endl;
	cout << setw(setlength) << "Sunday" << setw(setlength) << "Monday" << setw(setlength) << "Tuesday" << setw(setlength) << "Wednesday" << setw(setlength) <<  "Thursday" << setw(setlength) << "Friday" << setw(setlength) <<"Saturday" << endl;
	//cout << dateInfo.tm_year << "  " << dateInfo.tm_mon << "  " << dateInfo.tm_mday << endl;

	int ff = dateInfo.tm_wday;
	cout <<ff << endl;
	
	int month_days = CalendarComponent::days[dateInfo.tm_mon];
	int last_w = ((ff + month_days % weekdayNum)-1) % weekdayNum;
	
	if (dateInfo.tm_wday == 0) {
		cout << "           " << endl;
	}
	else {
		cout << setw(setlength * dateInfo.tm_wday) << " ";
	}
	int row_num = -1;
	for (int j = 1; j < month_days + 1; ++j) {

		cout << setw(setlength) << j;

		if (ff % weekdayNum == sei) {
			++row_num;
			cout << endl;
			size_t max_size = 0;
			if (row_num == 0) {
				for (int p = 0; p < j;++p) {
					if (children[p]->children.size() > max_size)
					{
						max_size = children[p]->children.size();
					}
				}
				if (max_size != 0) {
					if (dateInfo.tm_wday == 0) {
						cout << "           ";
					}
					else {
						cout << setw(setlengthlonger * dateInfo.tm_wday) << "      ";
					}
				}
				for (int y = 0; y < max_size;++y) {
					for (int h = 0; h < j; ++h) {
						if (y == 0) {
							continue;
						}
						if(y < (children[h]->children).size())
						{
							if (children[h]->children[y] != nullptr) {
								cout << setw(setlength);
								children[h]->children[y]->display();
							}
							else {
								cout << setw(setlength) << " ";
							}
						}
						else {
							cout << setw(setlength) << " "; }
					}
					cout << endl;
				}
				cout << endl;
			}
			else if (row_num > 0) {
				for (int p = j - sei; p < j+1;++p) {
					if (children[p-1]->children.size() > max_size)
					{
						max_size = children[p-1]->children.size();
					}
				}
				for (int y = 0; y < max_size;++y) {
					for (int h = j-sei; h < j+1; ++h) {
						if (y == 0) {
							continue;
						}
						if (y < (children[h-1]->children).size())
						{						
							if (children[h - 1]->children[y] != nullptr) {
								cout << setw(setlength);
								children[h - 1]->children[y]->display();
							}
							else {
								cout << setw(setlength) << " ";
							}
						
						}
						else {
							cout << setw(setlength) << " ";
						}
					}
					cout << endl;
				}

			}
			cout << endl;
		}
		else if (j == month_days) {
			++row_num;
			cout << endl;
			size_t max_size = 0;
			for (int p = j - sei; p < j + 1;++p) {
				if (children[p - 1]->children.size() > max_size)
				{
					max_size = children[p - 1]->children.size();
				}
			}
			for (int y = 0; y < max_size;++y) {
				for (int h = j - last_w; h < j + 1; ++h) {
					if (y == 0) {
						continue;
					}
					if (y < (children[h - 1]->children).size())
					{


						if (children[h - 1]->children[y] != nullptr) {
							Event* temp1 = dynamic_cast<Event*> (children[h - 1]->children[y].get());
							cout << setw(setlength);
							children[h - 1]->children[y]->display();
						}
						else {
							cout << setw(setlength) << " ";
						}

					}
					else {
						cout << setw(setlength) << " ";
					}
				}
				cout << endl;

			}
		}
		
		++ff;


	}
cout << endl;
	
}
else {
	cout << "\t\t" << name << ":" << endl;
	cout << setw(setlength) << "Sunday" << setw(setlength) << "Monday" << setw(setlength) << "Tuesday" << setw(setlength) << "Wednesday" << setw(setlength) << "Thursday" << setw(setlength) << "Friday" << setw(setlength) << "Saturday" << endl;
	int ff = dateInfo.tm_wday;
	cout << ff << endl;
	int month_days = CalendarComponent::days[dateInfo.tm_mon];
	if (dateInfo.tm_wday == 0) {
		cout << "           " << endl;
	}
	else {
		cout << setw(setlength * dateInfo.tm_wday) << " ";
	}
	int row_num = -1;
	for (int j = 1; j < month_days + 1; ++j) {

		cout << setw(setlength) << j;

		if (ff % weekdayNum == sei) {
			++row_num;
			cout << endl;
		}
		++ff;


	}
	cout << endl;
}

}

shared_ptr<DisplayableComponent> DisplayableMonth::addComponent(shared_ptr<DisplayableComponent> comp) {
	// try to dynamically cast comp to a pointer to a DisplayableDay, will fail if the DisplayableComponent is not a day
	DisplayableDay* d = dynamic_cast<DisplayableDay *>(comp.get()); // can only cast regular pointers, not shared_ptrs
	if (d == nullptr) { // the cast failed
		return nullptr;
	}
	// otherwise, add the day to the correct location
	int dayOfMonth = d->dateInfo.tm_mday - 1;
	if (children[dayOfMonth] == nullptr) { // day does not already exist
		children[dayOfMonth] = comp;
		return comp;
	}
	else {  // day already exist, return existing day
		return children[dayOfMonth];
	}
}

// Year
DisplayableYear::DisplayableYear(std::tm d, std::shared_ptr<DisplayableComponent> p, bool l) : CalendarComponent(d, p), leap(l) {
	for (size_t i = 0; i < CalendarComponent::MONTHS; ++i) { 
		children.push_back(nullptr);
	}
}

void DisplayableYear::display() {
	cout << "\tYear " << dateInfo.tm_year + CalendarComponent::BASEYEAR << ":" << endl;
	tm temp;
	
	for (int i = 0; i < monthNum; ++i) {
		cout << CalendarComponent::months[i] << endl;
		cout << "  S   M   T   W   T   F   S" << endl;
		temp.tm_year = dateInfo.tm_year;
		temp.tm_mon = i;
		temp.tm_mday = 1; 
		temp.tm_hour = 0;
		temp.tm_yday = 0;
		for (int f = 0; f < i ; ++f) {
			temp.tm_yday = temp.tm_yday+CalendarComponent::days[f];
		}
		temp.tm_wday = (dateInfo.tm_wday +  temp.tm_yday) % CalendarComponent::DAYSINAWEEK;
		cout << temp.tm_wday << endl;
		int ff = temp.tm_wday;
		int month_days = CalendarComponent::days[i];
		if (temp.tm_wday == 0) {
			cout << "  " << endl;
		}
		else {
			cout << setw(cua * temp.tm_wday) << " ";
		}
		for (int j = 1; j < month_days + 1; ++j) {
			cout << setw(setsmallerlength) << j << " ";
			if (ff % weekdayNum == sei)
			{
				cout << endl;
			}
			++ff;
		}
		cout << endl;
		cout << endl;

		
	}
}
shared_ptr<DisplayableComponent> DisplayableYear::addComponent(shared_ptr<DisplayableComponent> comp) {
	// try to dynamically cast comp to a pointer to a DisplayableMonth
	DisplayableMonth * m = dynamic_cast<DisplayableMonth *>(comp.get());
	if (m == nullptr) { // if the cast fails, return nullptr
		return nullptr;
	}
	// otherwise, add the month to the correct location
	int monthOfYear = m->dateInfo.tm_mon;
	if (children[monthOfYear] == nullptr) { // month does not already exist
		children[monthOfYear] = comp;
		return comp;
	}
	else {  // month already exist, return existing month
		return children[monthOfYear];
	}
}

// Calendar
Calendar::Calendar(string n, size_t y) : CalendarComponent(tm(), nullptr), name(n), yearsToHold(y) { // just initialize with a default tm for now.
	time_t now = time(0); // get the current time
	tm now_tm;
	gmtime_s(&now_tm, &now); // create a struct tm(now_tm) from the current time
	currentDate = now_tm;    // set Calendar's date and time to now
	dateInfo = now_tm; // setup dateInfo to represent January 1st of the current year, start time of the calendar
	dateInfo.tm_sec = 0;
	dateInfo.tm_min = 0;
	dateInfo.tm_hour = 0;
	dateInfo.tm_mday = 1;
	dateInfo.tm_mon = 0;
	// calculate and set day of the week to that of January 1st, 2018. Very sloppy, I know
	dateInfo.tm_wday = (now_tm.tm_wday + CalendarComponent::DAYSINAWEEK - (now_tm.tm_yday % CalendarComponent::DAYSINAWEEK)) % CalendarComponent::DAYSINAWEEK; 
	dateInfo.tm_yday = 0;
	dateInfo.tm_isdst = 0;
	// intialize calendar to hold __ years
	for (size_t i = 0; i < yearsToHold; ++i) {
		children.push_back(nullptr);
	}
}

void Calendar::display() {
	cout << "Calendar: " << name << endl;
	for (int i = 0; i < children.size(); ++i)
	{
		cout << dateInfo.tm_year + CalendarComponent::BASEYEAR + i << endl;
	}
}

shared_ptr<DisplayableComponent> Calendar::addComponent(std::shared_ptr<DisplayableComponent> comp) {
	DisplayableYear* y = dynamic_cast<DisplayableYear *>(comp.get());
	if (y == nullptr) { // if the cast fails, return nullptr
		return nullptr;
	}
	int calendarYear = CalendarComponent::BASEYEAR + dateInfo.tm_year;
	int yearAdding = CalendarComponent::BASEYEAR + y->dateInfo.tm_year;
	int index = yearAdding - calendarYear; 
	if (index >= 0 && index < children.size() && children[index] == nullptr) {
		children[index] = comp;
		return comp;
	}
	else {
		return nullptr;
	}
}





void mergedecorator::display()
{
	string name = cal_name;
	string newName = name.replace((name.size() - 4), 4, "");
	if (event_time < onedig) {
		cout << "0" + to_string(event_time * cien) + " " +newName + ":" + event_n;
	}
	else  {
		cout << to_string(event_time * cien)  + " " +newName + ":" + event_n;
	}
}

mergedecorator::mergedecorator(string n, shared_ptr<DisplayableComponent> e):DisplayableComponent(e),cal_name(n)
{
	Event* ee = dynamic_cast<Event*>(e.get());
	if (ee != nullptr) {
		event_n = ee->event_name;
		event_time = ee->date_time.tm_hour;
	}
}


TodoList::TodoList(std::tm t, shared_ptr<DisplayableComponent> p) : CalendarComponent(t,p) , list_time(t) {}


shared_ptr<DisplayableComponent> TodoList::addComponent(shared_ptr<DisplayableComponent> task_to_add)
{


		children.push_back(task_to_add);
		return task_to_add;
	

}

void TodoList::display()
{	
		cout << "ToDoLIST";

		for (int i = 0; i < children.size(); ++i) {
			if (children[i] != nullptr) {
				cout << endl;
				children[i]->display();

			}
		}
		
}

class Task;
shared_ptr<DisplayableComponent> TodoList::MarkComplete(string completed)
{
	
	for (unsigned int i = 0; i < children.size(); ++i) {

		Task* task_to_mark = dynamic_cast<Task*>(getChild(i).get());
		if (task_to_mark != nullptr) {
			if (task_to_mark->taskName == completed) {
				
				task_to_mark->isCompleted = true;
				Task::status = "Completed";
			}
			else {
				cout << "Could not find the task to mark" << endl;
			}

		}
	}
	return children[0];
}



Task::Task(std::string s, std::tm t, shared_ptr<DisplayableComponent> p) : CalendarComponent(t, p),taskName(s), task_time(t),  isCompleted(false) {}


void Task::display()
{

	cout << taskName << " "<< status << endl;
}
