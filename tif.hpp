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

struct Tif {
	std::uint32_t width, height;
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
	static void Read(T& width, T& height, T& fileSize, std::string fileName){
		std::ifstream is(fileName, std::ios::in | std::ios::binary);

		// Determine the file size (may disable this later)
		is.seekg(0, is.end);	// set the position of the next character to be extracted form the input stream (offset, seekdir)
		fileSize = is.tellg();	// return the position of the current character
		is.seekg(0, is.beg);	// set it back to beginning
		std::cout << "File size: " << fileSize << " Bytes" << std::endl;
		
		Tif tif(0,0);
		std::uint32_t firstIfd;		
		std::uint32_t nIfds;
		
		tif.readHeader(is, firstIfd);	// readHeader
		tif.getNumIfds(is, nIfds, firstIfd);
		tif.readIfds(is, nIfds, firstIfd);
		
	}


	void readIfds(std::istream& is, const std::uint32_t& nIfds, const std::uint32_t& firstIfd){
		
		// store the offset of this Ifd. Update it at the end of loop. (offset wrt header beginning is positive, so this type difference should be OK)
		std::int32_t nextIfd = firstIfd;		
		char* bufPtr_2 = new char[2];	// 2 Bytes buf to read # of Ifd entries
		char* bufPtr_4 = new char[4];	// 4 Bytes buf to read the value of nextIfd offset

		std::vector<std::vector<char>> IfdCopy;	// ptr[] to arrays that stores individual Ifd binary copies
		
		for (int iIfd = 0; iIfd < nIfds; ++ iIfd){
			// (1) go to/set position to beginning of this Ifd, read 2 bytes to get # of entries
			is.seekg(nextIfd);
			is.read(bufPtr_2, 2);
			std::uint32_t nEntries = EndianSwap(*(reinterpret_cast<std::uint16_t*>(bufPtr_2)), m_endianSwap);
			std::uint32_t pos = nextIfd + 2 + 12 * nEntries;		// offSet position that stores [the value of the nextIfd]
			
			// (2) copy the whole Ifd, push to IfdCopy
			is.seekg(nextIfd);
			std::vector<char> a(2+12*nEntries+4);
			is.read(a.data(), 2+12*nEntries+4);
			IfdCopy.push_back(a);





			// (Last) go to the end of this Ifd, read 4 bytes to get the offSet of the nextIfd
			is.seekg(pos);			
			is.read(bufPtr_4, 4);
			nextIfd = EndianSwap(*(reinterpret_cast<std::uint32_t*>(bufPtr_4)), m_endianSwap);
		}

		for (int iIfd = 0; iIfd < nIfds; ++ iIfd){
			std::cout << "\nIfd #: " << iIfd << std::endl;
			printf("# of entries: %X%X", (uint8_t)IfdCopy[iIfd][0], (uint8_t)IfdCopy[iIfd][1]);	// # Ifds
			for(int j=2; j<IfdCopy[iIfd].size()-4;++j){
				if (2==j%12) std::cout<<std::endl;
				printf("%X ", (uint8_t)IfdCopy[iIfd][j]);
			}
			std::cout << "\nNext Ifd offset:";
			for(int j=IfdCopy[iIfd].size()-4; j<IfdCopy[iIfd].size();++j){
				if (IfdCopy[iIfd].size()-4==j%16) std::cout<<std::endl;
				printf("%X ", (uint8_t)IfdCopy[iIfd][j]);
			}
		}
		delete[] bufPtr_2;
		delete[] bufPtr_4;
	}


	void getNumIfds(std::istream& is, std::uint32_t& nIfds, const std::uint32_t& firstIfd){
		
		std::int32_t nextIfd = firstIfd;	// offsets. firstIfd is positive, so this type difference should be OK.
		std::int32_t pos;			// offSet position that stores [the value of the nextIfd]
		char* buf_2 = new char[2];	// 2 Bytes buf to read # of Ifd entries
		char* buf_4 = new char[4];	// 4 Bytes buf to read the value of nextIfd offset

		while(0 != nextIfd){
			is.seekg(nextIfd);			// (1) go to/set position to firstIfd
			is.read(buf_2, 2);		// (2) read 2 bytes to get # of entries
			pos = nextIfd + 2 + 12 * EndianSwap(*(reinterpret_cast<std::uint16_t*>(buf_2)), m_endianSwap);

			is.seekg(pos);			// (3) go to the end of this Ifd and read the offSet of the nextIfd
			is.read(buf_4, 4);
			nextIfd = EndianSwap(*(reinterpret_cast<std::uint32_t*>(buf_4)), m_endianSwap);

			++nIfds;
		}
		std::cout << "Total number of Ifds: " << nIfds << std::endl;

		delete[] buf_2;
		delete[] buf_4;
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

		printf("%s %X %X %X %X \n","magicBytes:", magicBytes[0], magicBytes[1], magicBytes[2], magicBytes[3]);
		std::cout << "The endian of this computer (is big?): " << m_bigEndian << std::endl;
		std::cout << "The endian of the target file (is big?): " << bigEndianTiff << std::endl;
		std::cout << "When reading, need to swap endian ?: " << m_endianSwap << std::endl;
		std::cout << "firstIfd offset endian swapped corrected: " << firstIfd << std::endl;
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