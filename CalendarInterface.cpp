#include "stdafx.h"
#include "CalendarInterface.h"
#include "displayablecomponent.h"
#include "Enum.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <iterator>

using namespace std;
static int kk = 0;
string CalendarInterface::current_file = " ";

int CalendarInterface::depth = 0;

tm CalendarInterface::d = { 0 };
tm CalendarInterface::y = { 0 };
tm CalendarInterface::m = { 0 };


CalendarInterface::CalendarInterface(std::string builderType, size_t years) : builder(nullptr),cal(nullptr),currentDisplay(nullptr) {
	if (builderType == "full") {
		builder = make_shared<FullCalendarBuilder>();
		cal = builder->buildCalendar("test", years);
		currentDisplay = cal;
	}

	else {
		builder = make_shared<IncrementalCalendarBuilder>();
		cal = builder->buildCalendar("test", years);
		currentDisplay = cal;
	}

}

void CalendarInterface::display() {
	currentDisplay->display();
	DisplayableDay* d = dynamic_cast<DisplayableDay *>(currentDisplay.get());
	if (DisplayableComponent::full == true) {
		cout << endl;
		cout << endl;
		cout << "View: zoom out: (out) , zoom in: (in) " << endl;
		cout << "Quit: (q) " << endl;
		cout << "Function: add event: (ae) ,   search event : (s) ,   jump to a specific date : (jump) " << endl;
		cout << "File: save file : (save), load file : (load)" << endl;
		cout << "Edit: merge file : (merge), edit : (edit), add todolist : (tdl)" << endl;
		cout << "Help: ONLY in days: you can choose to edit and add todolist" << endl;
		cout << endl;
		cout << endl;
	}
	else {
		cout << "zoom out: out, zoom in: in, quit: q, jump to a specific date: jump" << endl;
	}
	string in;
	cin >> in;
	if (in == "in") {
		cout << "index? (0-" << currentDisplay->children.size() - 1 << ")" << endl;


		int index = 0;
		cin >> index;
		zoomIn(index);
	}
	else if (in == "out") {
		zoomOut();
	}
	else if (in == "q") {
		return;
	}
	else if (in == "ae") {
		cout << "enter an event name" << endl;
		string name;
		cin >> name;
		tm time;
		int year, month, monthday, hour;
		cout << "enter the event time with year, month, day, hour order" << endl;
		cin >> year >> month >> monthday >> hour;

		time.tm_year = (year - CalendarComponent::BASEYEAR);
		if (time.tm_year < cal->dateInfo.tm_year)
		{
			cout << "event can not be set for previous years" << endl;
			return;
		}
		if (time.tm_year > ((*cal).children.size() + cal->currentDate.tm_year))
		{
			cout << "this year is not included in this calendar" << endl;
			return;
		}
		time.tm_mon = month - 1;//0-11
		time.tm_mday = monthday;
		time.tm_yday = 0;
		for (int i = 0; i < month - 1; ++i) {
			time.tm_yday = CalendarComponent::days[i] + time.tm_yday;
		}
		time.tm_yday = time.tm_yday + monthday;
		time.tm_wday = ((*cal).dateInfo.tm_wday + (time.tm_year - (*cal).dateInfo.tm_wday)*CalendarComponent::DAYSINAYEAR + time.tm_yday) % CalendarComponent::DAYSINAWEEK;
		time.tm_hour = hour;
		time.tm_isdst = 0;
		time.tm_min = 0;
		time.tm_sec = 0;


		/////recur
		int recur_every;
		cout << "enter how ofen the event should be in days(recur every ? days)" << endl;
		cin >> recur_every;
		int recur_for;
		cout << "enter how long the event should be in days(event last for ? days)" << endl;
		cin >> recur_for;

		time_t base_time = mktime(&time);


		for (int i = 0; i * recur_every < recur_for; i++)
		{

			const time_t interval_time = base_time + i * recur_every * hourNum * minNum * secNum;
		
			const time_t * temp = &interval_time;
			tm recurred_time;
			gmtime_s(&recurred_time, &interval_time);
			

			shared_ptr<DisplayableComponent> y1 = (*cal).children[(recurred_time).tm_year - (*cal).currentDate.tm_year];
			auto m1 = y1->children[(recurred_time).tm_mon];
			auto d1 = m1->children[(recurred_time).tm_mday -1];

			d1->addComponent((*builder).buildEvent(d1, name, (recurred_time), recur_every, recur_for));

			shared_ptr<DisplayableComponent> a = (*builder).buildEvent(d1, name, (recurred_time), recur_every, recur_for);
			
			
			
			
			pair <string, pair<tm, shared_ptr<DisplayableComponent> > > ss;
			pair<tm, shared_ptr<DisplayableComponent> > sss = make_pair((recurred_time), a);
			ss = make_pair(name, sss);
			event_store.insert(ss);


		}
		kk++;
		display();
	}
	else if (in == "s") {
		cout << "enter the event name" << endl;
		string name_;
		cin >> name_;
		if (event_store.find(name_) != event_store.end())
		{
	
				cout << "search result: " << endl;
				pair <multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator, multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator> ret;	
				ret = event_store.equal_range(name_);
				for (multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator it = ret.first; it != ret.second; ++it) {
					cout << name_ << " Date: " << CalendarComponent::BASEYEAR + it->second.first.tm_year << " " << it->second.first.tm_mon + 1 << " " << it->second.first.tm_mday << endl;
			}
		}
		else
		{
			cout << "this event does not exist" << endl;
		}
		display();
	}
	else if (in == "jump") {
		if (DisplayableComponent::full == true) {
			cout << "enter the event time (Month Date Year) and granularity (year month date)" << endl;
			string input;
			
			getline(cin, input);
			while (input == "") {
				getline(cin, input);
			}

			istringstream iss(input);

			int month;
			int day;
			int year;
			string granularity;


			if (iss >> month && iss >> day && iss >> year && iss >> granularity) {
				cout << month << " " << day << " " << year << " " << granularity << endl;
				cout << validDate(year, month, day) << endl;
				if (validDate(year, month, day) == false) {
					cout << "Invalid date " << endl;
				}
				else if (granularity == "year") {
					cout << "child: " << year - CalendarComponent::BASEYEAR << endl;
					currentDisplay = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year);
				}
				else if (granularity == "month") {
				
					currentDisplay = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year)->getChild(month - 1);
					
				}
				else if (granularity == "day") {
					currentDisplay = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year)->getChild(month - 1)->getChild(day - 1);
				}
				else {
					cout << "invalid granularity format " << endl;
				}
			
				display();
			}
			else {
				cout << "Invalid input to jump to " << endl;
			}
		}
		else {
			cout << "enter the event time (Month Date Year) and granularity (year month date)" << endl;
			string input;
			
			getline(cin, input);
			while (input == "") {
				getline(cin, input);
			}
			istringstream iss(input);
			int month;
			int day;
			int year;
			string granularity;

			if (iss >> month && iss >> day && iss >> year && iss >> granularity) {
				cout << month << " " << day << " " << year << " " << granularity << endl;
				tm dd;
				tm mm;
				tm yy;
				tm firstday = cal->dateInfo;
				yy = firstday;
				yy.tm_year = year - CalendarComponent::BASEYEAR;
				yy.tm_wday = (firstday.tm_wday + ((yy.tm_year - firstday.tm_year)*yearNum) % weekdayNum) % weekdayNum;
				mm.tm_year = yy.tm_year;
				mm.tm_mon = month - 1;
				mm.tm_mday = 1;
				mm.tm_hour = 0;
				mm.tm_isdst = 0;
				mm.tm_min = 0;
				mm.tm_sec = 0;
				mm.tm_yday = 0;
				for (int i = 0; i < month - 1; ++i) {
					mm.tm_yday = mm.tm_yday + CalendarComponent::days[i];
				}
				mm.tm_wday = (yy.tm_wday + mm.tm_yday % weekdayNum ) % weekdayNum;


				dd.tm_year = yy.tm_year;
				dd.tm_mon = month - 1;
				dd.tm_mday = day;
				dd.tm_hour = 0;
				dd.tm_isdst = 0;
				dd.tm_min = 0;
				dd.tm_sec = 0;
				dd.tm_yday = mm.tm_year + day - 1;
				dd.tm_wday = (mm.tm_wday + (day-1) % weekdayNum) % weekdayNum;

				if (validDate(year, month, day) == false) {
					cout << "Invalid date " << endl;
				}
				else if (granularity == "year") {


					shared_ptr<DisplayableComponent> temp = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year);
					if (temp != nullptr) {
						currentDisplay = temp;
					}
					else {
						temp = builder->buildYear(yy, currentDisplay);
						currentDisplay = temp;

					}
				}
				else if (granularity == "month") {
					shared_ptr<DisplayableComponent> temp = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year);
					if (temp != nullptr) {
						currentDisplay = temp;
						shared_ptr<DisplayableComponent> temp2 = temp->getChild(month - 1);
						if (temp2 != nullptr) {
							currentDisplay = temp2;
						}
						else {
							temp2 = builder->buildMonth(mm, currentDisplay);
							currentDisplay = temp2;
						}

					}
					if (temp == nullptr) {
						temp = builder->buildYear(yy, currentDisplay);
						currentDisplay = temp;
						shared_ptr<DisplayableComponent> temp2 = temp->getChild(month - 1);
						temp2 = builder->buildMonth(mm, currentDisplay);
						currentDisplay = temp2;

					}
				}

				else if (granularity == "day") {

					shared_ptr<DisplayableComponent> temp = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year);

					if (temp != nullptr) {

						currentDisplay = temp;
						shared_ptr<DisplayableComponent> temp2 = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year)->getChild(month - 1);
						if (temp2 != nullptr) {
							currentDisplay = temp2;
							shared_ptr<DisplayableComponent> temp3 = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year)->getChild(month - 1)->getChild(day - 1);
							if (temp3 != nullptr) {
								currentDisplay = temp3;
							}
							else {
								temp3 = builder->buildDay(dd, currentDisplay);
								currentDisplay = temp3;
							}
						}
						else {

							shared_ptr<DisplayableComponent> temp2 = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year)->getChild(month - 1);
							temp2 = builder->buildMonth(mm, currentDisplay);
							currentDisplay = temp2;
							shared_ptr<DisplayableComponent> temp3 = cal->getChild(year - CalendarComponent::BASEYEAR - (*cal).dateInfo.tm_year)->getChild(month - 1)->getChild(day - 1);
							temp3 = builder->buildDay(dd, currentDisplay);
							currentDisplay = temp3;
						}

					}
					else {

						temp = builder->buildYear(yy, currentDisplay);
						currentDisplay = temp;
						shared_ptr<DisplayableComponent> temp2 = temp->getChild(month - 1);
						temp2 = builder->buildMonth(mm, currentDisplay);
						currentDisplay = temp2;
						shared_ptr<DisplayableComponent> temp3 = temp2->getChild(day - 1);
						temp3 = builder->buildDay(dd, currentDisplay);
						currentDisplay = temp3;


					}
				}

				else {
					cout << "invalid granularity format " << endl;
				}
				display();
			}


			else {
				cout << "Invalid input to jump to " << endl;
			}

		}
		

	}
	else if (in == "save") {
		cout << "enter the name for your file" << endl;
		string name_file;
		cin >> name_file;
		ofstream ofs(name_file);

		ofs << cal->name << endl;
		ofs << cal->children.size() << endl;

		for (auto it = event_store.begin(); it != event_store.end(); ++it) {
			ofs << it->first << " " << it->second.first.tm_year << " " << it->second.first.tm_mon << " " << it->second.first.tm_mday << " " << it->second.first.tm_hour << endl;
		}
		display();
	}
	else if (in == "load") {
		cout << "enter the file name to load" << endl;
		string file_name;
		cin >> file_name;
		ifstream ifs(file_name);
		if (ifs.is_open()) {
			string line;
			int year_interval;
			ifs >> line;
			ifs >> year_interval;		
			builder = make_shared<FullCalendarBuilder>();
			cal = builder->buildCalendar(line, year_interval);
			currentDisplay = cal;
			event_store.clear();
			string event_info;
			while (getline(ifs,event_info))
			{
				istringstream iss(event_info);
				
				string e_name;
				int e_year;
				int e_mon;
				int e_day;
				int e_hour;
				tm e_tm;
				if ((!(iss >> e_name))) { continue; }
				iss >> e_year;
				iss >> e_mon;
				iss >> e_day;
				iss >> e_hour;
				
				e_tm.tm_mon = e_mon;//0-11
				e_tm.tm_mday = e_day;
				e_tm.tm_year = e_year;
				e_tm.tm_hour = e_hour;
				mktime(&e_tm);
				int recur_every = 1;
				int recur_for = 1;

				shared_ptr<DisplayableComponent> ee = (*builder).buildEvent(((*(*(*cal).children[e_tm.tm_year - (*cal).currentDate.tm_year]).children[e_tm.tm_mon]).children[e_tm.tm_mday - 1]), e_name, e_tm, recur_every, recur_for);
				
				(*(*(*(*cal).children[e_tm.tm_year - (*cal).currentDate.tm_year]).children[e_tm.tm_mon]).children[e_tm.tm_mday - 1]).addComponent(ee);
				pair <string, pair<tm, shared_ptr<DisplayableComponent> > > ss;
				pair<tm, shared_ptr<DisplayableComponent> > sss = make_pair(e_tm, ee);
				ss = make_pair(e_name, sss);
				event_store.insert(ss);
			}
		}

		display();
	}
	else if (in == "merge")
	{

		cout << "put in the files' names you want to merge" << endl;
		string file_name;
		cin >> file_name;
			ifstream ifs(file_name);
			if (ifs.is_open()) {
				string line;
				int year_interval;
				ifs >> line;
				ifs >> year_interval;
				string event_info;
				while (getline(ifs, event_info))
				{

					istringstream iss(event_info);
					string e_name;
					int e_year;
					int e_mon;
					int e_day;
					int e_hour;
					tm e_tm;
					if ((!(iss >> e_name))) { continue; }
					iss >> e_year;
					iss >> e_mon;
					iss >> e_day;
					iss >> e_hour;

					e_tm.tm_mon = e_mon;//0-11
					e_tm.tm_mday = e_day;
					e_tm.tm_year = e_year;
					e_tm.tm_hour = e_hour;
					mktime(&e_tm);
					int recur_every = 1;
					int recur_for = 1;
					shared_ptr<DisplayableComponent> ee = (*builder).buildEvent(((*(*(*cal).children[e_tm.tm_year - (*cal).currentDate.tm_year]).children[e_tm.tm_mon]).children[e_tm.tm_mday - 1]), e_name, e_tm, recur_every, recur_for);
					shared_ptr<DisplayableComponent> ff = make_shared<mergedecorator>(file_name, ee);
					(*(*(*(*cal).children[e_tm.tm_year - (*cal).currentDate.tm_year]).children[e_tm.tm_mon]).children[e_tm.tm_mday - 1]).addComponent(ff);

					//cout << (*(*(*(*cal).children[e_tm.tm_year - (*cal).currentDate.tm_year]).children[e_tm.tm_mon]).children[e_tm.tm_mday - 1]).children.size() << endl;
					pair <string, pair<tm, shared_ptr<DisplayableComponent> > > ss;
					pair<tm, shared_ptr<DisplayableComponent> > sss = make_pair(e_tm, ee);
					ss = make_pair(e_name, sss);
					event_store.insert(ss);

				}
			
		}
		
		
		display();
	}


	
	/////Edit the event 
	else if (in == "edit") {
		if (d != nullptr) {
			cout << "	Enter Event: (name year month day hour) " << endl;
			string eName;
			int year;
			int month;
			int day;
			int hour;
			cin >> eName;
			cin >> year;
			cin >> month;
			cin >> day;
			cin >> hour;

			cout << "and Change Date(change) / Delete Event ? (delete)" << endl;
			string type;
			cin >> type;
			
			if (type == "change") {
				for (unsigned int i = 1; i < d->children.size(); ++i) {

					Event* event_to_change = dynamic_cast<Event*>(d->getChild(i).get());

					pair <multimap <string, pair<tm, shared_ptr<DisplayableComponent> >   >::iterator, multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator> ret;

					ret = event_store.equal_range(eName);
					multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator it = ret.first;
					if (ret.first == ret.second) {
						cout << "this event does not exist" << endl;
						display();
					}
					else {
						
						for (multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator it = ret.first; it != ret.second; ++it) {
							
							if (it->second.first.tm_year  == event_to_change->dateInfo.tm_year) {

								if (it->second.first.tm_mon  == event_to_change->dateInfo.tm_mon) {
									if (it->second.first.tm_mday == event_to_change->dateInfo.tm_mday) {
										if (it->second.first.tm_hour == event_to_change->dateInfo.tm_hour) {
											
											currentDisplay->removeComponent(i);

											event_store.erase(it);
											break;
										}
									}
								}
							}
						}
					}
					}
				cout << "enter an new event name" << endl;
				string name;
				cin >> name;
				tm time;
				int year, month, monthday, hour;
				cout << "enter the event time with year, month, day, hour order" << endl;
				cin >> year >> month >> monthday >> hour;

				time.tm_year = (year - CalendarComponent::BASEYEAR);
				if (time.tm_year < cal->dateInfo.tm_year)
				{
					cout << "event can not be set for previous years" << endl;
					return;
				}
				if (time.tm_year >((*cal).children.size() + cal->currentDate.tm_year))
				{
					cout << "this year is not included in this calendar" << endl;
					return;
				}
				time.tm_mon = month - 1;//0-11
				time.tm_mday = monthday;
				time.tm_yday = 0;
				for (int i = 0; i < month - 1; ++i) {
					time.tm_yday = CalendarComponent::days[i] + time.tm_yday;
				}
				time.tm_yday = time.tm_yday + monthday;
				time.tm_wday = ((*cal).dateInfo.tm_wday + (time.tm_year - (*cal).dateInfo.tm_wday)*CalendarComponent::DAYSINAYEAR + time.tm_yday) % CalendarComponent::DAYSINAWEEK;
				time.tm_hour = hour;
				time.tm_isdst = 0;
				time.tm_min = 0;
				time.tm_sec = 0;


				/////recur
				int recur_every;
				cout << "enter how ofen the event should be in days(recur every ? days)" << endl;
				cin >> recur_every;
				int recur_for;
				cout << "enter how long the event should be in days(event last for ? days)" << endl;
				cin >> recur_for;

				time_t base_time = mktime(&time);
				

				for (int i = 0; i * recur_every < recur_for; i++)
				{

					const time_t interval_time = base_time + i * recur_every * hourNum * minNum * secNum;
					
					const time_t * temp = &interval_time;
					tm recurred_time;
					gmtime_s(&recurred_time, &interval_time);


					shared_ptr<DisplayableComponent> y1 = (*cal).children[(recurred_time).tm_year - (*cal).currentDate.tm_year];
					auto m1 = y1->children[(recurred_time).tm_mon];
					auto d1 = m1->children[(recurred_time).tm_mday - 1];

					d1->addComponent((*builder).buildEvent(d1, name, (recurred_time), recur_every, recur_for));

					shared_ptr<DisplayableComponent> a = (*builder).buildEvent(d1, name, (recurred_time), recur_every, recur_for);

					cout << time.tm_hour << endl;

					pair <string, pair<tm, shared_ptr<DisplayableComponent> > > ss;
					pair<tm, shared_ptr<DisplayableComponent> > sss = make_pair((recurred_time), a);
					ss = make_pair(name, sss);
					event_store.insert(ss);			
				}
				kk++;
				display();
			}
			else if (type == "delete") {
				for (unsigned int i = 1; i < d->children.size(); ++i) {

					Event* event_to_delete = dynamic_cast<Event*>(d->getChild(i).get());
	
					pair <multimap <string, pair<tm, shared_ptr<DisplayableComponent> >   >::iterator, multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator> ret;

					ret = event_store.equal_range(eName);
					multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator it = ret.first;
					for (multimap<string, pair<tm, shared_ptr<DisplayableComponent> >>::iterator it = ret.first; it != ret.second; ++it) {

						if (it->second.first.tm_year == event_to_delete->dateInfo.tm_year) {

							if (it->second.first.tm_mon == event_to_delete->dateInfo.tm_mon) {
								if (it->second.first.tm_mday == event_to_delete->dateInfo.tm_mday) {
									if (it->second.first.tm_hour == event_to_delete->dateInfo.tm_hour) {
										currentDisplay->removeComponent(i);
										event_store.erase(it);
								
										break;
									}
									
								}

							}
						}
					}
					
				}
				display();
			}
			
		}
	else {
				cout << "you cannot change outside a date. Hint: zoom in to a day" << endl;
		}
	}
	else if ( in == "tdl") {
		if (d != nullptr) {
		
			shared_ptr<DisplayableComponent> d_ = (*builder).getComponentByDate(cal, d->dateInfo,"day" );
			if (d->accessToDoList(d->dateInfo, d_)) {
				
				cout << "Todo List not found" << endl;
				cout << "To add a todo list? (a)" << endl;
				string i;
				cin >> i;
				if (i == "a") {
					shared_ptr<DisplayableComponent> todo = make_shared<TodoList>( d->dateInfo, d_);
					d->children[0] = todo;
				}
				else {
					display();
				}
					
			}
			else {
				auto list = d->children[0];
				cout << "Todo List found: " << endl;
			    cout << "to add task: (add)" << endl;
				cout << "to mark completed for a task: (mark)" << endl;
				
				string input;
				cin >> input;
				if (input == "add"){
					cout << "Enter the task name: " << endl;
					string tName;
					cin >> tName;
					//build the task
					shared_ptr<DisplayableComponent> task = make_shared<Task>(tName, d->dateInfo , d->children[0]);
					list->addComponent(task);
				}
				else if (input == "mark") {
					cout << "Enter the task name to mark: " << endl;
					string tName_toMark;
					cin >> tName_toMark;
					TodoList* list_ = dynamic_cast<TodoList*>(list.get());
					list_->MarkComplete(tName_toMark);
					
				}
				else { cout << "invalid todolist input" << endl; }
			}
			display();
		}
		else {
			cout << "you cannot change outside a date. Hint: zoom in to a day" << endl;
		}		
	}
	
}
bool CalendarInterface::validDate(int year, int month, int day) {

	

	if (year < CalendarComponent::BASEYEAR || year > (CalendarComponent:: BASEYEAR +  (*cal).children.size()  +  cal->currentDate.tm_year  -1)) {
		cout << "==Year not accessable " << endl;
		return false;
	}
	else if (month > monthNum || month < 1) {
		cout << "==Month not accessable " << endl;
		return false;
	}
	else if (day < 1 || day > CalendarComponent::days[month-1]) {
		cout << "==Day not accessable " << endl;
		return false;
	}
	else {
		
		return true;
	}
}
void CalendarInterface::zoomIn(unsigned int index) {

	shared_ptr<DisplayableComponent> temp = currentDisplay->getChild(index);
	if (temp != nullptr) {
		currentDisplay = temp;
	}
	if (temp == nullptr) {
		if (CalendarInterface::depth == depth0) {
			cal->dateInfo;

			y = cal->dateInfo;
			y.tm_year = y.tm_year + index;
			y.tm_wday = y.tm_wday + (yearNum * index % weekdayNum);
			temp = builder->buildYear(y, currentDisplay);
			currentDisplay = temp;
			++depth;
		}
		else if (CalendarInterface::depth == depth1) {
			m.tm_year = y.tm_year;
			m.tm_mday = 1;
			m.tm_mon = index;
			int days = 0;
			for (unsigned int i = 0; i < index; ++i) {
				days = days + CalendarComponent::days[i];
			}
			m.tm_yday = days;
			m.tm_hour = 0;
			m.tm_mday = 0;
			m.tm_min = 0;
			m.tm_sec = 0;
			m.tm_isdst = 0;
			m.tm_wday = (y.tm_wday + days % weekdayNum) % weekdayNum;
			temp = builder->buildMonth(m, currentDisplay);
			currentDisplay = temp;
			++depth;
		}
		else if (CalendarInterface::depth == depth2) {
			d.tm_year = m.tm_year;
			d.tm_mon = m.tm_mon;
			d.tm_yday = m.tm_yday + index;
			d.tm_hour = m.tm_hour;
			d.tm_isdst = m.tm_isdst;
			d.tm_min = m.tm_min;
			d.tm_mday = index + 1;
			d.tm_sec = m.tm_sec;
			d.tm_wday = (m.tm_wday + index % weekdayNum) % weekdayNum;
			temp = builder->buildDay(d, currentDisplay);
			currentDisplay = temp;
			++depth;
		}

	}
	display();
}
void CalendarInterface::zoomOut() {
	if (currentDisplay->getParent().lock() != nullptr) {
		currentDisplay = currentDisplay->getParent().lock();
	}
	display();
}