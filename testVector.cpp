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
	for (int i=0;i<a.size();++i)
	std::cout << a[i] << " ";
}