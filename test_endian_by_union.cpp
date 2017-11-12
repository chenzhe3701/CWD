#include <iostream>

	union S{
		std::uint16_t a;
		std::int16_t b;
		std::uint8_t c[2];
	};

	const union {
		std::uint16_t i;
		char c[4];
	} u = {0x0102};

int main(){

	std::cout << int (u.c[0]) << std::endl;

	S a;
	a.a = 0x494D;	// because of little-endian, 0x49
	std::cout << a.a << "," << a.b << std::endl;
	std::cout << a.c[0] << "," << a.c[1]  << std::endl;

	S b;
	b.c[0] = 0x4D;
	b.c[1] = 0x49;
	std::cout << b.a << "," << b.b << std::endl;
	std::cout << b.c[0] << "," << b.c[1]  << std::endl;

	uint8_t temp;

	temp = a.c[0];
	a.c[0] = a.c[1];
	a.c[1] = temp;

	std::cout << a.a << "," << a.b;

}