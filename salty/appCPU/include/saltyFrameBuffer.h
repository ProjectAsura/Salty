//------------------------------------------------------------------------------
// File : saltyFrameBuffer.h
// Desc : Salty Frame Buffer Module.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------

#ifndef __SALTY_FRAMEBUFFER_H__
#define __SALTY_FRAMEBUFFER_H__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <asdxTypedef.h>
#include <saltyPixel.h>

namespace salty {

////////////////////////////////////////////////////////////////////////////////
// FrameBuffer class
////////////////////////////////////////////////////////////////////////////////
class FrameBuffer
{
    //==========================================================================
    // list of friend classes and methods.
    //==========================================================================
    /* NOTHING */

public:
    ////////////////////////////////////////////////////////////////////////////
    // Desc structure
    ////////////////////////////////////////////////////////////////////////////
    struct Desc
    {
        u32     Width;      //!< 横幅です.
        u32     Height;     //!< 縦幅です.
    };

    //==========================================================================
    // public variables.
    //==========================================================================
    /* NOTHING */

    //==========================================================================
    // public methods.
    //==========================================================================
    FrameBuffer();
    virtual ~FrameBuffer();

    bool    Create  ( const Desc& desc );
    void    Release ();
    void    Clear   ( f32 r, f32 g, f32 b, f32 a );

    Pixel&       operator []( u32 index );
    const Pixel& operator []( u32 index ) const;

    Pixel&  GetPixel        ( u32 index );
    f32&    GetPixelR       ( u32 index );
    f32&    GetPixelG       ( u32 index );
    f32&    GetPixelB       ( u32 index );
    f32&    GetPixelA       ( u32 index );
    u32     GetWidth        () const;
    u32     GetHeight       () const;
    u32     GetPixelCount   () const;

    bool    ExportToBmpA( const char8*  filename );
    bool    ExportToBmpW( const char16* filename );
    bool    ExportToTgaA( const char8*  filename );
    bool    ExportToTgaW( const char16* filename );

protected:
    //==========================================================================
    // protected variables.
    //==========================================================================
    u32     m_Width;
    u32     m_Height;
    u32     m_PixelCount;
    Pixel*  m_pPixels;

    //==========================================================================
    // protected methods.
    //==========================================================================
    /* NOTHING */

private:
    //==========================================================================
    // private variables.
    //==========================================================================
    /* NOTHING */

    //==========================================================================
    // private methods.
    //==========================================================================
    FrameBuffer     ( const FrameBuffer& buffer );
    void operator = ( const FrameBuffer& buffer );
};

} // namespace salty


#endif//__SALTY_FRAMEBUFFER_H__