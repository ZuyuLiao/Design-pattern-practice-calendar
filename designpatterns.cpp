// Calendar.cpp : Defines the entry point for the console application.
//

// You should only edit this file to change the builder type if you complete that functionality,
// or the number of years
#include "stdafx.h"
#include "CalendarInterface.h"
#include "Enum.h"
#include<iostream>
using namespace std;


int main()
{
	cout << "Do you want a full calendar or an incremental calendar? Enter full or incremental." << endl;
	string in;
	cin >> in;
	if (in == "full") {
		CalendarInterface c("full", 3);
		c.display();
		return success;
	}
	else if (in == "q") {
		return quit;
	}
	else {
		CalendarInterface c("incremental", 3);
		c.display();
		return success;
	}
}

