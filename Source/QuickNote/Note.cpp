#include "stdafx.h"
#include "Note.h"

CNote::CNote()
{
	content = L"";
	listTags.clear();
	path = "";
}


CNote::CNote(string filename)
{
	docNoteTuFile("notes\\"+filename);
}

void CNote::getTags(wstring tags)
{
	this->listTags.clear();
	int dem = 0;
	for(int i=0;i<tags.length();i++)
	{
		if(tags[i] == ',')
		{
			CTag tag;
			tag.name = tags.substr(dem,i-dem);
			tag.countNotes = 1;
			dem = i+1;
			this->listTags.push_back(tag);
		}

	}
	this->listTags.push_back(CTag(tags.substr(dem,tags.length()-dem),1));
}

CNote CNote::createNote(wstring tags, wstring content)
{
	CNote note;
	int dem = 0;
	for(int i=0;i<tags.length();i++)
	{
		if(tags[i] == ',')
		{
			CTag tag;
			tag.name = tags.substr(dem,i-dem);
			tag.countNotes = 1;
			dem = i+1;
			note.listTags.push_back(tag);
		}

	}
	note.listTags.push_back(CTag(tags.substr(dem,tags.length()-dem),1));
	note.content = content;
	note.path = "";
	return note;
}


void CNote::docNoteTuFile(string filename)
{
	wfstream fin;
	fin.open(filename,ios::in || ios::out);
    std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
    fin.imbue(loc);
	while(!fin.eof())
	{
		fflush(stdin);
		wstring tags;
		wstring path;
		wstring temp;
		wstring content = L"";
		//bỏ dòng đầu
		getline(fin,path);
		getline(fin,tags);
		if(tags == L"")
			continue;
		while(!fin.eof())
		{
			getline(fin,temp);
			content.append(temp);
		}
		*this = createNote(tags,content);
		this->path = filename.erase(0,6);
		break;
	}
	fin.close();
}

//Tao va ghi file text va luu ten file vao ds
void CNote::ghiNoteVaoFile(string pathlist)
{
	SYSTEMTIME time;
	bool isExistedName = true;
	CHAR* pathitem = new CHAR[50];
	GetLocalTime(&time);
	//Dat ten cho file note nếu chưa có
	sprintf_s(pathitem,50,"Note_%d-%d-%d_%dh%dm%ds.txt",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
	//Ktra nếu tên file đã tồn tại hay chưa (EditNote)
	if(this->path == "")
	{
		isExistedName = false;
		this->path = pathitem;
	}
	wfstream fout;
	fstream sout;
	//Tao va ghi file text
	sout.open("notes\\"+this->path,ios::out);
	sout << this->path <<endl;
	sout.close();
	fout.open("notes\\"+this->path,ios::app);
    std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
    fout.imbue(loc);
	int count = 0;
	while(count < listTags.size()-1)
	{
		CTag tag = listTags[count];
		fout << tag.name <<",";
		count++;
	}
	if(count == listTags.size()-1)
	{
		CTag tag = listTags[count];
		fout << tag.name;
		fout << endl;
	}
	fout << content << endl;
	fout.close();

	//Ghi ten file vao list
	if(!isExistedName)
	{
		fout.open(pathlist,ios::app);
		fout << pathitem << endl;
		fout.close();
	}

}


bool CNote::isTrueList()
{
	for(int i=0;i<listTags.size()-1;i++)
	{
		if(listTags[i].name == L"" || listTags[i].countNotes == 0)
			return false;
		for(int j=i+1;j<listTags.size();j++)
		{
			if(listTags[i].name == listTags[j].name)
			{
				return false;
			}

		}
	}
	if(listTags[listTags.size()-1].name == L"" || listTags[listTags.size()-1].countNotes == 0)
	{
		return false;
	}
	return true;
}


wstring CNote::getStrTags()
{
	wstring temp = L"";
	for(int i=0;i<listTags.size()-1;i++)
	{
		temp += listTags[i].name + L",";
	}
	temp += listTags[listTags.size()-1].name;
	return temp;
}
