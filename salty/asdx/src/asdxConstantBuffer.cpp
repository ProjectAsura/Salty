//---------------------------------------------------------------------------------------------
// File : asdxConstantBuffer.cpp
// Desc : Constant Buffer Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <asdxConstantBuffer.h>
#include <cassert>
#include <asdxUtil.h>
#include <asdxLog.h>

namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////
// ConstantBuffer class
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
//      コンストラクタです.
//---------------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer()
: m_pBuffer( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      デストラクタです.
//---------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{ assert( m_pBuffer == nullptr ); }

//---------------------------------------------------------------------------------------------
//      定数バッファを生成します.
//---------------------------------------------------------------------------------------------
bool ConstantBuffer::Create( ID3D11Device* pDevice, const size_t byteWidth )
{
    // NULLチェック.
    if ( pDevice == nullptr )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid Argument. pDevice is NULL." );

        // 異常終了.
        return false;
    }

    // ゼロチェック.
    if ( byteWidth == 0 )
    {
        // エラーログ出力.
        ELOG( "Error : Invalid Argument. byteWidth is ZERO. " );

        // 異常終了.
        return false;
    }

    // 定数バッファの設定.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage     = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = byteWidth;

    // 定数バッファを生成.
    HRESULT hr = pDevice->CreateBuffer( &desc, nullptr, &m_pBuffer );

    // チェック.
    if ( FAILED( hr ) )
    {
        // エラーログ出力.
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

        // 異常終了.
        return false;
    }

    // デバッグオブジェクト名を設定.
    SetDebugObjectName( m_pBuffer, "asdx::ConstantBuffer" );

    // 正常終了.
    return true;
}

//---------------------------------------------------------------------------------------------
//      定数バッファを解放します.
//---------------------------------------------------------------------------------------------
void ConstantBuffer::Release()
{
    if ( m_pBuffer )
    {
        m_pBuffer->Release();
        m_pBuffer = nullptr;
    }
}

//---------------------------------------------------------------------------------------------
//      定数バッファを取得します.
//---------------------------------------------------------------------------------------------
ID3D11Buffer* ConstantBuffer::GetBuffer()
{ return m_pBuffer; }

//---------------------------------------------------------------------------------------------
//      定数バッファを取得します.
//---------------------------------------------------------------------------------------------
ID3D11Buffer* const ConstantBuffer::GetBuffer() const
{ return m_pBuffer; }

} // namesapce asdx
