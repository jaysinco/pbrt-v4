///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2014, Industrial Light & Magic, a division of Lucas
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

#ifndef INCLUDED_IMF_DEEP_IMAGE_H
#define INCLUDED_IMF_DEEP_IMAGE_H

//----------------------------------------------------------------------------
//
//      class DeepImage
//
//      For an explanation of images, levels and channels,
//      see the comments in header file Image.h.
//
//----------------------------------------------------------------------------

#include "ImfDeepImageLevel.h"
#include "ImfImage.h"
#include "ImfUtilExport.h"

#include <ImfTileDescription.h>

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER


class DeepImage : public Image
{
  public:

    //
    // Constructors and destructor.
    // The default constructor constructs an image with an empty data
    // window level mode ONE_LEVEL and level rounding mode ROUND_DOWN.
    //

  	IMFUTIL_EXPORT 
    DeepImage();

    IMFUTIL_EXPORT 
  	DeepImage(const IMATH_NAMESPACE::Box2i &dataWindow,
               LevelMode levelMode = ONE_LEVEL,
               LevelRoundingMode levelRoundingMode = ROUND_DOWN);

    IMFUTIL_EXPORT 
  	virtual ~DeepImage();


    //
    // Accessing image levels by level number
    //

    IMFUTIL_EXPORT 
    virtual DeepImageLevel &        level(int l = 0);
    IMFUTIL_EXPORT 
    virtual const DeepImageLevel &  level(int l = 0) const;

    IMFUTIL_EXPORT 
    virtual DeepImageLevel &        level(int lx, int ly);
    IMFUTIL_EXPORT 
    virtual const DeepImageLevel &  level(int lx, int ly) const;

  protected:

    IMFUTIL_EXPORT 
  	virtual DeepImageLevel *
        newLevel (int lx, int ly, const IMATH_NAMESPACE::Box2i &dataWindow);
};


OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT

#endif
