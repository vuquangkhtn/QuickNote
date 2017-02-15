#pragma once
#include <string>
using namespace std;
class CTag
{
public:
	wstring name;
	int countNotes;
public:
	CTag();
	CTag(wstring name, int countNotes);
};

