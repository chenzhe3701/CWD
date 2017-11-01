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
	std::vector<int> a(10);
	std::iota(a.begin(),a.end(),0);
	int num = 2;
	std::cout << *( a.begin()+num) << std::endl;
	std::cout << *( a.rbegin()+a.size()-1 - num) << std::endl;
	
	std::cout<<std::endl;
	for (int i=0;i<a.size();i++){
		std::cout<<a[i]<<",";
	}
}
