//-------------------------------------------------------------------------------------------
// File : asdxConstantBuffer.h
// Desc : Constant Buffer Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __ASDX_CONSTANT_BUFFER_H__
#define __ASDX_CONSTANT_BUFFER_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <d3d11.h>


namespace asdx {

/////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class
/////////////////////////////////////////////////////////////////////////////////////////////
class ConstantBuffer
{
    //=======================================================================================
    // list of friend classes and methods.
    //=======================================================================================
    /* NOTHING */

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // private methods.
    //=======================================================================================
    ConstantBuffer  ( const ConstantBuffer& buffer );       // アクセス禁止.
    void operator = ( const ConstantBuffer& buffer );       // アクセス禁止.

protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    ID3D11Buffer*   m_pBuffer;

    //=======================================================================================
    // protected methods.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables.
    //=======================================================================================
    /* NOTHING */

    //---------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //---------------------------------------------------------------------------------------
    ConstantBuffer();

    //---------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //---------------------------------------------------------------------------------------
    virtual ~ConstantBuffer();

    //---------------------------------------------------------------------------------------
    //! @brief      定数バッファを生成します.
    //!
    //! @param [in]     pDevice         デバイスです.
    //! @param [in]     byteWidth       バイトサイズです.
    //! @retval true    生成に成功.
    //! @retval false   生成に失敗.
    //---------------------------------------------------------------------------------------
    bool Create( ID3D11Device* pDevice, const size_t byteWidth );

    //---------------------------------------------------------------------------------------
    //! @brief      定数バッファを解放します.
    //---------------------------------------------------------------------------------------
    void Release();

    //---------------------------------------------------------------------------------------
    //! @brief      定数バッファを取得します.
    //!
    //! @return     定数バッファを返却します.
    //---------------------------------------------------------------------------------------
    ID3D11Buffer* GetBuffer();

    //---------------------------------------------------------------------------------------
    //! @brief      定数バッファを取得します.
    //!
    //! @return     定数バッファを返却します.
    //---------------------------------------------------------------------------------------
    ID3D11Buffer* const GetBuffer() const;
};

}// namespace asdx

#endif//__ASDX_CONSTANT_BUFFER_H__