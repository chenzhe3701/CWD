#include <iostream>
#include <bitset>
#include <cmath>

int myfun(const uint16_t a){
	std::cout << "uint16_t " << a << "\n";
}
int myfun(const uint32_t a){
	std::cout << "uint32_t: " << a << "\n";
}

int main(){

	myfun(uint32_t(0x0001));

	std::cout << sizeof(int) << std::endl;


	char A = 'A';
	char a = 'a';
	int N = 3;
	std::cout << A << "," << (int)A  << "," << a << "," << (int)a << '\n';
	printf("%c %hhi %s \n", A, A, " ---- printf using \%hhi");
	std::bitset<8> myclass;
	myclass = 5;
	std::cout<<myclass;
	std::cout << std::bitset<8>(N) << " ---- bitset output \n";
	
	std::cout << CHAR_BIT << " ---- CHAR_BIT \n";
}