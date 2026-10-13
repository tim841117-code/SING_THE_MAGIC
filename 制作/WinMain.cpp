#pragma	comment(lib,"winmm")	//	?????????????????????
#include "MyPG.h"


LRESULT CALLBACK WndProc(HWND wnd_, UINT msg_, WPARAM wParam_, LPARAM lParam_);
HWND MyPG_CreateWindow(HINSTANCE inst_, const TCHAR cName_[], const TCHAR tName_[], RECT *wSize_, int showCmd_);

//-----------------------------------------------------------------------------
//??????
//??????????????????????
//-----------------------------------------------------------------------------
int __stdcall WinMain(	HINSTANCE inst_,	//
						HINSTANCE,			//
						LPSTR,				//
						int showCmd)		//
{
	ge = new MyPG::MyGameEngine( );

	HWND	wnd;								//	?????????
	RECT	ws = { 0, 0, (LONG)(ge->screenWidth  * ge->viewScaleW), (LONG)(ge->screenHeight * ge->viewScaleH) };
	wnd = MyPG_CreateWindow(inst_, ge->windowClassName, ge->windowTitle, &ws, showCmd);
	if(wnd == nullptr){ return 0;}

//	?????????
	ge->B_Initialize(wnd);

//	????????
	MSG		msg = {};
	while (msg.message != WM_QUIT)
	{
		//???????????
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			//??????????????
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (GetActiveWindow() == wnd)
		{
			ge->B_Step(wnd);		//	?????
		}
	}

	//	????????
	delete ge;
	return 0;
}

//-----------------------------------------------------------------------------
//???????
//???????????????????Í[??????????????
//-----------------------------------------------------------------------------
HWND MyPG_CreateWindow(HINSTANCE inst_, const TCHAR cName_[], const TCHAR tName_[], RECT* wSize_, int showCmd_)
{
	WNDCLASSEX wcex;						//	??????????
	HWND wnd;								//	?????????

//	??????
	wcex.style			= (CS_HREDRAW | CS_VREDRAW);
	wcex.hIcon			= LoadIcon(inst_, IDI_APPLICATION);
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	wcex.hIconSm		= LoadIcon(inst_, IDI_APPLICATION);
	wcex.hInstance		= inst_;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= cName_;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
//	Windows???
	if(!RegisterClassEx(&wcex))
	{
		MessageBox(nullptr, "????????Âc?", nullptr, MB_OK);
		return	nullptr;
	}
//	???????
	DWORD dws = ge->screenMode ? WS_POPUP : (WS_CAPTION | WS_SYSMENU);
	AdjustWindowRectEx(wSize_, dws, false, WS_EX_APPWINDOW);

	wnd = CreateWindowEx(	WS_EX_APPWINDOW, 
							cName_,
							tName_,
							dws, 0, 0,
							(wSize_->right  - wSize_->left),
							(wSize_->bottom - wSize_->top),
							nullptr, nullptr, inst_, nullptr);
	if(!wnd)
	{
		MessageBox(nullptr, "????????Âc?", nullptr, MB_OK);
		return	nullptr;
	}

//	?????????????
	ShowWindow(wnd, showCmd_);
	UpdateWindow(wnd);

	return wnd;	//??
}

//-----------------------------------------------------------------------------
//??????????
//???????????????????
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(	HWND	wnd_,		//	?????????
							UINT	msg_,		//	?????
							WPARAM	wParam_,	//	?????
							LPARAM	lParam_)	//	?????
{
	LRESULT ret = (LRESULT)0;
//	Windows???„°????????????
	switch(msg_){

	case WM_KEYDOWN:
		if(wParam_ == VK_BACK){
			static bool dm = true;
			ge->DebugMode( dm = !dm );
		}
		break;

	//	???????????
		case WM_CREATE:
			break;

	//	?????????????????
		case WM_ACTIVATEAPP:
			break;

	//	?????????
		case WM_CLOSE:
			ge->quitRequest = true;
			break;

	//	???????????
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

	//	??ÁV???????Windows???????
		default:
			ret = DefWindowProc(wnd_, msg_, wParam_, lParam_);
	}
	return ret;
}

