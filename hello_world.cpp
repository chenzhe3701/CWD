#include <iostream>
#include <bitset>
#include <cmath>

int main(){

	std::cout << sizeof(int) << std::endl;


	char A = 'A';
	char a = 'a';
	int N = 3;
	std::cout << A << "," << (int)A  << "," << a << "," << (int)a << '\n';
	printf("%c %hhi %s \n", A, A, " ---- printf using \%hhi");
	std::bitset<8> myclass;
	myclass = 5;
	std::cout<<myclass;
	std::cout  << std::bitset<8>(N) << " ---- bitset output \n";
	
	std::cout  << CHAR_BIT << " ---- CHAR_BIT \n";
}