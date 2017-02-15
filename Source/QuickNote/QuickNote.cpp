// QuickNote.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "QuickNote.h"
#include "ListNote.h"
#include "ListTag.h"
#include <shellapi.h>
#include <strsafe.h>
#include <CommCtrl.h>
#include <Unknwn.h> 
#include <gdiplus.h>
#include <Windowsx.h>
#include <winuser.h> 
#include <direct.h>
#define MAX_LOADSTRING 1024
#define IDC_LINK 4000
using namespace Gdiplus;
#pragma comment (lib, "gdiplus.lib")
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK FormNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FormTags(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FormNotesOfTag(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FormStatistics(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

NOTIFYICONDATA showNotifyIcon(HWND hWnd);
void createTxtFile(string filename);
void loadLVNotes(CTag curTag);
void createCol();
void doInstallHook(HWND hWnd);
void doRemoveHook(HWND);
void getCode();
HFONT getDefaultFont(int size);

HMENU hMenu;
NOTIFYICONDATA nid;
CListNote notes;
CListTag tags;
int code;
bool isRunning = false;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QUICKNOTE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKNOTE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKNOTE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_QUICKNOTE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, 0); //An man hinh chinh
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
	case WM_CREATE:
		
		hMenu = CreatePopupMenu();
		InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, ID_NOTE, L"&Note	Window+Space");
		InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ID_VIEWTAGS, L"View &tags	Window+T");
		InsertMenu(hMenu, 2, MF_BYPOSITION | MF_STRING, ID_VIEWSTATITISTICS, L"View Statitistics	Window+S");
		InsertMenu(hMenu, 3, MF_BYPOSITION | MF_STRING, ID_EXIT, L"&Exit	Window+E");
		MessageBox(NULL,L"Khởi động thành công!",L"Welcome",MB_OK|MB_ICONINFORMATION);
		SetForegroundWindow(hWnd);
		nid = showNotifyIcon(hWnd);
		_mkdir("notes");
		createTxtFile("ListTags.txt");
		createTxtFile("ListNotes.txt");
		doInstallHook(hWnd);
		SetTimer(hWnd,ID_TIMER,100,NULL);
		break;
	
	case WM_TIMER:
		switch(wParam)
		{
		case ID_TIMER:
			if(isRunning==true)
			{
				getCode();
				if(code >=1)
				{
					switch (code)
					{
					case 1:
						code = -1;
						DialogBox(hInst, MAKEINTRESOURCE(IDD_NOTE), hWnd, FormNote);
						break;
					case 2:
						code = -1;
						DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMTAGS), hWnd, FormTags);
						break;
					case 3:
						code = -1;
						DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTICS), hWnd, FormStatistics);
						break;
					case 4:
						code = -1;
						DestroyWindow(hWnd);
						break;
					default:
						code = -1;
						break;
					}
				}
			}
			break;
		}
	case WM_QUICKNOTE:
	{
		wmId    = LOWORD(lParam);
		wmEvent = HIWORD(lParam);
		switch (wmId)
		{
		case WM_RBUTTONUP:
			{
				POINT pt;
				GetCursorPos(&pt);
				TrackPopupMenu (hMenu,TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_NOTE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_NOTE), hWnd, FormNote);
			break;
		case ID_VIEWTAGS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMTAGS), hWnd, FormTags);
			break;
		case ID_VIEWSTATITISTICS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTICS), hWnd, FormStatistics);
			break;
		case ID_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		doRemoveHook(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
NOTIFYICONDATA showNotifyIcon(HWND hWnd)
{
	//// {9D44F210-D29A-4D9B-8E59-27065A37D209}
	//static const GUID myGUID  = 
	//{ 0x9d44f210, 0xd29a, 0x4d9b, { 0x8e, 0x59, 0x27, 0x6, 0x5a, 0x37, 0xd2, 0x9 } };
	//NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
	nid.uVersion = NOTIFYICON_VERSION_4;
	nid.uID = 100;
    nid.uFlags =  NIF_TIP | NIF_INFO |NIF_ICON | NIF_MESSAGE |NIF_SHOWTIP;
	// This text will be shown as the icon's tooltip.
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Quick note");
	StringCchCopy(nid.szInfo, ARRAYSIZE(nid.szInfo), L"Quick note");
    // Load the icon
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_QUICKNOTE));
	nid.uCallbackMessage = WM_QUICKNOTE;
	//nid.guidItem = myGUID;
    // Show the notification.
    Shell_NotifyIcon(NIM_ADD, &nid);
	// Set the version
	Shell_NotifyIcon(NIM_SETVERSION, &nid);
	return nid;
}


void SelText(HWND hwndEdit,int iFirst,int iSecond)
{
	if (iFirst <= iSecond)
        SendMessage(hwndEdit, EM_SETSEL, iFirst, iSecond);
	else
    {
        SendMessage(hwndEdit, EM_SETSEL, iFirst, iFirst);
	}
}

void unselectText(HWND hwndEdit)
{
	SendMessage(hwndEdit, EM_SETSEL, 0, 0);
}

HWND tagEvent;
bool isChecking = false;
vector<CTag> list;
void setBtnText(HWND hDlg,vector<CTag> decreaseList)
{
		
	WCHAR* curTag = NULL;
	curTag = new WCHAR[MAX_LOADSTRING];
	if(decreaseList.size() >= 1)
	{
		swprintf(curTag,MAX_LOADSTRING,L"%s",decreaseList[0].name.c_str());
		SetDlgItemText(hDlg,IDC_BUTTON1,curTag);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1),true);
	}
	else
	{
		SetDlgItemText(hDlg,IDC_BUTTON1,L"Không có");
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1),false);
		
	}
	
	if(decreaseList.size() >= 2)
	{
		swprintf(curTag,MAX_LOADSTRING,L"%s",decreaseList[1].name.c_str());
		SetDlgItemText(hDlg,IDC_BUTTON2,curTag);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON2),true);
	}
	else
	{
		SetDlgItemText(hDlg,IDC_BUTTON2,L"Không có");
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON2),false);
	}
	
	if(decreaseList.size() >= 3)
	{
		swprintf(curTag,MAX_LOADSTRING,L"%s",decreaseList[2].name.c_str());
		SetDlgItemText(hDlg,IDC_BUTTON3,curTag);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON3),true);
	}
	else
	{
		SetDlgItemText(hDlg,IDC_BUTTON3,L"Không có");
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON3),false);
	}
	if(!curTag)
		delete[] curTag;
}

vector<CTag> editList(HWND hDlg)
{
	vector<CTag> decreaseList = tags.DecreaseListTags();
	WCHAR* contentTag = new WCHAR[MAX_LOADSTRING];
	GetDlgItemText(hDlg,IDC_NOTE_TAGS,contentTag,MAX_LOADSTRING);
	wstring wtag = contentTag;
	int pos = 0;
	for(int i=0;i<wtag.length();i++)
	{
		if(wtag[i] == L',')
		{
			wstring temp = wtag.substr(pos,i-pos);
			pos = i+1;
			for(int j=0;j<decreaseList.size();j++)
			{
				if(decreaseList[j].name == temp)
				{
					decreaseList.erase(decreaseList.begin()+j);
					break;
				}
			}
		}
		else
		if(i == wtag.length()-1)
		{
			wstring temp = wtag.substr(pos,i-pos+1);
			for(int j=0;j<decreaseList.size();j++)
			{
				if(decreaseList[j].name == temp)
				{
					decreaseList.erase(decreaseList.begin()+j);
					break;
				}
			}
		}
	}
	return decreaseList;
}
//Tạo note
INT_PTR CALLBACK FormNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	WCHAR* buffText = NULL;
	WCHAR* buffTag = NULL;
	int stText,stTag;
	switch (message)
	{
	case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			SetTextColor(hdcStatic, RGB(0,0,0));
			SetBkColor(hdcStatic, RGB(250, 252, 186));
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
		}
	case WM_INITDIALOG:
		{
			isRunning = false;
			tags.docDSTagTuFile("ListTags.txt");
			tagEvent = GetDlgItem(hDlg,IDC_NOTE_TAGS);
			list = tags.DecreaseListTags();
			list = editList(hDlg);
			setBtnText(hDlg,list);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
        {
		case IDC_BUTTON1:
			{
				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1 );
				
				WCHAR* buffBtn = new WCHAR[MAX_LOADSTRING];
				GetDlgItemText(hDlg,IDC_BUTTON1,buffBtn,MAX_LOADSTRING);
				
				if(buffBtn == L"None")
				{
					break;
				}
				if(buffText[0] == 0)
				{
					buffText = buffBtn;
				}
				else
				{
					swprintf(buffText,MAX_LOADSTRING,L"%s,%s",buffText,buffBtn);
				}
				SetWindowText(tagEvent,buffText);
				list = editList(hDlg);
				setBtnText(hDlg,list);
			}
			break;
		case IDC_BUTTON2:
			{
				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1  );
				
				WCHAR* buffBtn = new WCHAR[MAX_LOADSTRING];
				GetDlgItemText(hDlg,IDC_BUTTON2,buffBtn,MAX_LOADSTRING);
				
				if(buffBtn == L"None")
				{
					break;
				}
				if(buffText[0] == 0)
				{
					buffText = buffBtn;
				}
				else
				{
					swprintf(buffText,MAX_LOADSTRING,L"%s,%s",buffText,buffBtn);
				}
				SetWindowText(tagEvent,buffText);
				list = editList(hDlg);
				setBtnText(hDlg,list);

			}
			break;
		case IDC_BUTTON3:
			{
				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1  );
				
				WCHAR* buffBtn = new WCHAR[MAX_LOADSTRING];
				GetDlgItemText(hDlg,IDC_BUTTON3,buffBtn,MAX_LOADSTRING);
				
				if(buffBtn == L"None")
				{
					break;
				}
				if(buffText[0] == 0)
				{
					buffText = buffBtn;
				}
				else
				{
					swprintf(buffText,MAX_LOADSTRING,L"%s,%s",buffText,buffBtn);
				}
				SetWindowText(tagEvent,buffText);
				list = editList(hDlg);
				setBtnText(hDlg,list);

			}
			break;

		case IDC_NOTE_TAGS:
			if(wmEvent == EN_CHANGE && isChecking == false)
			{
				isChecking = true;
				setBtnText(hDlg,list);
				wstring sBuffer, sDummy, sWords;

				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1  );
				sBuffer = buffText;
				if(sBuffer.empty() || GetAsyncKeyState(VK_BACK)!=0 || GetAsyncKeyState(VK_DELETE)!=0)
				{
					list = editList(hDlg);
					setBtnText(hDlg,list);
					isChecking = false;
					return (INT_PTR)TRUE;
				}

				wchar_t nChar = sBuffer[sBuffer.length()-1];

				if( nChar == ',')
				{
					list = editList(hDlg);
					setBtnText(hDlg,list);
					isChecking = false;
					return (INT_PTR)TRUE;
				}
				sDummy = sBuffer;
				//xác định các kí tự được bấm của tag
				int curPos = sBuffer.find_last_of(',');
				sWords = sBuffer.substr(curPos+1);
				sDummy.erase(curPos+1,sWords.length());
				if( !sWords.empty() ) 
				{
					for( int f=0; f<list.size(); f++ ) 
					{
						if(sWords == list[f].name.substr(0,sWords.length()))
						{
							sDummy += list[f].name;
							swprintf(buffText,100,L"%s",sDummy.c_str());
							SetWindowText(tagEvent,buffText);
							int pos1 = curPos+1+sWords.length();
							int pos2 = curPos+1+list[f].name.length();
							SelText(tagEvent,pos1,pos2);
							break;
						}

					}
				}
				isChecking = false;
				return (INT_PTR)TRUE;
			}
			break;
        case IDOK:
			{
				HWND text, tag;
				text = GetDlgItem(hDlg,IDC_NOTE_NOTE);
				tag = GetDlgItem(hDlg,IDC_NOTE_TAGS);

				stText = GetWindowTextLength(text);
				buffText = new WCHAR[stText+1];
				GetWindowText(text, buffText, stText+1);

				stTag = GetWindowTextLength(tag);
				buffTag = new WCHAR[stTag+1];
				GetWindowText(tag, buffTag, stTag+1);
				CNote note = note.createNote(buffTag,buffText);
				//Ktra xem co tag trung nhau k?
				if(note.isTrueList())
				{
					tags.findNewTags(note);//Xem thu co tag moi thi them vao, có tag trùng thì tăng
					note.ghiNoteVaoFile("ListNotes.txt");
				}
				else
				{
					MessageBox(hDlg,L"The tags must not be null or repeated",L"Warning",MB_OK|MB_ICONWARNING);
					break;
				}
				MessageBox(hDlg,L"Create successfully!",L"Success",MB_OK|MB_ICONINFORMATION);
				return (INT_PTR)TRUE;
			}
        case IDCANCEL:
			DestroyWindow(hDlg);
			return (INT_PTR)FALSE;
		}
		break;
	case WM_DESTROY:
		tags.ghiDSTagVaoFile("ListTags.txt");
        EndDialog(hDlg, LOWORD(wParam));
		isRunning = true;
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

//tag được nháy đúp vào
CTag currentTag;
//listbox tags
HWND hwndList;
//Xem tags
INT_PTR CALLBACK FormTags(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	int wmId, wmEvent;
	switch (message)
	{
	case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			SetTextColor(hdcStatic, RGB(0,0,0));
			SetBkColor(hdcStatic, RGB(250, 252, 186));
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
		}
	case WM_INITDIALOG:
		hwndList = GetDlgItem(hDlg, IDC_LISTTAGS);
		SendMessage(hwndList,WM_SETFONT,WPARAM(getDefaultFont(20)),TRUE);
		isRunning = false;
		tags.docDSTagTuFile("ListTags.txt");
		for (int i = 0; i < tags.listTags.size(); i++) 
        { 
			WCHAR* buff = new WCHAR[MAX_LOADSTRING];
			swprintf(buff,50,L"The tag [%s]\t\twith\t%d note(s)",tags.listTags[i].name.c_str(),tags.listTags[i].countNotes);
            int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM) buff); 
            SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
        }
        // Set input focus to the list box.
        SendMessage(hwndList, LB_SETCURSEL, 0, 0);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
		case IDCANCEL:
			DestroyWindow(hDlg);
			break;
		}
		case IDC_LISTTAGS:
        {
            switch (wmEvent) 
            { 
            case LBN_DBLCLK:
                {
					int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 

					// Get item data.
					int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
                    //Go to View notes of tag
					currentTag = tags.listTags[lbItem];
					EndDialog(hDlg, LOWORD(wParam));
					DialogBox(hInst,MAKEINTRESOURCE(IDD_VIEWNOTES),hDlg,FormNotesOfTag);
                } 
				break;
			}
		}
		break;
	case WM_DESTROY:
		isRunning = true;
        EndDialog(hDlg, LOWORD(wParam));
		break;
	}
	return (INT_PTR)FALSE;

}

//vị trí note được click
int curPos;
CNote note;
CNote oldNote;
INT_PTR CALLBACK EditNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	WCHAR* buffText = NULL;
	WCHAR* buffTag = NULL;
	int stText,stTag;
	HWND btn,btnLink;
	switch (message)
	{
	case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			SetTextColor(hdcStatic, RGB(0,0,0));
			SetBkColor(hdcStatic, RGB(250, 252, 186));
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
		}
	case WM_INITDIALOG:
		{
			btn = GetDlgItem(hDlg,IDOK);
			SetWindowText(btn,L"Edit");
			btnLink = CreateWindowEx(0,L"BUTTON",L"Open note", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,140, 251, 90, 22,hDlg, (HMENU)IDC_LINK,hInst,NULL);
			SendMessage(btnLink, WM_SETFONT, WPARAM(getDefaultFont(15)), TRUE);
			//lấy note được click
			note = notes.listNotes[curPos];
			//Lưu note cũ
			oldNote = note;
			//xử lý xuống hàng
			wstring temp = note.content;
			for(int i=0;i<temp.length();i++)
			{
				if(temp[i] == L'\r')
				{
					temp.insert(temp.begin()+i+1,L'\n');
				}
			}
			//gán giá trị
			SetDlgItemText(hDlg,IDC_NOTE_NOTE,temp.c_str());
			SetDlgItemText(hDlg,IDC_NOTE_TAGS,note.getStrTags().c_str());
			isRunning = false;
			tagEvent = GetDlgItem(hDlg,IDC_NOTE_TAGS);
			tags.docDSTagTuFile("ListTags.txt");
			list = tags.DecreaseListTags();
			list = editList(hDlg);
			setBtnText(hDlg,list);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
        {
		case IDC_BUTTON1:
			{
				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1 );
				
				WCHAR* buffBtn = new WCHAR[MAX_LOADSTRING];
				GetDlgItemText(hDlg,IDC_BUTTON1,buffBtn,MAX_LOADSTRING);
				
				if(buffBtn == L"None")
				{
					break;
				}
				if(buffText[0] == 0)
				{
					buffText = buffBtn;
				}
				else
				{
					swprintf(buffText,MAX_LOADSTRING,L"%s,%s",buffText,buffBtn);
				}
				SetWindowText(tagEvent,buffText);
				list = editList(hDlg);
				setBtnText(hDlg,list);

			}
			break;
		case IDC_BUTTON2:
			{
				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1  );
				
				WCHAR* buffBtn = new WCHAR[MAX_LOADSTRING];
				GetDlgItemText(hDlg,IDC_BUTTON2,buffBtn,MAX_LOADSTRING);
				
				if(buffBtn == L"None")
				{
					break;
				}
				if(buffText[0] == 0)
				{
					buffText = buffBtn;
				}
				else
				{
					swprintf(buffText,MAX_LOADSTRING,L"%s,%s",buffText,buffBtn);
				}
				SetWindowText(tagEvent,buffText);
				list = editList(hDlg);
				setBtnText(hDlg,list);

			}
			break;
		case IDC_BUTTON3:
			{
				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1  );
				
				WCHAR* buffBtn = new WCHAR[MAX_LOADSTRING];
				GetDlgItemText(hDlg,IDC_BUTTON3,buffBtn,MAX_LOADSTRING);
				
				if(buffBtn == L"None")
				{
					break;
				}
				if(buffText[0] == 0)
				{
					buffText = buffBtn;
				}
				else
				{
					swprintf(buffText,MAX_LOADSTRING,L"%s,%s",buffText,buffBtn);
				}
				SetWindowText(tagEvent,buffText);
				list = editList(hDlg);
				setBtnText(hDlg,list);

			}
			break;

		case IDC_LINK:
			{
				buffText = new WCHAR[MAX_LOADSTRING];
				wstring stemp = wstring(note.path.begin(), note.path.end());
				swprintf(buffText,MAX_LOADSTRING,L"notes\\%s",stemp.c_str());
				ShellExecute(NULL, _T("open"), buffText, NULL, NULL, SW_SHOWNORMAL);
			}
			break;
		case IDC_NOTE_TAGS:
			if(wmEvent == EN_CHANGE && isChecking == false)
			{
				isChecking = true;
				wstring sBuffer, sDummy, sWords;

				stText = GetWindowTextLength(tagEvent);
				buffText = new WCHAR[stText+1];
				GetWindowText(tagEvent, buffText,stText+1 );
				sBuffer = buffText;
				if(sBuffer.empty() || GetAsyncKeyState(VK_BACK)!=0 || GetAsyncKeyState(VK_DELETE)!=0)
				{
					list = editList(hDlg);
					setBtnText(hDlg,list);
					isChecking = false;
					return (INT_PTR)TRUE;
				}

				wchar_t nChar = sBuffer[sBuffer.length()-1];

				if( nChar == ',')
				{
					list = editList(hDlg);
					setBtnText(hDlg,list);
					isChecking = false;
					return (INT_PTR)TRUE;
				}
				sDummy = sBuffer;
				//xác định các kí tự được bấm của tag
				int curPos = sBuffer.find_last_of(',');
				sWords = sBuffer.substr(curPos+1);
				sDummy.erase(curPos+1,sWords.length());
				if( !sWords.empty() ) 
				{
					for( int f=0; f<list.size(); f++ ) 
					{
						if(sWords == list[f].name.substr(0,sWords.length()))
						{
							sDummy += list[f].name;
							swprintf(buffText,100,L"%s",sDummy.c_str());
							SetWindowText(tagEvent,buffText);
							int pos1 = curPos+1+sWords.length();
							int pos2 = curPos+1+list[f].name.length();
							SelText(tagEvent,pos1,pos2);
							break;
						}

					}
				}
				isChecking = false;
				return (INT_PTR)TRUE;
			}
			break;
        case IDOK:
			{
				HWND text, tag;
				text = GetDlgItem(hDlg,IDC_NOTE_NOTE);
				tag = GetDlgItem(hDlg,IDC_NOTE_TAGS);

				stText = GetWindowTextLength(text);
				buffText = new WCHAR[stText+1];
				GetWindowText(text, buffText, stText+1);

				stTag = GetWindowTextLength(tag);
				buffTag = new WCHAR[stTag+1];
				GetWindowText(tag, buffTag, stTag+1);

				note.getTags(buffTag);
				note.content = buffText;
				//Ktra xem co tag trung nhau k?
				if(note.isTrueList())
				{
					//Xóa listTag cũ ->thêm listTag mới
					tags.eraseTags(oldNote);
					tags.findNewTags(note);//Xem thu co tag moi thi them vao
					note.ghiNoteVaoFile("ListNotes.txt");
				}
				else
				{
					MessageBox(hDlg,L"The tags must not be null or repeated",L"Warning",MB_OK|MB_ICONWARNING);
					break;
				}
				MessageBox(hDlg,L"Create successfully!",L"Success",MB_OK|MB_ICONINFORMATION);
				return (INT_PTR)TRUE;
			}
        case IDCANCEL:
			DestroyWindow(hDlg);
			break;
		}
		break;
	case WM_DESTROY:
		{
			isRunning = true;
			tags.ghiDSTagVaoFile("ListTags.txt");
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}

HWND lvDSNotes;
//Xem list note tương ứng với tag
INT_PTR CALLBACK FormNotesOfTag(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	int wmId, wmEvent;
	switch (message)
	{
	case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			SetTextColor(hdcStatic, RGB(0,0,0));
			SetBkColor(hdcStatic, RGB(250, 252, 186));
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
		}
	case WM_INITDIALOG:
		{
			//sửa tên
			WCHAR* buff = new WCHAR[MAX_LOADSTRING];
			swprintf(buff,50,L"The tag [%s] with %d note(s)",currentTag.name.c_str(),currentTag.countNotes);
			SetWindowText(hDlg,buff);

			HWND hText = CreateWindowEx(0, L"STATIC", L"List of tags", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 100, 20, hDlg, NULL, hInst, NULL);
			SendMessage(hText, WM_SETFONT, WPARAM(getDefaultFont(17)), TRUE);

			hText = CreateWindowEx(0, L"STATIC", L"Double-click on one note to view and edit note", WS_CHILD | WS_VISIBLE | SS_LEFT, 150, 340, 300, 20, hDlg, NULL, hInst, NULL);
			SendMessage(hText, WM_SETFONT, WPARAM(getDefaultFont(17)), TRUE);
			//Tạo listview
			lvDSNotes = CreateWindowEx(0, WC_LISTVIEW, L"List of notes", WS_CHILD | WS_VISIBLE | WS_VSCROLL |WS_BORDER,10,30, 600, 300,hDlg, NULL,hInst,NULL);
			SendMessage(lvDSNotes, WM_SETFONT, WPARAM(getDefaultFont(17)), TRUE);
			//Đổi sang kiểu details
			SetWindowLong(lvDSNotes, GWL_STYLE, GetWindowLong(lvDSNotes, GWL_STYLE) | LVS_REPORT);
			ListView_SetExtendedListViewStyle(lvDSNotes,LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
			createCol();
			//mỗi lần mở list tags sẽ load lại listnote
			notes.listNotes.clear();
			notes.docDSNoteTuFile("ListNotes.txt");
			//ktra và xóa note không lquan
			loadLVNotes(currentTag);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
		case IDCANCEL:
			DestroyWindow(hDlg);
			return (INT_PTR)FALSE;
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_DBLCLK:
			{
				int lbItem = ListView_GetNextItem(lvDSNotes, -1, LVNI_SELECTED);
				// Get item data.
				int i = (int)SendMessage(lvDSNotes, LB_GETITEMDATA, lbItem, 0);
				if(lbItem < 0)
					break;
				curPos = lbItem;
				EndDialog(hDlg, LOWORD(wParam));
				DialogBox(hInst, MAKEINTRESOURCE(IDD_NOTE), hDlg, EditNote);
				break;
			}
		default:
			break;
		}
		break;
	case WM_DESTROY:
		{
			EndDialog(hDlg, LOWORD(wParam));
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMTAGS), hDlg, FormTags);
			return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;

}

//Thống kê
Color listColor[10] = {Color(255,0,0,255),
						Color(255, 0, 0),
						Color(0, 255, 0),
						Color(1, 255, 213),
						Color(0, 0, 255),
						Color(231, 76, 60),
						Color(146, 1, 255),
						Color(128, 0, 128),
						Color(95, 95, 95),
						Color(255, 153, 0)
						};
INT_PTR CALLBACK FormStatistics(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	PAINTSTRUCT ps;
	HDC hdc;
	int wmId, wmEvent;
	WCHAR* buffText = NULL;
	switch (message)
	{
	case WM_CTLCOLORDLG:
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			SetTextColor(hdcStatic, RGB(0,0,0));
			SetBkColor(hdcStatic, RGB(250, 252, 186));
			return (INT_PTR)CreateSolidBrush(RGB(250, 252, 186));
		}
	case WM_INITDIALOG:
		{
			tags.docDSTagTuFile("ListTags.txt");
			notes.listNotes.clear();
			notes.docDSNoteTuFile("ListNotes.txt");
			HWND hText = CreateWindowEx(0, L"STATIC",L"TAGS DETAIL", WS_CHILD | WS_VISIBLE | SS_LEFT, 300,10,100,20, hDlg, NULL, hInst, NULL);
			SendMessage(hText, WM_SETFONT, WPARAM(getDefaultFont(20)), TRUE);
			hText = CreateWindowEx(0, L"STATIC",L"", WS_CHILD | WS_VISIBLE | SS_LEFT, 10,250,150,60, hDlg, NULL, hInst, NULL);
			SendMessage(hText, WM_SETFONT, WPARAM(getDefaultFont(18)), TRUE);
			buffText = new WCHAR[MAX_LOADSTRING];
			swprintf(buffText,MAX_LOADSTRING,L"Total notes:	%d\nTotal tags:	%d",notes.listNotes.size(),tags.listTags.size());
			SetWindowText(hText,buffText);

		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
		case IDCANCEL:
			DestroyWindow(hDlg);
			break;
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hDlg, &ps);
			Graphics* graphics = new Graphics(hdc);
			vector<CTag> list = tags.DecreaseListTags();
			int totalNotes = tags.totalNotes();
			float maxAngle = 360.0F;
			float eachNoteAngle = maxAngle/totalNotes;
			float startAngle =  0.0F;//bắt đầu
			float sweepAngle = 0.0F;//số đoạn được phân

			int yPosNote = 0;
			//10 tags đầu
			for(int i=0;i<list.size() && i<9;i++)
			{
				sweepAngle = list[i].countNotes*eachNoteAngle;
				Pen* pen = new Pen(listColor[i],1);
				Brush* brush= new SolidBrush(listColor[i]);
				//Vẽ pie
				Rect rect(10,10,210,210);
				graphics->FillPie(brush,rect,startAngle,sweepAngle);
				graphics->DrawPie(pen,rect,startAngle,sweepAngle);
				startAngle += sweepAngle;
				//Vẽ chú thích
				yPosNote = 20*i+50;
				Rect rectNote(300,yPosNote,20,20);
				graphics->FillRectangle(brush,rectNote);
				graphics->DrawRectangle(pen,rectNote);
				buffText = new WCHAR[MAX_LOADSTRING];
				swprintf(buffText,MAX_LOADSTRING,L"%s: %d note(s)",list[i].name.c_str(),list[i].countNotes);
				HWND hText = CreateWindowEx(0, L"STATIC",buffText, WS_CHILD | WS_VISIBLE | SS_LEFT, 330,yPosNote,100,20, hDlg, NULL, hInst, NULL);
				SendMessage(hText, WM_SETFONT, WPARAM(getDefaultFont(17)), TRUE);
			}

			//others tags
			if(list.size()>=9)
			{
				int tong=0;
				for(int i=9;i<list.size();i++)
				{
					tong+=list[i].countNotes;
				}
				sweepAngle = tong*eachNoteAngle;
				Pen* pen = new Pen(listColor[9],1);
				Brush* brush= new SolidBrush(listColor[9]);
				//Vẽ pie
				Rect rect(10,10,210,210);
				graphics->FillPie(brush,rect,startAngle,sweepAngle);
				graphics->DrawPie(pen,rect,startAngle,sweepAngle);
				//Vẽ chú thích
				yPosNote = 20*9+50;
				Rect rectNote(300,yPosNote,20,20);
				graphics->FillRectangle(brush,rectNote);
				graphics->DrawRectangle(pen,rectNote);
				buffText = new WCHAR[MAX_LOADSTRING];
				swprintf(buffText,MAX_LOADSTRING,L"Others: %d note(s)",tong);
				HWND hText = CreateWindowEx(0, L"STATIC",buffText, WS_CHILD | WS_VISIBLE | SS_LEFT, 330,yPosNote,100,20, hDlg, NULL, hInst, NULL);
				SendMessage(hText, WM_SETFONT, WPARAM(getDefaultFont(17)), TRUE);
			}


			delete graphics;
			EndPaint(hDlg, &ps);
		}
		break;
	case WM_DESTROY:
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
	}
	if(!buffText)
		delete[] buffText;
	return (INT_PTR)FALSE;

}


bool existTagInNote(CTag tag, CNote note)
{
	for(int i=0;i<note.listTags.size();i++)
	{
		if(tag.name == note.listTags[i].name)
		{
			return true;
		}
	}
	return false;
}

void loadLVNotes(CTag curTag)
{
	ListView_DeleteAllItems((HWND)lvDSNotes);
	LV_ITEM lv;
	lv.mask =LVIF_TEXT;
	lv.iItem = notes.listNotes.size();
	WCHAR* buffNgay = new WCHAR[MAX_LOADSTRING];
	WCHAR* buffTags = new WCHAR[MAX_LOADSTRING];
	WCHAR* buffNoidung = new WCHAR[MAX_LOADSTRING];
	int dem = 0;
	for(int i=0;i<notes.listNotes.size();i++)
	{
		if(existTagInNote(curTag,notes.listNotes[i]))
		{
			CNote note = notes.listNotes[i];
			wstring strNgay = L"";
			lv.iSubItem = 0;
			swprintf(buffNoidung,50,L"%s",note.content.c_str());
			//Đổi kí tự cho đẹp hơn :p
			for(int j=0;j<note.path.length()-9;j++)
			{
				if(note.path[j+5] == '-')
				{
					strNgay += L"/";
				}
				else
				if(note.path[j+5] == '_'||note.path[j+5] =='s')
				{
					strNgay += L" ";
				}
				else
				if(note.path[j+5] == 'm'||note.path[j+5] =='h')
				{
					strNgay += L":";
				}
				else
					strNgay += (WCHAR) note.path[j+5];
			}
			swprintf(buffNgay,50,L"%s",strNgay.c_str());
			swprintf(buffTags,50,L"%s",note.getStrTags().c_str());
			lv.pszText = buffNgay;
			ListView_InsertItem(lvDSNotes, &lv);
			ListView_SetItemText(lvDSNotes, dem, 1,buffTags);
			ListView_SetItemText(lvDSNotes, dem, 2, buffNoidung);
			dem++;
		}
		else
		{
			//xóa note không có tag tương ứng
			notes.xoaNote(i);
			i--;
		}
	}

	if(!buffNgay)
		delete[]buffNgay;
	if(!buffTags)
		delete[]buffTags;
	if(!buffNoidung)
		delete[]buffNoidung;

}

void createCol()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt=LVCFMT_LEFT;
	lvCol.cx = 150;
	lvCol.pszText = _T("Date created");
	ListView_InsertColumn(lvDSNotes, 0, &lvCol);
	
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt=LVCFMT_LEFT;
	lvCol.cx = 150;
	lvCol.pszText = _T("List tags");
	ListView_InsertColumn(lvDSNotes, 1, &lvCol);
	
	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 300;
	lvCol.pszText = _T("Note content");
	ListView_InsertColumn(lvDSNotes, 2, &lvCol);
}

void createTxtFile(string filename)
{
	wfstream file;
	file.open(filename);
	if(file == NULL)
		file.open(filename,ios::out);
	file.close();
}

//Hook
void doInstallHook(HWND hWnd)
{
	// gọi hàm DLL theo kiểu Run-time
	// Định nghĩa prototype của hàm
	typedef VOID (*MYPROC)(HWND hWnd); 
	
	HINSTANCE hinstLib; 
	MYPROC ProcAddr; 
	
	// load DLL và lấy handle của DLL module
	hinstLib = LoadLibrary(L"DLLKeyBoard.dll");
	// Nếu load thành công, lấy địa chỉ của hàm DrawCircle trong DLL
	if (hinstLib != NULL) { 
		ProcAddr = (MYPROC) GetProcAddress(hinstLib, "_doInstallHook"); 
		// Nếu lấy được địa chỉ hàm, gọi thực hiện hàm
		if (ProcAddr != NULL)
		{
			isRunning = true;
			ProcAddr(hWnd);
		}
	} 
}

void doRemoveHook(HWND hWnd)
{
	// gọi hàm DLL theo kiểu Run-time
	// Định nghĩa prototype của hàm
	typedef VOID (*MYPROC)(HWND); 
	
	HINSTANCE hinstLib; 
	MYPROC ProcAddr; 
	
	// load DLL và lấy handle của DLL module
	hinstLib = LoadLibrary(L"DLLKeyBoard.dll");
	// Nếu load thành công, lấy địa chỉ của hàm DrawCircle trong DLL
	if (hinstLib != NULL) { 
		ProcAddr = (MYPROC) GetProcAddress(hinstLib, "_doRemoveHook"); 
		// Nếu lấy được địa chỉ hàm, gọi thực hiện hàm
		if (ProcAddr != NULL) 
		{
			isRunning = false;
			ProcAddr (hWnd);
		}
	} 
}

//Lấy code từ DLL tương ứng với phím tắt được bấm
void getCode()
{
	// gọi hàm DLL theo kiểu Run-time
	// Định nghĩa prototype của hàm
	typedef int (*MYPROC)(); 
	
	HINSTANCE hinstLib; 
	MYPROC ProcAddr; 
	
	// load DLL và lấy handle của DLL module
	hinstLib = LoadLibrary(L"DLLKeyBoard.dll");
	// Nếu load thành công, lấy địa chỉ của hàm DrawCircle trong DLL
	if (hinstLib != NULL) { 
		ProcAddr = (MYPROC) GetProcAddress(hinstLib, "getCode"); 
		// Nếu lấy được địa chỉ hàm, gọi thực hiện hàm
		if (ProcAddr != NULL) 
		{
			code = ProcAddr();
		}
	} 
}

//lấy font
HFONT getDefaultFont(int size)
{
	// Lấy font hệ thống
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont = CreateFont(size, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, lf.lfFaceName);
	return hFont;
}