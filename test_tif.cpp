#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <D:\p\c++\CWD\tif.hpp>

int main(int argc, char *argv[]){
	

	// first make a tif image.
	std::string fileName = "d:\\2by2tif.tif";
	// std::vector<uint16_t> vec(4);
	// vec[0] = 0;
	// vec[1] = pow(2,14)-1;
	// vec[2] = pow(2,15)-1;
	// vec[3] = pow(2,16)-1;
	// Tif::Write(vec.data(),2,2,fileName);		
	
	//fileName = "d:\\mm.tif";
	//fileName = "d:\\8_stack.tif";

	if(argc>1) {
		std::string s(argv[1]);
		fileName = s;
	}

	std::vector<uint32_t> imageWidth, imageHeight, nChannels;
	std::vector<std::vector<uint16_t>> imageFrames;
	Tif::Read(imageFrames, imageWidth, imageHeight, nChannels, fileName);

	// unsigned int aa = 0xffffffae;
	// std::cout << aa;
	// printf some values
	// for (int i = 0; i<8; i++){
	// 	printf("%X ",buffer[i]);
	// }
	// std::cout << std::endl;
}

