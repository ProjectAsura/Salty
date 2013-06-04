//---------------------------------------------------------------------------------------------
// File : asdxTextRenderer.h
// Desc : Text Renderer Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------------------

#ifndef __ASDX_TEXT_RENDERER_H__
#define __ASDX_TEXT_RENDERER_H__

//---------------------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------------------
#include <d3d11.h>
#include <asdxMath.h>
#include <vector>


namespace asdx {

///////////////////////////////////////////////////////////////////////////////////////////////
// TextRenderer class
///////////////////////////////////////////////////////////////////////////////////////////////
class TextRenderer
{
    //========================================================================================
    // list of friend classes and methods.
    //========================================================================================
    /* NOTHING */

public:
    //////////////////////////////////////////////////////////////////////////////////////////
    // Glyph structure
    //////////////////////////////////////////////////////////////////////////////////////////
    struct Glyph
    {
        uint32_t    Character;      //!< 文字コードです.
        RECT        SubRect;        //!< 矩形です.
        float       OffsetX;        //!< X方向のオフセットです.
        float       OffsetY;        //!< Y方向のオフセットです.
        float       AdvanceX;       //!< 文字スペースです.
    };

    //========================================================================================
    // public variables.
    //========================================================================================
    /* NOTHING */

    //========================================================================================
    // public methods.
    //========================================================================================

    //----------------------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //----------------------------------------------------------------------------------------
    TextRenderer();

    //----------------------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //----------------------------------------------------------------------------------------
    virtual ~TextRenderer();

    //----------------------------------------------------------------------------------------
    //! @brief      初期化処理です.
    //!
    //! @param [in]     pDevice     デバイスです.
    //! @param [in]     filename    フォントファイル名です.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //----------------------------------------------------------------------------------------
    bool Init( ID3D11Device* pDevice, const wchar_t* filename );

    //----------------------------------------------------------------------------------------
    //! @brief      終了処理です.
    //----------------------------------------------------------------------------------------
    void Term();

    //----------------------------------------------------------------------------------------
    //! @brief      描画開始処理です.
    //----------------------------------------------------------------------------------------
    void Begin();

    //----------------------------------------------------------------------------------------
    //! @brief      文字列を描画します.
    //!
    //! @param [in]     text            描画する文字列です.
    //! @param [in]     position        描画開始位置です.
    //! @param [in]     color           文字カラーです.
    //! @param [in]     layDepth        レイヤーの深さです.
    //----------------------------------------------------------------------------------------
    void DrawString( wchar_t* text, FLOAT2 position, FLOAT4 color = FLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ), float layerDepth = 0.0f );

    //----------------------------------------------------------------------------------------
    //! @brief      描画終了処理です.
    //----------------------------------------------------------------------------------------
    void End( ID3D11DeviceContext* pDeviceContext );

    //----------------------------------------------------------------------------------------
    //! @brief      行間サイズを取得します.
    //!
    //! @return     行間サイズを返却します.
    //----------------------------------------------------------------------------------------
    float   GetLineSpace() const;

    //----------------------------------------------------------------------------------------
    //! @brief      行間サイズを設定します.
    //!
    //! @param [in]     lineSpace       設定する行間サイズです.
    //----------------------------------------------------------------------------------------
    void    SetLineSpace( float lineSpace );

    //----------------------------------------------------------------------------------------
    //! @brief      デフォルト文字を取得します.
    //!
    //! @return     デフォルト文字を返却します.
    //----------------------------------------------------------------------------------------
    wchar_t GetDefaultCharacter() const;

    //----------------------------------------------------------------------------------------
    //! @brief      デフォルト文字を設定します.
    //!
    //! @param [in]     character       設定するデフォルト文字です.
    //----------------------------------------------------------------------------------------
    void    SetDefaultCharacter( wchar_t character );

    //----------------------------------------------------------------------------------------
    //! @brief      指定された文字が含まれているかチェックします.
    //!
    //! @retval true    含まれています.
    //! @retval false   含まれていません.
    //----------------------------------------------------------------------------------------
    bool    ContainsCharacter( wchar_t character ) const;


protected:
    //////////////////////////////////////////////////////////////////////////////////////////
    // Vertex structure
    //////////////////////////////////////////////////////////////////////////////////////////
    struct Vertex
    {
        FLOAT3      Position;       //!< 位置座標です.
        FLOAT4      Color;          //!< 頂点カラーです.
        FLOAT2      TexCoord;       //!< テクスチャ座標です.

        static const size_t                     InputElementCount = 3;                  //!< 入力要素数です.
        static const D3D11_INPUT_ELEMENT_DESC   InputElements[ InputElementCount ];     //!< 入力要素です.
    };

    //========================================================================================
    // protected variables.
    //========================================================================================
    static const size_t     NUM_MAX_SPRITE        = 2048;       //!< 最大スプライト数です.
    static const size_t     NUM_VERTEX_PER_SPRITE = 4;          //!< 1スプライト当たりの頂点数です.
    static const size_t     NUM_INDEX_PER_SRPITE  = 6;          //!< 1スプライト当たりのインデックス数です.

    ID3D11VertexShader*         m_pVS;              //!< 頂点シェーダです.
    ID3D11PixelShader*          m_pPS;              //!< ピクセルシェーダです.
    ID3D11InputLayout*          m_pIL;              //!< 入力レイアウトです.
    ID3D11Buffer*               m_pVB;              //!< 頂点バッファです.
    ID3D11Buffer*               m_pIB;              //!< インデックスバッファです.
    ID3D11Buffer*               m_pCB;              //!< 定数バッファです.
    ID3D11Texture2D*            m_pTexture;         //!< テクスチャです.
    ID3D11ShaderResourceView*   m_pSRV;             //!< シェーダリソースビューです.
    ID3D11SamplerState*         m_pSmp;             //!< サンプラーステートです.
    std::vector<Glyph>          m_Glyphs;           //!< グリフです.
    Glyph const*                m_pDefaultGlyph;    //!< デフォルトグリフです.
    float                       m_LineSpace;        //!< 行間サイズです.
    size_t                      m_SpriteCount;      //!< 描画するスプライト数です.
    bool                        m_IsBeginDraw;

    FLOAT2 MeasureString( wchar_t const* text ) const;
    Glyph const* FindGlyph   ( wchar_t character ) const;
    void         DrawSprite( 
        ID3D11ShaderResourceView* pSRV,
        FLOAT2 position,
        RECT const* pSrcRect,
        FLOAT4 color,
        FLOAT2 origin,
        float layerDepth
    );

private:
    //========================================================================================
    // private variables.
    //========================================================================================
    /* NOTHING */

    //========================================================================================
    // private methods.
    //========================================================================================
    TextRenderer    ( const TextRenderer& value );      // アクセス禁止.
    void operator = ( const TextRenderer& value );      // アクセス禁止.
};

} // namespace asdx


#endif//__ASDX_TEXT_RENDERER_H__