#include "stdafx.h"
#include "ListTag.h"


CListTag::CListTag()
{
	listTags.clear();
}


void CListTag::docDSTagTuFile(string path)
{
	wfstream fin;
	fin.open(path,ios::in || ios::out);
    std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
    fin.imbue(loc);
	listTags.clear();
	while(!fin.eof())
	{
		fflush(stdin);
		CTag tag;
		wstring data;
		wstring temp;
		getline(fin,data);
		if(data == L"")
			continue;
		int pos1 = data.find_first_of(',');
		temp = data.substr(0,pos1);
		tag.name = temp;
		temp = data.substr(pos1+1);
		tag.countNotes = _wtoi(temp.c_str());
		themTag(tag);
	}
	fin.close();
}

void CListTag::ghiDSTagVaoFile(string path)
{
	
	wfstream fout;
	fout.open(path,ios::out);
    std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
    fout.imbue(loc);
	int count = 0;
	while(count < listTags.size()-1 && listTags.size() > 0)
	{
		CTag tag = listTags[count];
		if(tag.name == L"" || tag.countNotes == 0)
		{
			count++;
			continue;
		}
		fout << tag.name<<L"," <<tag.countNotes;
		fout << endl;
		count++;
	}
	if(count == listTags.size()-1)
	{
		CTag tag = listTags[count];
		fout << tag.name<<L"," <<tag.countNotes;
	}

	fout.close();
}

void CListTag::themTag(CTag tag)
{
	listTags.push_back(tag);
}

void CListTag::xoaTag(int pos)
{
	listTags.erase(listTags.begin()+pos);
}

//thêm tag mới và tăng countnote
void CListTag::findNewTags(CNote note)
{
	for(int i=0;i<note.listTags.size();i++)
	{
		int j=0;
		for(j;j<this->listTags.size();j++)
		{

			if(listTags[j].name == note.listTags[i].name)
			{
				listTags[j].countNotes++;
				break;
			}

		}
		if(j == listTags.size())
		{
			themTag(CTag(note.listTags[i].name,1));
		}

	}
}

//Ktra neu oldNote bi thay doi thi xoa het cac gia tri trong listTags
void CListTag::eraseTags(CNote oldNote)
{
	for(int i=0;i<oldNote.listTags.size();i++)
	{
		for(int j=0;j<this->listTags.size();j++)
		{

			if(listTags[j].name == oldNote.listTags[i].name)
			{
				listTags[j].countNotes--;
				break;
			}

		}

	}
}

bool CListTag::isTrueList()
{
	for(int i=0;i<listTags.size()-1;i++)
		for(int j=i+1;j<listTags.size();j++)
		{
			if(listTags[i].name == listTags[j].name)
			{
				return false;
			}

		}

	return true;
}


vector<CTag> CListTag::DecreaseListTags()
{
	vector<CTag> list = this->listTags;
	if(list.empty())
		return list;
	for(int i=0;i<list.size()-1;i++)
	{
		CTag temp = list[i];
		int pos = i;
		for(int j=i+1;j<list.size();j++)
		{
			if(list[j].countNotes > temp.countNotes)
			{
				temp = list[j];
				pos = j;
			}

		}
		//swap
		list[pos] = list[i];
		list[i] = temp;
	}
	return list;
}

int CListTag::totalNotes()
{
	int tong = 0;
	for(int i=0;i<this->listTags.size();i++)
	{
		tong += this->listTags[i].countNotes;
	}
	return tong;
}
