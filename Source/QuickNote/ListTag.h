#pragma once
#include "Tag.h"
#include "Note.h"
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
class CListTag
{
public:
	vector<CTag> listTags;
public:
	CListTag();
	void docDSTagTuFile(string filename);
	void ghiDSTagVaoFile(string filename);
	void themTag(CTag tag);
	void xoaTag(int pos);
	void findNewTags(CNote);
	void eraseTags(CNote oldNote);
	void increaseCountnotes(CNote note);
	bool isTrueList();
	vector<CTag> DecreaseListTags();
	int totalNotes();
};
