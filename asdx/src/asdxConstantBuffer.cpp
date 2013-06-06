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
//      �R���X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer()
: m_pBuffer( nullptr )
{ /* DO_NOTHING */ }

//---------------------------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//---------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{ assert( m_pBuffer == nullptr ); }

//---------------------------------------------------------------------------------------------
//      �萔�o�b�t�@�𐶐����܂�.
//---------------------------------------------------------------------------------------------
bool ConstantBuffer::Create( ID3D11Device* pDevice, const size_t byteWidth )
{
    // NULL�`�F�b�N.
    if ( pDevice == nullptr )
    {
        // �G���[���O�o��.
        ELOG( "Error : Invalid Argument. pDevice is NULL." );

        // �ُ�I��.
        return false;
    }

    // �[���`�F�b�N.
    if ( byteWidth == 0 )
    {
        // �G���[���O�o��.
        ELOG( "Error : Invalid Argument. byteWidth is ZERO. " );

        // �ُ�I��.
        return false;
    }

    // �萔�o�b�t�@�̐ݒ�.
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof( D3D11_BUFFER_DESC ) );
    desc.Usage     = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = byteWidth;

    // �萔�o�b�t�@�𐶐�.
    HRESULT hr = pDevice->CreateBuffer( &desc, nullptr, &m_pBuffer );

    // �`�F�b�N.
    if ( FAILED( hr ) )
    {
        // �G���[���O�o��.
        ELOG( "Error : ID3D11Device::CreateBuffer() Failed." );

        // �ُ�I��.
        return false;
    }

    // �f�o�b�O�I�u�W�F�N�g����ݒ�.
    SetDebugObjectName( m_pBuffer, "asdx::ConstantBuffer" );

    // ����I��.
    return true;
}

//---------------------------------------------------------------------------------------------
//      �萔�o�b�t�@��������܂�.
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
//      �萔�o�b�t�@���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11Buffer* ConstantBuffer::GetBuffer()
{ return m_pBuffer; }

//---------------------------------------------------------------------------------------------
//      �萔�o�b�t�@���擾���܂�.
//---------------------------------------------------------------------------------------------
ID3D11Buffer* const ConstantBuffer::GetBuffer() const
{ return m_pBuffer; }

} // namesapce asdx
