#pragma once
#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include "Note.h"
class CListNote
{
public:
	vector<CNote> listNotes;
public:
	CListNote();	
	void docDSNoteTuFile(string pathlist);
	void ghiDSNoteVaoFile(string pathlist);
	void themNote(CNote);
	void xoaNote(int pos);
};

