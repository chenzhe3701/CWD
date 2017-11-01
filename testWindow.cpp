//
// This function demonstrate how to (1) find a window, 
// (2) activate it and (3) using sendinput to close it.

#include <iostream>
#include <windows.h>

int main(int argc, char *argv[]){


	HWND hwnd;
	hwnd = FindWindow(NULL,"Vic-2D Analysis");
	if(hwnd){
		std::cout << "window found" << std::endl;
		SetForegroundWindow(hwnd);

		Sleep(500);
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.wVk = VK_ESCAPE;

		ip.ki.dwFlags = 0;
		SendInput(1,&ip,sizeof(INPUT));

		ip.ki.dwFlags = 0|KEYEVENTF_KEYUP;
		SendInput(1,&ip,sizeof(INPUT));

	}
}