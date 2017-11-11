#include <iostream>
#include <bitset>

int main(){

	struct IfdEntry{
		std::uint16_t tag;
		std::uint16_t type;
		std::uint32_t typeCount;
		union{
			std::uint32_t _long;
			struct{
				std::uint16_t _v0;
				std::uint16_t _v1;
			} _short;
		} valueOffset;
	};

	struct Ifd{
		IfdEntry width;
		IfdEntry height;
	};

	Ifd ifd;

	std::cout << ifd.width.tag;
}