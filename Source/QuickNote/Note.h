#pragma once
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include "Tag.h"
using namespace std;
class CNote
{
public:
	vector<CTag> listTags;
	wstring content;
	string path;
public:
	CNote();
	CNote(string filename);
	CNote createNote(wstring tags, wstring content);
	void getTags(wstring tags);
	void docNoteTuFile(string filename);
	void ghiNoteVaoFile(string pathlist);
	bool isTrueList();
	wstring getStrTags();
};
