#include <windows.h>
#include <time.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>

#include "bit.h"
#include "video_seq.h"
#include "picture.h"
#include "slice.h"
#include "macroblock.h"
#include "block.h"
#include "image.h"
 
#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")
 
using namespace std;
 
VideoSeq *video;
const char g_szClassName[] = "MpegPlayer";
int frame_cnt = 0;
bool isPlay = false;
 
void run_background(void* argv) {
	char* filename = (char *)argv;
	ifstream fin(filename, ios::binary);
	if (!fin.is_open()){
		cout << "open input file " << filename << " error." << endl;
		return;
	}

	bool DEBUG = false;

	InBit inBit(fin);
	Image image;

	Block block(inBit, DEBUG);
	Macroblock macroblock(inBit, DEBUG, block, image);
	Slice slice(inBit, DEBUG, macroblock);
	Picture picture(inBit, DEBUG, slice, image);
	VideoSeq videoSeq(inBit, DEBUG, picture, image);
	video = &videoSeq;

	videoSeq.video_sequence();

	fin.close();
	isPlay = true;
	while (isPlay)
		;
}
 
void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
    RECT rcClient, rcWind;
    POINT ptDiff;
    GetClientRect(hWnd, &rcClient);
    GetWindowRect(hWnd, &rcWind);
    ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
    ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
    MoveWindow(hWnd,rcWind.left, rcWind.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
}
 
void frame_update(HWND hwnd, int start_time) {   
    // Wait until next frame should be shown
   	while((double)(clock()-start_time)/CLOCKS_PER_SEC * video->p_rate() < frame_cnt)
  		return;

    // Next frame
    int width = video->width();
    int height = video->height();
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbm = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
    int *buffer = new int[width*height];

    // Lock window's size to fit the clip
    ClientResize(hwnd, width, height);

    // Retrieve next frame
	Frame* frame = video->get_frame(frame_cnt);
	frame_cnt++;
	if (frame == NULL){
		if (isPlay){
			cout << "Player: Display done." << endl;
			exit(0);
		}
		else {
			cout << "Player: Decoder not realtime." << endl;
			exit(0);
		}
	}
	cout << "Player: " << frame_cnt << " frame." << endl;
    // Fill in to buffer (YCbCr->RGB)
    for(int m = 0; m < height; m++) {
        for(int n = 0; n < width; n++) {
            float y = frame->y[m][n];
            float cb = frame->cb[m>>1][n>>1]-128.0;
            float cr = frame->cr[m>>1][n>>1]-128.0;
            int R = y + 1.402*cr;
            int G = y - 0.34414*cb - 0.71414*cr;
            int B = y + 1.772*cb;
            R = R > 255? 255: R < 0? 0: R;
            G = G > 255? 255: G < 0? 0: G;
            B = B > 255? 255: B < 0? 0: B;
            buffer[m * width + n] = (R<<16)|(G<<8)|B;
        }
    }

    // And set to HBITMAP
    SetBitmapBits(hbm, width * height * sizeof(int), buffer);
     
    // Window's size is fixed, so just use BitBlt
    BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
     
    // Output
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
    delete [] buffer;
     
}
 
// Step 4. the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CLOSE: // X button
            DestroyWindow(hwnd); // destroy the specific window
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int argc = 0;
	LPWSTR *argv;
	argv = CommandLineToArgvW((LPCWSTR)lpCmdLine, &argc);

    _beginthread(run_background, 0, (char *)argv[0]);
     
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
     
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
     
    hwnd = CreateWindowEx (
        WS_EX_CLIENTEDGE, // extended window style, can try 0 or more other values
        g_szClassName,
        "MPEG Player",
        WS_OVERLAPPEDWINDOW, // window style parameter
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, // X and Y co-ordinate of left top, width, height
        NULL, NULL, hInstance, NULL // parent window handle, menu handle, application instance handle, pointer to window creation data
    );
    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	// Let decodr has time to begin.
	Sleep(300);

    ShowWindow(hwnd, nCmdShow); // show the window
    UpdateWindow(hwnd); // redraw
	
    int start_time = clock();
    while(1) {
        if(PeekMessage(&Msg, NULL, 0, 0, 0)) {  // non-blocking
            if(GetMessage(&Msg, NULL, 0, 0) > 0) {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
            else break;
        }
		Sleep(1);
        frame_update(hwnd, start_time);
    }
    return Msg.wParam;
}
