#include <iostream>
#include <bitset>
#include <cassert>

struct IfdEntry{
	std::uint16_t tag;
	std::uint16_t type;
	std::uint32_t typeCount;
	union{
		std::uint32_t _long;
		struct{
			std::uint16_t v0;
			std::uint16_t v1;
		} _short;
	} valueOffset;

	void setValue(const std::uint32_t v) {valueOffset._long     = v; type = 0x0004;}	// LONG
	void setValue(const std::uint16_t v) {valueOffset._short.v0 = v; type = 0x0003;}	// SHORT
	IfdEntry(const std::uint16_t t, const std::uint32_t v) : tag(t), typeCount(0x0001) {setValue(v);}
	IfdEntry(const std::uint16_t t, const std::uint16_t v) : tag(t), typeCount(0x0001) {setValue(v);}
	IfdEntry(const std::uint16_t t): tag(t) {
		if((0x0102 == t)||(0x0103 == t)||(0x0106 == t)||(0x0115 == t)||(0x0128 == t)||(0x0140 == t)||(0x0153 == t)){
			this->type = 0x0003;
		}
		else if((0x011A == t)||(0x011B == t)){
			this->type = 0x0005;
		}
	}
};

// [B] required for Bilevel image, [G] required for grayscale image, [P] pallete color, [R] RGB
struct Ifd{
	std::uint16_t nEntry;
	IfdEntry ImageWidth = IfdEntry(0x0100);			// [B] value = image # of columns
	IfdEntry ImageLength = IfdEntry(0x0101);		// [B] value = image # of rows
	IfdEntry BitsPerSample = IfdEntry(0x0102);		//    [G][R] the num of bits per component, value = 4 or 8 (16 or 256 shades), or = 8,8,8 for RGB
	IfdEntry Compresssion = IfdEntry(0x0103);		// [B] value 1 = no compression

	IfdEntry Color = IfdEntry(0x0106);				// [B] (or PhotometricInterpretation) value 0 = white is zero, 1 = black is zero, 2 = RGB, 3 = palette color
	IfdEntry StripOffsets = IfdEntry(0x0111);		// [B] value = for each strip, the byte offset of that strip
	IfdEntry SamplesPerPixel = IfdEntry(0x0115);	//       [R] the number of components per pixel, value = 3 for RGB (or >3 for something special)
	IfdEntry RowsPerStrip = IfdEntry(0x0116);		// [B] value = # of rows in each stripe
	IfdEntry StripByteCounts = IfdEntry(0x0117);	// [B] value = for each strip, the number of bytes in that strip after any compression
	
	// IfdEntry XResolution = IfdEntry(0x011A);		// [B] 0x0005 for Rational [long,long]=[numerator,demoninator]
	// IfdEntry YResolution = IfdEntry(0x011B);		// [B] 
	// IfdEntry ResolutionUnit = IfdEntry(0x0128);	// [B] value: 1 = no measurement, 2=inch (default), 3=centimeter
	// IfdEntry ColorMap = IfdEntry(0x0140);		//    [P] for palette color, typeCount = 3*(2**BitsPerSample)

	IfdEntry SampleFormat = IfdEntry(0x0153);		//  value 1=unsigned integer, 2=two's complement signed integer, 3=IEEE floating point, 4=undefined. Note, the size of sample is still done by BitsPerSample.
	std::uint32_t nextOffset;

	void getvalue(){
		std::cout<<"getting value"<<std::endl;
	}
};

int main(){	

	Ifd ifd;

	std::cout << ifd.ImageWidth.tag <<","<<ifd.ImageWidth.type<<","<<ifd.ImageWidth.typeCount<<","<<ifd.ImageWidth.valueOffset._long<<std::endl;
	std::cout << "The structure is not packed, i.e., Ifd size: " << sizeof(Ifd) <<std::endl;

}