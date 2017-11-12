#include <iostream>
#include <bitset>

int main(){

	char a = 0xFF;
	unsigned char b;
	b = a;

	a >>= 1;
	b >>= 1;
	std::cout << std::bitset<8>(a) << "," << std::bitset<8>(b) << std::endl;
	std::cout << std::bitset<32>(a) << "," << std::bitset<32>(b) << std::endl;

}