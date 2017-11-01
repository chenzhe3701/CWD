#include <iostream>
#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[]){

union {
	std::uint32_t i;
	char c[4];
} u = {0x01020304};

std::cout<< int(u.c[0]);
//	MessageBox(NULL,"my message", "my title",MB_OK);

}