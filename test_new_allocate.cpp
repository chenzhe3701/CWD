#include <iostream>

void makeNew(char*& ptr){
	ptr = new char(4);
	ptr[0] = 'a';
	ptr[1] = 'b';
	ptr[2] = 'c';
	ptr[3] = 'd';
}

int main(){
	char * ptr;
	makeNew(ptr);
	std::cout << ptr[1];

}