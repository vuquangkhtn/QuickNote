#include "stdafx.h"
#include "Tag.h"


CTag::CTag()
{
	name = L"";
	countNotes = 0;
}


CTag::CTag(wstring name, int countNotes)
{
	this->name = name;
	this->countNotes = countNotes;
}