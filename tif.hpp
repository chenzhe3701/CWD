/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 * Copyright (c) 2017, William C. Lenthe                                           *
 * All rights reserved.                                                            *
 *                                                                                 *
 * Redistribution and use in source and binary forms, with or without              *
 * modification, are permitted provided that the following conditions are met:     *
 *                                                                                 *
 * 1. Redistributions of source code must retain the above copyright notice, this  *
 *    list of conditions and the following disclaimer.                             *
 *                                                                                 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,    *
 *    this list of conditions and the following disclaimer in the documentation    *
 *    and/or other materials provided with the distribution.                       *
 *                                                                                 *
 * 3. Neither the name of the copyright holder nor the names of its                *
 *    contributors may be used to endorse or promote products derived from         *
 *    this software without specific prior written permission.                     *
 *                                                                                 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"     *
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  *
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL      *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR      *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.            *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _tif_h_
#define _tif_h_

#include <iostream>
#include <fstream>
#include <cstdint>
#include <climits>
#include <array>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <vector>
#include <numeric>
#include <D:\p\c++\CWD\endian.hpp>

template <typename T>
void printVector(std::vector<T> a){
	for(int i=0; i<a.size(); ++i) std::cout << a[i] << " ";

	std::cout << std::endl;
}


struct Tif {
	std::uint32_t width, height, channels;
	bool m_bigEndian;	// endian of this computer
	bool m_endianSwap;	// need to swap endian for the target file?

	struct IfdEntry {
		std::uint16_t tag;
		std::uint16_t type;	//can be BYTE or SHORT or LONG 
		std::uint32_t count;
		union {
			std::uint32_t _long;
			struct {
				std::uint16_t v0;
				std::uint16_t v1;
			} _short;
		} valueOffset;

		void setValue(const std::uint32_t v) {valueOffset._long     = v; type = 0x0004;}	// LONG
		void setValue(const std::uint16_t v) {valueOffset._short.v0 = v; type = 0x0003;}	// SHORT
		IfdEntry(const std::uint16_t t, const std::uint32_t v) : tag(t), count(0x0001) {setValue(v);}
		IfdEntry(const std::uint16_t t, const std::uint16_t v) : tag(t), count(0x0001) {setValue(v);}
	};
	static_assert(12 == sizeof(IfdEntry), "IfdEntry must be packed");

	void writeHeader(std::ostream& os) {
		const union {
			std::uint16_t i;
			char c[4];
		} u = {0x0102};
		const bool bigEndian = u.c[0] == 1;
		const char bigMagic[4] = {'M','M',0x00,0x2A};
		const char litMagic[4] = {'I','I',0x2A,0x00};
		const std::uint32_t firstIfd = 0x0008;//first ifd immediately following 8 byte header
		char const * const magicBytes = u.c[0] == 1 ? bigMagic : litMagic;
		os.write(magicBytes, 4);
		os.write((char*)&firstIfd, 4);
	}
	
	template <typename T>
	std::uint32_t writeIfd(std::ostream& os, const std::uint32_t offset, const bool finalEntry) {
		std::array<char, 126> ifd;//2 bytes for entry count, 10 12 byte entries, 4 bytes for offset to next ifd
		reinterpret_cast<std::uint16_t*>(ifd.data())[0] = 0x000A;//10 entries
		IfdEntry* entries = reinterpret_cast<IfdEntry*>(ifd.data()+2);
		entries[0] = IfdEntry(0x0100, width);//width
		entries[1] = IfdEntry(0x0101, height);//height
		entries[2] = IfdEntry(0x0102, std::uint16_t(CHAR_BIT * sizeof(T)));//bits per sample
		entries[3] = IfdEntry(0x0103, (std::uint16_t)0x0001);//compression: none
		entries[4] = IfdEntry(0x0106, (std::uint16_t)0x0001);//photometric interpretation: black is zero
		entries[5] = IfdEntry(0x0111, offset + 126);//strip offset: data immediately after ifd
		entries[6] = IfdEntry(0x0115, (std::uint16_t)0x0001);//samples per pixel: 1
		entries[7] = IfdEntry(0x0116, height);//rows per strip
		entries[8] = IfdEntry(0x0117, std::uint32_t(width * height * sizeof(T)));//strip byte count
		std::uint16_t format = 0x0004;//unknown
		if(std::numeric_limits<T>::is_integer) {
			if(!std::numeric_limits<T>::is_signed) format = 0x0001;//signed int
			else format = 0x0002;//unsigned int
		} else if(std::numeric_limits<T>::is_iec559) format = 0x0003;//floating point
		entries[9] = IfdEntry(0x0153, format);//sample format
		const std::uint32_t nextOffset = offset + 126 + std::uint32_t(width * height * sizeof(T));
		reinterpret_cast<std::uint32_t*>(ifd.data()+122)[0] = finalEntry ? 0x00000000 : nextOffset;
		os.write(ifd.data(), ifd.size());
		return nextOffset;
	}

	template <typename T>
	static void Read(std::vector<std::vector<uint16_t>>& imageFrames, std::vector<T>& imageWidth, std::vector<T>& imageHeight, std::vector<T>& imageSampleFormat, std::vector<T>& nChannels, std::string fileName){
		std::ifstream is(fileName, std::ios::in | std::ios::binary);

		// Determine the file size (may disable this later)
		is.seekg(0, is.end);	// set the position of the next character to be extracted form the input stream (offset, seekdir)
		std::uint32_t fileSize = is.tellg();	// return the position of the current character
		is.seekg(0, is.beg);	// set it back to beginning
		std::cout << "File size: " << fileSize << " Bytes" << std::endl;
		
		Tif tif(0,0);
		std::uint32_t firstIfd;		
		std::uint32_t nIfds = 0;
		
		tif.readHeader(is, firstIfd);	// readHeader, and update Ifd.
		tif.getNumIfds(is, nIfds, firstIfd);
		tif.readAllStack(is, nIfds, firstIfd, imageFrames, imageWidth, imageHeight, imageSampleFormat, nChannels);
		
	}

	// read into vector<vector<uint16_t>> type vector.
	// nChannels: 1=grayscale, 3= RGB
	// Currently suggest planarConfiguration = 1 Chunky. 
	template <typename T>
	void readAllStack(std::istream& is, const std::uint32_t& nIfds, const std::uint32_t& firstIfd,
		 std::vector<std::vector<uint16_t>>& imageFrames, std::vector<T>& imageWidth, std::vector<T>& imageHeight, std::vector<T>& imageSampleFormat, std::vector<T>& nChannels){
		
		// store the offset of this Ifd. Update it at the end of loop. (offset wrt header beginning is positive, so this type difference should be OK)
		std::int32_t nextIfd = firstIfd;

		// need 1 Byte buf to read single byte data
		// need 2 Bytes buf to read, e.g., # of Ifd entries
		// need 4 Bytes buf to read, e.g., the value of nextIfd offset
		char* bufPtr = new char[8];


		// std::vector<std::vector<char>> IfdCopy;	// ptr[] to arrays that stores individual Ifd binary copies
		std::vector<uint16_t> frameImg;
		for (int iIfd = 0; iIfd < nIfds; ++ iIfd){
			frameImg.clear();
			// used to get this image frame/stack/Ifd.  Must have value before reading actual image data.
			std::vector<uint32_t> stripOffSets;		// This will definitely be updated.
			std::vector<uint32_t> stripByteCounts;	// This will definitely be updated
			std::vector<uint32_t> bitsPerSample(1,1);	// (default 1, for bilevel) required for [grayscale, 4 or 8][rgb, {8,8,8}] image.
														// note if need to update, first need to clean it.
			std::uint32_t samplesPerPixel = 1;			// (default 1) required for [rgb] image. so need some default
			std::uint32_t planarConfiguration = 1;	// 1=chunky (default), 2=planar
			std::uint32_t dataType = 1;				// 1=unsinged int (default). 2=int, 3=float, 4=undefined
			std::uint32_t compression = 1;
			std::uint32_t stackWidth = 0;
			std::uint32_t stackHeight = 0;

			// (1) go to/set position to beginning of this Ifd, read 2 bytes to get # of entries
			is.seekg(nextIfd);
			is.read(bufPtr, 2);
			std::uint32_t nEntries = EndianSwap(*(reinterpret_cast<std::uint16_t*>(bufPtr)), m_endianSwap);
			std::uint32_t pos = nextIfd + 2 + 12 * nEntries;		// offSet position that stores [the value of the nextIfd]
			
			// (2) copy the whole Ifd, push to IfdCopy
			// is.seekg(nextIfd);
			// std::vector<char> a(2+12*nEntries+4);
			// is.read(a.data(), 2+12*nEntries+4);
			// IfdCopy.push_back(a);

			// (3) for each entry, interp.
			std::cout << "\nIfd number: " << iIfd << " ,  number of IfdEntries: " << nEntries << std::endl;
			for (int iEntry = 0; iEntry < nEntries; ++iEntry){
				// (3.0) set to beginning of entry
				is.seekg(nextIfd+2+12*iEntry);
				// (3.1) read Tag
				is.read(bufPtr, 2);
				std::uint16_t tag = EndianSwap(*(reinterpret_cast<std::uint16_t*>(bufPtr)), m_endianSwap);
				printf("tag: %4X\t", tag);
				// (3.2) read Type
				is.read(bufPtr, 2);
				std::uint16_t type = EndianSwap(*(reinterpret_cast<std::uint16_t*>(bufPtr)), m_endianSwap);
				printf(" type: %X\t", type);				
				// (3.3) read Count (N)
				is.read(bufPtr, 4);
				std::uint32_t count = EndianSwap(*(reinterpret_cast<std::uint32_t*>(bufPtr)), m_endianSwap);
				printf(" count: %4X\t", count);
				// (3.4) read Value/Offset raw (without interpretation)
				is.read(bufPtr, 4);
				std::uint32_t valueRaw = EndianSwap(*(reinterpret_cast<std::uint32_t*>(bufPtr)), m_endianSwap);
				printf(" value/offset as int32_t: %8X", valueRaw);

				// This contains the 'byteSize' corresponding to 'Type'
				std::uint32_t byteSizeOfType[13] = {0,sizeof(uint8_t),sizeof(char),sizeof(uint16_t),sizeof(uint32_t),2*sizeof(uint32_t),
					sizeof(int8_t),sizeof(char),sizeof(int16_t),sizeof(int32_t),2*sizeof(int32_t), sizeof(float),sizeof(double)};
				// determine (use valueRaw as value) or (use valueRaw as offset)
				if(byteSizeOfType[type] * count <=4){
					is.seekg(nextIfd+2+12*iEntry+8);
				}
				else{
					is.seekg((int32_t)valueRaw);
				}
				
				std::vector<uint64_t> value;	// Note, currently use a larger<uint64_t> vector to store all those values. !!!
				
				// (3.5) read [count] times to get the value of this entry, and push to 'value'
				for (int iCount = 0; iCount < count; ++iCount){
					// switch 'type', read properly
					// 1=uint8(byte), 2=ascii, 3=uint16(short), 4=uint32(long), 5=[long_numerator,long_demoninator], ... 
					try{
						switch(type){
							case 1:
								is.read(bufPtr, 1);
								value.push_back(EndianSwap(*(reinterpret_cast<std::uint8_t*>(bufPtr)), m_endianSwap));
								break;
							case 2:
								is.read(bufPtr, 1);
								value.push_back(EndianSwap(*(reinterpret_cast<char*>(bufPtr)), m_endianSwap));
								break;
							case 3:
								is.read(bufPtr, 2);
								value.push_back(EndianSwap(*(reinterpret_cast<std::int16_t*>(bufPtr)), m_endianSwap));
								break;
							case 4:
								is.read(bufPtr, 4);
								value.push_back(EndianSwap(*(reinterpret_cast<std::int32_t*>(bufPtr)), m_endianSwap));
								break;
							case 5:
								is.read(bufPtr, 4);
								value.push_back(EndianSwap(*(reinterpret_cast<std::int32_t*>(bufPtr)), m_endianSwap));
								is.read(bufPtr, 4);
								value.push_back(EndianSwap(*(reinterpret_cast<std::int32_t*>(bufPtr)), m_endianSwap));
								break;
							default:
								throw std::runtime_error("");
						}
					}
					catch(const std::exception& e){
						std::cout << e.what();
					}

				}

				// (3.6) print interpreted value
				std::cout << "\tvalue: ";
				for(int i=0; i<value.size(); ++i){
					switch(type){
						case 1: 
							if(i<32){std::cout << uint8_t(value[i]);}
							else{std::cout << "..."; i = value.size()-1;}
							break;
						case 2: 
							if(i<32){std::cout << char(value[i]);}
							else{std::cout << "..."; i = value.size()-1;}
							break;
						case 3: std::cout << uint16_t(value[i]); break;
						case 4: std::cout << uint32_t(value[i]); break;
						case 5: std::cout << double(value[i])/double(value[i+1]); ++i; break;
					}
				}

				// (3.7) check 'tag' to get rules ready for reading data
				// get useful information (e.g., width, height, color, format, stripOffSets/byteCounts, sampleFormat, samplesPerPixel, BitsPerSamp)
				// prevent info that cannot understand (e.g., compression, bitsPerSample)
				// ignore info that don't want (e.g., rowPerStrip)
				dataType = 1;	// set back to default type
				try{
					switch(tag){
						case 0x0100: stackWidth = value[0]; break;
						case 0x0101: stackHeight = value[0]; break;
						case 0x0102: 
							bitsPerSample.clear();
							for (int k=0; k<value.size(); ++k){
								bitsPerSample.push_back(value[k]);
								if(0!=value[k]%8) {throw std::runtime_error("\tcurrently only suggest bitsPerSample = 8xN ");}
							}
							break;
						case 0x0103: 
							compression = value[0];
							if(1!=value[0]) throw std::runtime_error("\tcurrently only suggest Compression = 1 (no compression)");
							break;
						case 0x0111:
							for (int k=0; k<value.size(); ++k){
								stripOffSets.push_back(value[k]);
							}
							break;
						case 0x011c:
							planarConfiguration = value[0];
							if(1!=planarConfiguration) throw std::runtime_error("\tcurrently only suggested planarConfiguration = 1 (chunky)");
							break;
						case 0x0115: samplesPerPixel = value[0]; break;
						case 0x0117:
							for (int k=0; k<value.size(); ++k){
								stripByteCounts.push_back(value[k]);
							}
							break;
						case 0x0153: dataType = value[0]; break;
					}
				}
				catch(const std::exception& e){
						std::cout << e.what();
				}				

				std::cout << std::endl;	// end of current entry		
			}
			// after loop all entries of this Ifd, push
			imageWidth.push_back(stackWidth);
			imageHeight.push_back(stackHeight);
			nChannels.push_back(samplesPerPixel);
			imageSampleFormat.push_back(dataType);	// push back
			
			std::cout << "stripByteCounts: ";
			printVector(stripByteCounts);

			// (4) After all IfdEntries are read, read the actual image data
			stripOffSets;	
			stripByteCounts;
			bitsPerSample;
			samplesPerPixel ;
			planarConfiguration ;	// 1=chunky, 2=planar
			dataType ;		// 1 = by default unsinged int. 2=int, 3=float, 4=undefined.

			frameImg.reserve(stackWidth*stackHeight*3);
			int bitsPerPixel = 0;
			for (int ii=0; ii<samplesPerPixel; ++ii){
				bitsPerPixel += bitsPerSample[ii];
			}
			
			int pixelsThisStrip = 0;
			uint32_t bitsThisStack = bitsPerPixel * stackWidth * stackHeight;

			int bitsToReadTotal = bitsPerPixel * stackWidth * stackHeight; // total bits to read
			int iStrip = 0;
			int iChannel = 0;
			int iPixel = 0;
			
			int bitsToReadStrip = stripByteCounts[iStrip] * 8;
			int bitsToReadCurrent = bitsPerSample[iChannel];
			int bitsRead = 0;
			int bitOffset = 0;

			int pixelsToRead = stackWidth * stackHeight;
			int channelsToRead = samplesPerPixel;
			// read one by one.  Keep updating bitOffset, bitsRead, iPixel(pixelsRead)
			// keep checking bitsToReadTotal, bitsToReadStrip
			is.seekg(stripOffSets[iStrip]);
			while(bitsRead < bitsToReadTotal){
				frameImg.push_back( readToUint16(is, bitOffset, bitsToReadCurrent, m_endianSwap) );
				bitOffset += bitsToReadCurrent;

				bitsRead += bitsToReadCurrent;
				if(bitsRead == bitsToReadTotal) break;

				if(bitsRead == bitsToReadStrip){
					++ iStrip;
					bitsToReadStrip += stripByteCounts[iStrip] * 8;
					is.seekg(stripOffSets[iStrip]);
					bitOffset = 0;
				}
				
				if(1==planarConfiguration){
					++iChannel;
					bitsToReadCurrent = bitsPerSample[iChannel];	// it depends on the channel to read
					if(iChannel == channelsToRead){
						iChannel = 0;		// reset pixel #
						++iPixel;		// and update channel #
						bitsToReadCurrent = bitsPerSample[iChannel];
					}
				}
				else if(2==planarConfiguration){
					++iPixel;			// this applies to planar. keep checking iPixel
					if(iPixel == pixelsToRead){
						iPixel = 0;		// reset pixel #
						++iChannel;		// and update channel #
						bitsToReadCurrent = bitsPerSample[iChannel];
					}
				}

			}
			// a swapped version			
			// decide which to push back
			if(1==planarConfiguration){
				std::vector<uint16_t> frameImg1to2(frameImg);
				for(int indexFrom = 0; indexFrom < frameImg.size(); indexFrom ++){
					int channelTo = indexFrom % channelsToRead;
					int pixelTo = indexFrom / channelsToRead;
					int indexTo = channelTo*pixelsToRead + pixelTo;
					frameImg1to2[indexTo] = frameImg[indexFrom];
				}
				imageFrames.push_back(frameImg);
				imageFrames.push_back(frameImg1to2);				
			}
			else{
				std::vector<uint16_t> frameImg2to1(frameImg);
				for(int indexFrom = 0; indexFrom < frameImg.size(); indexFrom ++){
					int channelTo = indexFrom / pixelsToRead;
					int pixelTo = indexFrom % pixelsToRead;
					int indexTo = pixelTo*channelsToRead + channelTo;
					frameImg2to1[indexTo] = frameImg[indexFrom];
				}
				imageFrames.push_back(frameImg2to1);
				imageFrames.push_back(frameImg);		
			}



			// (5) go to the end of this Ifd, read 4 bytes to get the offSet of the nextIfd
			is.seekg(pos);			
			is.read(bufPtr, 4);
			nextIfd = EndianSwap(*(reinterpret_cast<std::uint32_t*>(bufPtr)), m_endianSwap);
		}

		delete[] bufPtr;
	}

	// bitOffset is with respect to current position. Convert to uint16_t.  Consider endian.
	std::uint16_t readToUint16(std::istream& is, const int bitOffset, const int bitsToRead, const bool endianSwapTF){
		int32_t oldPos = is.tellg();
		char* bufPtr = new char[8];
		uint16_t returnValue = 0;

		int bitsBack = bitOffset%8;
		int byteOffset = bitOffset/8;
		is.seekg(oldPos + byteOffset);
		if(4 == bitsToRead){
			is.read(bufPtr, 1);
			uint8_t val = *(reinterpret_cast<std::uint8_t*>(bufPtr));
			val <<= bitsBack;
			val >>= 8-bitsToRead;
			returnValue = val;
		}
		else if(8==bitsToRead){
			is.read(bufPtr, 1);
			returnValue = (*(reinterpret_cast<std::uint8_t*>(bufPtr)))*256;
		}
		else if(16==bitsToRead){
			is.read(bufPtr, 2);
			returnValue = EndianSwap(*(reinterpret_cast<std::uint16_t*>(bufPtr)), endianSwapTF);
		}
		else if(32==bitsToRead){
			is.read(bufPtr, 4);
			float val = EndianSwap(*(reinterpret_cast<float*>(bufPtr)), endianSwapTF);
			returnValue = uint16_t(val * 65565 + 32768);
		}
		// std::cout << returnValue << "  returned"<< std::endl;
		is.seekg(oldPos);	// put is back to original position
		delete[] bufPtr;
		return returnValue;
	}

	void getNumIfds(std::istream& is, std::uint32_t& nIfds, const std::uint32_t& firstIfd){
		
		std::int32_t nextIfd = firstIfd;	// offsets. firstIfd is positive, so this type difference should be OK.
		std::int32_t pos;			// offSet position that stores [the value of the nextIfd]
		char* bufPtr = new char[4];	// 2 Bytes buf to read # of Ifd entries, 4 Bytes buf to read the value of nextIfd offset

		while(0 != nextIfd){
			is.seekg(nextIfd);			// (1) go to/set position to firstIfd
			is.read(bufPtr, 2);		// (2) read 2 bytes to get # of entries
			pos = nextIfd + 2 + 12 * EndianSwap(*(reinterpret_cast<std::uint16_t*>(bufPtr)), m_endianSwap);

			is.seekg(pos);			// (3) go to the end of this Ifd and read the offSet of the nextIfd
			is.read(bufPtr, 4);
			nextIfd = EndianSwap(*(reinterpret_cast<std::uint32_t*>(bufPtr)), m_endianSwap);

			++nIfds;
		}
		std::cout << "Total number of Ifds: " << nIfds << std::endl;

		delete[] bufPtr;
	}

	void readHeader(std::istream& is, std::uint32_t& firstIfd){
		is.seekg(0, is.beg);	// reset position to beginning
		char* buffer = new char[4];

		// determine endian
		is.read(buffer, 4);
		char magicBytes[4] = {buffer[0],buffer[1],buffer[2],buffer[3]};
		bool bigEndianTiff = (magicBytes[0]=='M');
		m_endianSwap = !(bigEndianTiff == m_bigEndian);

		// find firstIfd
		is.read(buffer, 4);
		firstIfd = *(reinterpret_cast<uint32_t*>(buffer));
		firstIfd = EndianSwap(firstIfd, m_endianSwap);

		// printf("%s %X %X %X %X \n","magicBytes:", magicBytes[0], magicBytes[1], magicBytes[2], magicBytes[3]);
		// std::cout << "The endian of this computer (is big?): " << m_bigEndian << std::endl;
		// std::cout << "The endian of the target file (is big?): " << bigEndianTiff << std::endl;
		std::cout << "When reading, need to swap endian ?: " << m_endianSwap << std::endl;
		// std::cout << "firstIfd offset endian swapped corrected: " << firstIfd << std::endl;
		delete[] buffer;
	}

	template <typename T>
	void writeSlice(std::ofstream& os, T const * const data) {
		//most readers seem to ignore the orientation flag so data should be written with image convention
		const int rowBytes = (int)width * sizeof(T);
		// chenzhe's special note: Originally, Will Lenthe's code writes data up-side-down.  I changed it so the top of the image get saved first. (Consider this together with the reverse of the y data during scan data generation)
		// for(int i = int(height)-1; i >= 0; i--) os.write(reinterpret_cast<char const*>(data) + i * rowBytes, rowBytes);
		for (int i = 0; i < int(height); i++) os.write(reinterpret_cast<char const*>(data)+i * rowBytes, rowBytes);
	}

	template <typename T>
	static void Write(T const * const data, const std::uint32_t w, const std::uint32_t h, std::string fileName) {
		//open file and write header + single ifd
		Tif tif(w, h);
		std::ofstream os(fileName, std::ios::out | std::ios::binary);
		tif.writeHeader(os);
		tif.writeIfd<T>(os, 0x00000008, true);
		tif.writeSlice<T>(os, data);
	}

	template <typename T>
	static void Write(T const * const * const data, const std::uint32_t w, const std::uint32_t h, const std::uint32_t slices, std::string fileName) {
		//open file and write header + single ifd
		Tif tif(w, h);
		std::ofstream os(fileName, std::ios::out | std::ios::binary);
		tif.writeHeader(os);
		std::uint32_t offset = 0x00000008;
		for(std::uint32_t i = 0; i < slices; i++) {
			offset = tif.writeIfd<T>(os, offset, i+1 == slices);
			tif.writeSlice<T>(os, data[i]);
		}
	}

	template <typename T>
	static void Write(std::vector<T>& buff, const std::uint32_t w, const std::uint32_t h, std::string fileName) {
		Write(buff.data(), w, h, fileName);
	}
	
	template <typename T>
	static void Write(std::vector< std::vector<T> >& buff, const std::uint32_t w, const std::uint32_t h, std::string fileName) {
		std::vector<T const *> slicePointers(buff.size());
		for(size_t i = 0; i < buff.size(); i++) slicePointers[i] = buff[i].data();
		Write(slicePointers.data(), w, h, (std::uint32_t)slicePointers.size(), fileName);
	}

	private:
		Tif(const std::uint32_t w, const std::uint32_t h) : width(w), height(h) {
			union {
				std::uint16_t i;
				char c[sizeof(uint16_t)];
			} u;
			u.i = 0x0102;
			m_bigEndian = (u.c[0] == 1);
		}
};

#endif//_tif_h_