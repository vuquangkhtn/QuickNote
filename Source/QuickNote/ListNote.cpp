#include "stdafx.h"
#include "ListNote.h"


CListNote::CListNote()
{
	listNotes.clear();
}

void CListNote::docDSNoteTuFile(string pathlist)
{
	fstream fin;
	fin.open(pathlist,ios::in || ios::out);

	while(!fin.eof())
	{
		fflush(stdin);
		string filename;
		getline(fin,filename);
		if(filename == "")
			continue;
		CNote note(filename);
		themNote(note);
	}
	fin.close();
}

void CListNote::ghiDSNoteVaoFile(string pathlist)
{
	fstream fout, ffile;
	fout.open(pathlist,ios::out);
	for(int i=0;i<listNotes.size();i++)
	{
		listNotes[i].ghiNoteVaoFile(pathlist);

	}
	fout.close();

}

void CListNote::themNote(CNote note)
{
	listNotes.push_back(note);
}

void CListNote::xoaNote(int pos)
{
	listNotes.erase(listNotes.begin() + pos);
}
