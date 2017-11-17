#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <tif.hpp>


int main(int argc, char *argv[]){

	std::string folderName = "C:\\Users\\ZheChen\\Desktop\\Demonstrate Exteranl Shift\\";
	std::string outputName = folderName + "shift_Log.txt";

	bool snakeTF = false;
	std::string fileName; 
	fileName = folderName + "raster_2k_16_frame.tif";	snakeTF = false;
	fileName = folderName + "raster_4k_8_frame.tif";	snakeTF = false;
	fileName = folderName + "snake_2k_16_frame.tif";	snakeTF = true;
	fileName = folderName + "snake_4k_16_frame.tif";	snakeTF = true;

	// fileName = folderName + "3stack_small.tif";

	if(argc>1){
		std::string s(argv[1]);
		fileName = s;
	}

	std::vector<uint32_t> imageWidth, imageHeight, imageSampleFormat, nChannels;
	std::vector<std::vector<uint16_t>> imageFrames;
	Tif::Read(imageFrames, imageWidth, imageHeight, imageSampleFormat, nChannels, fileName);

	std::vector<float> shifts = correlateRows<float>(imageFrames, imageHeight[0], imageWidth[0], snakeTF, 10);

	std::fstream os;
	os.open(outputName, std::ios::app | std::ios::binary);	// append

	int ind = 0;
	for(int iF=0; iF<imageFrames.size(); iF++){
		os << shifts[ind];
		ind++;
		if(0 == iF%2) os << "\t";
		else os << "\n";
	}
	os << "\n";

	os.close();
}

