#ifndef _endian_hpp_
#define _endian_hpp_

// chenzhe <chenzhe@ucsb.edu>
// creatd to help treat endian related issue.
// 2017-11

template <typename T>
void printVector(std::vector<T> a){
	for(int i=0; i<a.size(); ++i) std::cout << a[i] << " ";

	std::cout << std::endl;
}


// Test if this computer is bigEndian
bool BigEndianThisMachine(){
	union {
			std::uint16_t i;
			char c[sizeof(uint16_t)];
		} u;
	u.i = 0x0102;
	bool isBigEndian = (u.c[0] == 1);
	return isBigEndian;
}

// push a <T> type variable into vector, byte-by-byte, ignoring data type.
template <typename T>
void byte_push(std::vector<char>& v, T t){
	for (int i=0; i<sizeof(T); ++i){
		v.push_back(*(reinterpret_cast<char*>(&t)+i));
	}
}

template <typename T>
void byte_push(std::vector<unsigned char>& v, T t){
	for (int i=0; i<sizeof(T); ++i){
		v.push_back(*(reinterpret_cast<char*>(&t)+i));
	}
}

// Swap endian of a input number (referenced by value), if the switch swapTF is set to true.
template <typename T>
T EndianSwap(T value, bool swapTF = true) {
    if (swapTF){
	    union {
	        T i;
	        unsigned char c[sizeof(T)];
	    } u;

	    u.i = value;
	    unsigned char temp;

	    for (int i = 0; i < sizeof(T)/2; ++i){
	    	temp = u.c[i];
	    	u.c[i] = u.c[sizeof(T)-1-i];
	    	u.c[sizeof(T)-1-i] = temp;
	    }

	    return u.i;
	}
	else{
		return value;
	}
}

#endif//_endian_hpp_