#include "myStringFunc.h"
#include <iostream>

using namespace std;

myStringFunc::myStringFunc()
{
}



int myStringFunc::stringSearch(const string stringToSearch, const char stopChar)
{
	
	if (stringToSearch[stringToSearch.size() - 1] != stopChar) {
		cout << "failed to search string" << endl;
		return -1;
	}

	string stringBuf1 = stringToSearch;
	string stringBuf2 = stringToSearch;

	while (stringBuf2.size() != 0) {
		pos_ = stringBuf2.find(stopChar);
		stringBuf1 = stringBuf2.substr(0, pos_);
		cout << "BUF: " << stringBuf1 << endl;
		subtrackedStrings_.push_back(stringBuf1);
		stringBuf2 = stringBuf2.substr(pos_);
		stringBuf2.erase(0, 1);
		
		stopCharsCount_++;
		
	}
	return 0;
}


void myStringFunc::removeSubtrackedStrings()
{
	for (int i = 0; i < getCount(); i++) {
		subtrackedStrings_.pop_back();
	}
	stopCharsCount_ = 0;
}


const int myStringFunc::getCount()
{
	return stopCharsCount_;
}
