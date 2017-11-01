#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdint.h>

// This test how to read from buffer into Raw array.

int main(){

	int nLineInt=2;
	int nRS = 2;
	bool snake = TRUE;
	int width_m = 4;
	int height = 3;
	int nDwellSamples = 2;


	int buffer[10000];
	for (int i=0;i<10000;i++){
		buffer[i] = i;
	}
	
	std::vector<std::vector<std::vector<std::vector<int16_t> > > > frameImagesRaw;	
	frameImagesRaw.assign(nLineInt, std::vector<std::vector<std::vector<int16_t> > >(nRS, std::vector<std::vector<int16_t> >(nDwellSamples, std::vector<int16_t>((size_t)width_m * height))));	//hold each frame as one block of memory, but expand one line into 2 lines

	for(int iRow = 0; iRow < 1; iRow++){
		for (uint64_t iLineInt = 0; iLineInt < nLineInt; ++iLineInt){
			for (uint64_t iRS = 0; iRS < nRS; ++iRS){
				if (snake && (1==iRS)) {
					const uint64_t rowOffset = width_m * iRow + width_m - 1;	//offset of row end
					for (uint64_t iDwellSamples = 0; iDwellSamples < nDwellSamples; iDwellSamples++)
						for (uint64_t iCol = 0; iCol < width_m; iCol++) {
							frameImagesRaw[iLineInt][iRS][iDwellSamples][(size_t)(rowOffset - iCol)] = buffer[(size_t)(iLineInt*nRS*nDwellSamples*width_m + iRS*nDwellSamples*width_m + nDwellSamples*iCol + iDwellSamples)];
							std::cout << (size_t)(iLineInt*nRS*nDwellSamples*width_m + iRS*nDwellSamples*width_m + nDwellSamples*iCol + iDwellSamples) << ",";
						}
				}
				else {
					const uint64_t rowOffset = width_m * iRow;		//offset of row start
					for (uint64_t iDwellSamples = 0; iDwellSamples < nDwellSamples; iDwellSamples++) {
						for (uint64_t iCol = 0; iCol < width_m; iCol++) {
							frameImagesRaw[iLineInt][iRS][iDwellSamples][(size_t)(rowOffset + iCol)] = buffer[(size_t)(iLineInt*nRS*nDwellSamples*width_m + iRS*nDwellSamples*width_m + nDwellSamples*iCol + iDwellSamples)];
							std::cout << (size_t)(iLineInt*nRS*nDwellSamples*width_m + iRS*nDwellSamples*width_m + nDwellSamples*iCol + iDwellSamples) << ",";
						}
					}
				}
			}
		}
	}

	for(int iLine = 0; iLine < nLineInt; iLine++){
		for(int iRS = 0; iRS<nRS; iRS++){
			for (int iD = 0; iD<nDwellSamples; iD++){
				for (int iElement=0; iElement<width_m*height; iElement++){
					std::cout<<frameImagesRaw[iLine][iRS][iD][iElement]<<",";
				}
				std::cout << std::endl;
			}
		}
	}
}
	
	// for (uint64_t iLineInt = 0; iLineInt < nLineInt; ++iLineInt){
	// 	for (uint64_t iRS = 0; iRS < nRS; ++iRS){

	// 		if (snake && (1==iRS)) {
	// 			const uint64_t rowOffset = width_m * iRow + width_m - 1;	//offset of row end
	// 			for (uint64_t iDwellSamples = 0; iDwellSamples < nDwellSamples; iDwellSamples++)
	// 				for (uint64_t iCol = 0; iCol < width_m; iCol++)
	// 					std::cout << iLineInt*nRS*nDwellSamples*width_m + iRS*nDwellSamples*width_m + nDwellSamples*iCol + iDwellSamples << ","<<std::endl;
	// 		}
	// 		else {
	// 			const uint64_t rowOffset = width_m * iRow;	//offset of row start
	// 			for (uint64_t iDwellSamples = 0; iDwellSamples < nDwellSamples; iDwellSamples++) {
	// 				for (uint64_t iCol = 0; iCol < width_m; iCol++) {
	// 					std::cout << iLineInt*nRS*nDwellSamples*width_m + iRS*nDwellSamples*width_m + nDwellSamples*iCol + iDwellSamples << std::endl;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

