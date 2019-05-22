//Tobias Bach Aunbøl - Playground Marketing

#include <string>
#include <list>
#pragma once

//

using namespace std;


class myStringFunc {
public:
	
	myStringFunc();


	int stringSearch(const string stringToSearch, const char stopChar);
	void removeSubtrackedStrings();
	const int getCount();
	list<string> subtrackedStrings_;

private:
	
	int pos_ = 0;
	int stopCharsCount_ = 0;
};