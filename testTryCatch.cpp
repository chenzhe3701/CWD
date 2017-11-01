//
// This code test try.. catch...

#include <iostream>

int main(int argc, char *argv[]){
	try{
		std::cout << "in try 1" << std::endl;
		throw std::runtime_error("designed error");
		std::cout << "in try 2" << std::endl;
	}
	catch(std::exception &e){
		std::cout << "in catch" << std::endl;
	}
	std::cout << "outside catch" << std::endl;
}