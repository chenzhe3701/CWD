#include <iostream>
#include <fstream>
#include <time.h>

int main(int argc, char *argv []){
	time_t timer = time(NULL);
	struct tm *timeInfo = localtime(&timer);
	std::string timeString = asctime(timeInfo);
	std::string str2 = "AAA";
	
	std::fstream myFile, myFile2;
	myFile.open("d:\\myfile.txt", std::fstream::app | std::ios::binary);	// append

	// myFile << timeString.substr(0, timeString.find("\n"));		// this eliminates the carriage
	myFile << timeString;
	
	myFile2.open("d:\\myfile.txt", std::fstream::app | std::ios::binary);	// append
	myFile2 << str2;

	myFile.close();
	return 0;	
}