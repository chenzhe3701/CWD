#ifndef _endian_hpp_
#define _endian_hpp_

bool BigEndianTF(){
	union {
			std::uint16_t i;
			char c[sizeof(uint16_t)];
		} u;
	u.i = 1;
	bool bigEndian = (u.c[0] == 1);
	return bigEndian;
}

template <typename T>
T EndianSwap(T value)
{
    union
    {
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


#endif//_endian_hpp_