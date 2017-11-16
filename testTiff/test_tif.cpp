#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <tif.hpp>


int main(int argc, char *argv[]){

	std::string fileName = "..\\3stack_small.tif";

	if(argc>1){
		std::string s(argv[1]);
		fileName = s;
	}

	std::vector<uint32_t> imageWidth, imageHeight, imageSampleFormat, nChannels;
	std::vector<std::vector<uint16_t>> imageFrames;
	Tif::Read(imageFrames, imageWidth, imageHeight, imageSampleFormat, nChannels, fileName);

	std::vector<float> shifts = correlateRows<float>(imageFrames, imageHeight[0], imageWidth[0], false, 100);

	std::cout << "\nshifts of each rows:\n";
	printVector(shifts);

}

