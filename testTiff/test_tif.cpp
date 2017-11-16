#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <tif.hpp>


int main(int argc, char *argv[]){

	// first make a tif image.
	std::string fileNameOut_1 = "d:\\output_1.tif";
	std::string fileNameOut_2 = "d:\\output_2.tif";
	std::string fileName = "d:\\2by2tif.tif";
	// std::vector<uint16_t> vec(4);
	// vec[0] = 0;
	// vec[1] = pow(2,14)-1;
	// vec[2] = pow(2,15)-1;
	// vec[3] = pow(2,16)-1;
	// Tif::Write(vec.data(),2,2,fileName);		
	
	fileName = "d:\\mm.tif";
	fileName = "d:\\8_stack.tif";
	// fileName = "d:\\8_bit_grayscale_pc.tif";
	// fileName = "d:\\16_bit_grayscale_pc.tif";
	// fileName = "d:\\32_bit_grayscale_pc.tif";
	// fileName = "d:\\8_bit_rgb_pc.tif";
	// fileName = "d:\\16_bit_rgb_pc.tif";
	// fileName = "d:\\32_bit_rgb_pc.tif";
	// fileName = "d:\\8_bit_rgb_planar_pc.tif";
	// fileName = "d:\\16_bit_rgb_planar_pc.tif";
	// fileName = "d:\\32_bit_rgb_planar_pc.tif";
	// fileName = "d:\\rgbrgb.tif";
	// fileName = "d:\\rrggbb.tif";

	if(argc>1){
		std::string s(argv[1]);
		fileName = s;
	}

	std::vector<uint32_t> imageWidth, imageHeight, imageSampleFormat, nChannels;
	std::vector<std::vector<uint16_t>> imageFrames;
	Tif::Read(imageFrames, imageWidth, imageHeight, imageSampleFormat, nChannels, fileName);

	int nF = 0;
	Tif::Write(imageFrames[2*nF],imageWidth[2*nF]*nChannels[2*nF],imageHeight[2*nF],fileNameOut_1);
	Tif::Write(imageFrames[2*nF+1],imageWidth[2*nF],imageHeight[2*nF]*nChannels[2*nF],fileNameOut_2);

	std::vector<float> shifts = correlateRows<float>(imageFrames, imageHeight[0], imageWidth[0], false, 100);

}

