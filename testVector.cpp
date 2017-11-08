//
// This code test operations on vector

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <numeric>

int main(int argc, char *argv[]){
	int nR = 2;
	int nC = 2;
	int nP = 2;

	std::vector<std::vector<std::vector<float> > > myVector;
	myVector.assign(nP, std::vector<std::vector<float> >(nR, std::vector<float>(nC,1)));
	std::cout << myVector[0][1][2] << std::endl; 

	int n=10;
	std::vector<int> a(n);
	std::iota(a.begin(),a.end(),0);
	for (int i=0;i<a.size();++i)
	std::cout << *(a.begin()+i) << " ";
	
	std::cout<<std::endl;
	std::vector<int> b(a);
	a.insert(a.begin(),b.rbegin(),b.rbegin()+3);
	for (int i=0;i<a.size();++i){
		std::cout << a[i] << " ";
	}
	std::cout << std::endl << *(a.end());	


int width = 20; 
int width_m = 24;
int height = 15;
int vRangeH = 4;
int vRangeV = 4;
int snake = 1;

	std::vector<float> xData((size_t)width), yData((size_t)height);
	std::iota(xData.begin(), xData.end(), 0.0);
	std::iota(yData.begin(), yData.end(), 0.0);
	float scaleX = xData.back();
	float scaleY = yData.back();	// scaleY is range in vertical direction
	std::for_each(xData.begin(), xData.end(), [scaleX](float& v){v /= scaleX; });//0->100% of scan size
	std::for_each(yData.begin(), yData.end(), [scaleY](float& v){v /= scaleY; });//0->100% of scan size
	scaleX = xData.back() / 2;
	std::for_each(xData.begin(), xData.end(), [scaleX](float& v){v -= scaleX; });//make symmetric about 0
	scaleY = yData.back() / 2;
	std::for_each(yData.begin(), yData.end(), [scaleY](float& v){v -= scaleY; });//make symmetric about 0
	scaleX = 2.0 * vRangeH;
	scaleY = 2.0 * vRangeV;
	std::for_each(xData.begin(), xData.end(), [scaleX](float& v){v *= scaleX; });//scale so limits are +/- vRangeH
	std::for_each(yData.begin(), yData.end(), [scaleY](float& v){v *= scaleY; });//scale so limits are +/- vRangeV

	float d1 = xData[1] - xData[0];
	// If snake, insert at begin&end.  If raster, only insert at begin.
	for (int i = 0; i < (width_m - width) / 2; ++i){
		xData.insert(xData.begin(), xData.front() - d1);
		if (snake){
			xData.insert(xData.end(), xData.back() + d1);
		}
		else{
			xData.insert(xData.begin(), xData.front() - d1);
		}
	}
		
	for (int i=0;i<xData.size();++i){
		std::cout << xData[i] << " ";
	}
}