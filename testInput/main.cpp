#include <iostream>
#include <stdio.h>

int main(int argc, char* argv[]){

if(argc==1){
	std::cout<<"usage: Name.exe "<<std::endl;
	std::cout<<"[-vic] vicName"<<std::endl;
	std::cout<<"[-folder] parent folder"<<std::endl;	
}else{
	int i = 1;
	while(i<argc){
		switch(argv[i][1]){
			case 'a': std::cout<<1<<std::endl;
			break;


		}
	}
}

}