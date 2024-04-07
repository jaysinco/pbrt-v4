///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2012, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#ifdef NDEBUG
#    undef NDEBUG
#endif

#include <ImfHuf.h>
#include "ImathRandom.h"
#include <ImfArray.h>
#include <iostream>
#include <exception>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>


using namespace OPENEXR_IMF_NAMESPACE;
using namespace std;


namespace {

void
fill1 (unsigned short data[/*n*/], int n, float bias, IMATH_NAMESPACE::Rand48 & rand48)
{
    for (int i = 0; i < n; ++i)
	data[i] = (unsigned short)
		  (pow (rand48.nextf(), double(bias)) * (USHRT_MAX + 1));
}


void
fill2 (unsigned short data[/*n*/], int n, int m, IMATH_NAMESPACE::Rand48 & rand48)
{
    for (int i = 0; i < n; ++i)
	data[i] = 0;

    for (int i = 0; i < m; ++i)
	data[rand48.nexti() % n] = (unsigned short) (rand48.nextf() * (USHRT_MAX + 1));
}


void
fill3 (unsigned short data[/*n*/], int n, int m)
{
    for (int i = 0; i < n; ++i)
	data[i] = m;
}


void
fill4 (unsigned short data[/*n*/], int n)
{
    for (int i = 0; i < n; ++i)
	data[i] = i & USHRT_MAX;
}


void
fill5 (unsigned short data[/*n*/], int n)
{
    for (int i = 0; i < n; ++i)
	data[i] = 0;

    int j = 0, k = 0;

    for (int i = 0; i < n; ++i)
    {
	data[i] = j;
	j = j + k;
	k = k + 1;

	if (j > USHRT_MAX)
	    break;
    }
}


void
compressUncompress (const unsigned short raw[], int n)
{
    Array <char> compressed (3 * n + 4 * 65536);
    Array <unsigned short> uncompressed (n);

    cout << "compressing " << flush;

    int nCompressed = hufCompress (raw, n, compressed);

    cout << "uncompressing " << flush;

    hufUncompress (compressed, nCompressed, uncompressed, n);

    cout << "comparing: " << flush;

    for (int i = 0; i < n; ++i)
	assert (uncompressed[i] == raw[i]);

    cout << sizeof (raw[0]) * n << " bytes, compressed " <<
	    nCompressed  << " bytes" << endl;
}

void
compressUncompressSubset(const unsigned short raw[], int n)
{
    // Test various subsets of the data set

    Array <char> compressed (3 * n + 4 * 65536);
    Array <unsigned short> uncompressed (n);

    int maxOffset = 16;
    if (n <= maxOffset) {
        maxOffset = n-1;
    }

    for (int offset=1; offset<maxOffset; ++offset) {

        int maxLength = n - offset;
        int minLength = maxLength - 16;
        if (minLength < 1) minLength = 1;

        for (int length=minLength; length<=maxLength; ++length) {
            cout << "compressing " << flush;

            int nCompressed = hufCompress (raw + offset, length, compressed + offset);

            cout << "uncompressing " << flush;

            hufUncompress (compressed + offset, nCompressed, uncompressed + offset, length);

            for (int i = 0; i < length; ++i)
                assert (uncompressed[offset+i] == raw[offset+i]);

            cout << sizeof (raw[0]) * length << " bytes, compressed " << nCompressed  << " bytes ";
            cout << "(+" << offset << ",-" << n-offset-length << ")" << endl;
        }
    }
}



//
//  Check the hash is 'dekHash'.
//  This check is intended to test for regressions
//  in the hufCompress() result or difference results across OSes.
// 
// The platform agnostic DEK hash of the huf compressed data for
// the set of numbers generated by fill4() and fill5().
// This DEK hash is determined from an aprior initial run of this
// test noting its value from the assert message compressVerify().
// 
#define  HUF_COMPRESS_DEK_HASH_FOR_FILL4_USHRT_MAX_PLUS_ONE 2013380646U
#define  HUF_COMPRESS_DEK_HASH_FOR_FILL4_N 213880353U
#define  HUF_COMPRESS_DEK_HASH_FOR_FILL5_N 2492982090U

void
compressVerify (const unsigned short raw[], 
                int n,
                const unsigned int dekHash)
{
    Array <char> compressed (3 * n + 4 * 65536);

    int nCompressed = hufCompress (raw, n, compressed);
    
    //
    // This hash algorithm proposed by Donald E. Knuth in 
    // The Art Of Computer Programming Volume 3,
    // under the topic of sorting and search chapter 6.4. 
    //
    unsigned int compressedHash = nCompressed;
    const unsigned char* cptr = reinterpret_cast<const unsigned char*>( (const char*) compressed);
    for (int i = 0; i < nCompressed; ++i)
    {
        compressedHash = 
            ((compressedHash << 5) ^ (compressedHash >> 27)) ^ (*cptr++);
    }

    cout << "verifying compressed checksum hash = " 
        << compressedHash << std::endl;

    if (compressedHash != dekHash)
    {
       cout << "hash verification failed. Got " << compressedHash << " expected " << dekHash << std::endl;
       const unsigned char* cptr = reinterpret_cast<const unsigned char*>( (const char*) compressed);
       for(int i = 0 ; i < nCompressed ; ++i )
       {
           cout << std::hex << (0xFF & (int) (*cptr++));
           if ( (i & 0xF) ==0 )
           {
              cout << '\n';
           }
           else
           {
              cout << ' ';
           }
       }
       cout << "\n";
    }

    assert (compressedHash == dekHash);
}


} // namespace


void
testHuf (const std::string&)
{
    try
    {
	cout << "Testing Huffman encoder" << endl;

	IMATH_NAMESPACE::Rand48 rand48 (0);

	const int N = 1000000;
	Array <unsigned short> raw (N);

	fill1 (raw, N, 1, rand48);	  // test various symbol distributions
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill1 (raw, N, 10, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill1 (raw, N, 100, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill1 (raw, N, 1000, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);

	fill2 (raw, N, 1, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill2 (raw, N, 10, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill2 (raw, N, 100, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill2 (raw, N, 1000, rand48);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);

	fill3 (raw, N, 0);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill3 (raw, N, 1);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill3 (raw, N, USHRT_MAX - 1);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);
	fill3 (raw, N, USHRT_MAX);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);

	fill4 (raw, USHRT_MAX + 1);
        compressVerify(raw, USHRT_MAX + 1, HUF_COMPRESS_DEK_HASH_FOR_FILL4_USHRT_MAX_PLUS_ONE);
	compressUncompress (raw, USHRT_MAX + 1);
	compressUncompressSubset (raw, USHRT_MAX + 1);
	fill4 (raw, N);
        compressVerify(raw, N, HUF_COMPRESS_DEK_HASH_FOR_FILL4_N);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);

	fill4 (raw, 0);
	compressUncompress (raw, 0);	// test small input data sets
	fill4 (raw, 1);
	compressUncompress (raw, 1);
	fill4 (raw, 2);
	compressUncompress (raw, 2);
	fill4 (raw, 3);
	compressUncompress (raw, 3);

	fill5 (raw, N);			// test run-length coding of code table
        compressVerify(raw, N, HUF_COMPRESS_DEK_HASH_FOR_FILL5_N);
	compressUncompress (raw, N);
	compressUncompressSubset (raw, N);

	cout << "ok\n" << endl;
    }
    catch (const std::exception &e)
    {
	cerr << "ERROR -- caught exception: " << e.what() << endl;
	assert (false);
    }
}
