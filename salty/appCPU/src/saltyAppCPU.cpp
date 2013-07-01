//------------------------------------------------------------------------------------
// File : saltyAppCPU.cpp
// Desc : Salty CPU Application.
// Copyright(c) Project Asura. All right reserved.
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <saltyAppCPU.h>
#include <asdxTimer.h>


namespace salty {

//////////////////////////////////////////////////////////////////////////////////////
// AppCPU class
//////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------
//      コンストラクタです.
//------------------------------------------------------------------------------------
AppCPU::AppCPU()
: m_Camera        ()
, m_FrameBuffer   ()
, m_ShapeList     ()
, m_Random        ( 56789 )
{
    /* DO_NOTHING */
}

//------------------------------------------------------------------------------------
//      デストラクタです.
//------------------------------------------------------------------------------------
AppCPU::~AppCPU()
{
    /* DO_NOTHING */
}

//------------------------------------------------------------------------------------
//      初期化処理です.
//------------------------------------------------------------------------------------
bool AppCPU::Init()
{
    FrameBuffer::Desc desc;
#if 1
    desc.Width  = 320;
    desc.Height = 240;
#else
    desc.Width  = 1280;
    desc.Height = 960;
#endif

    // フレームバッファ生成.
    if ( !m_FrameBuffer.Create( desc ) )
    {
        return false;
    }

    // フレームバッファをクリアする.
    //m_FrameBuffer.Clear( 0.392f, 0.584f, 0.929f, 1.0f );
    m_FrameBuffer.Clear( 0.0f, 0.0f, 0.0f, 1.0f );

    asdx::Vector3 pos   ( 0.0f, 0.0f, -4.5f );
    asdx::Vector3 target( 0.0f, 0.0f, 0.0f );
    asdx::Vector3 upward( 0.0f, 1.0f, 0.0f );

    // カメラ初期化.
    m_Camera.SetPosition( pos );
    m_Camera.SetTarget( target );
    m_Camera.SetUpward( upward );

    // 初期値を記憶させる.
    m_Camera.Preset();

    // ビュー行列更新.
    m_Camera.Update();

    // アスペクト比算出.
    f32 aspectRatio = static_cast<f32>( desc.Width ) / static_cast<f32>( desc.Height );

    // 行列計算.
    asdx::Matrix proj     = asdx::Matrix::CreatePerspectiveFieldOfViewRad( asdx::F_PI_OVER_4, aspectRatio, 1.0f, 1000.0f );
    asdx::Matrix view     = m_Camera.GetView();
    asdx::Matrix viewProj = view * proj;

    // 逆行列を求める.
    m_InvView     = asdx::Matrix::Invert( view );
    m_InvViewProj = asdx::Matrix::Invert( viewProj );

    m_LambertList.resize( 8 );
    m_LambertList[0] = Lambert( asdx::Vector3( 0.75f, 0.25f, 0.9f ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_LambertList[1] = Lambert( asdx::Vector3( 1.0f, 1.0f, 1.0f ), asdx::Vector3( 100.0f, 100.0f, 100.0f ) );
    m_LambertList[2] = Lambert( asdx::Vector3( 0.75f, 0.25f, 0.25f ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_LambertList[3] = Lambert( asdx::Vector3( 0.25f, 0.75f, 0.25f ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_LambertList[4] = Lambert( asdx::Vector3( 0.25f, 0.25f, 0.75f ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_LambertList[5] = Lambert( asdx::Vector3( 0.75f, 0.75f, 0.75f  ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_LambertList[6] = Lambert( asdx::Vector3( 0.75f, 0.75f, 0.75f ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );
    m_LambertList[7] = Lambert( asdx::Vector3( 0.1f, 0.1f, 0.1f ), asdx::Vector3( 0.0f, 0.0f, 0.0f ) );

    m_ShapeList.resize( 8 );
    m_ShapeList[0] = new Sphere( asdx::Vector3( 0.0f, 0.0f, 0.0f ), 1.0f );
    m_ShapeList[1] = new Box( asdx::Vector3( 0.0f, 3.0f, 0.0f ), asdx::Vector3( 0.5f, 0.125f, 0.5f ) );

    f32 size = 5.0f;
    m_ShapeList[2] = new Quad( 
        asdx::Vector3( -size, -size, -size ),
        asdx::Vector3( -size,  size, -size ),
        asdx::Vector3( -size,  size,  size ),
        asdx::Vector3( -size, -size,  size )
        );

    m_ShapeList[3] = new Quad( 
        asdx::Vector3( -size, -size,  size ),
        asdx::Vector3( -size,  size,  size ),
        asdx::Vector3(  size,  size,  size ),
        asdx::Vector3(  size, -size,  size ) );

    m_ShapeList[4] = new Quad( 
        asdx::Vector3(  size, -size,  size ),
        asdx::Vector3(  size,  size,  size ),
        asdx::Vector3(  size,  size, -size ),
        asdx::Vector3(  size, -size, -size ) );

    m_ShapeList[5] = new Quad( 
        asdx::Vector3( -size,  size,  size ),
        asdx::Vector3( -size,  size, -size ),
        asdx::Vector3(  size,  size, -size ),
        asdx::Vector3(  size,  size,  size ) );

    m_ShapeList[6] = new Quad( 
        asdx::Vector3( -size, -size, -size ),
        asdx::Vector3( -size, -size,  size ),
        asdx::Vector3(  size, -size,  size ),
        asdx::Vector3(  size, -size, -size ) );

#if 1
    m_ShapeList[7] = new Quad( 
        asdx::Vector3(  size, -size, -size ),
        asdx::Vector3(  size,  size, -size ),
        asdx::Vector3( -size,  size, -size ),
        asdx::Vector3( -size, -size, -size ) );
#endif


    //m_ShapeList[0] = new Triangle( 
    //    asdx::Vector3( -0.5f, -0.5f, 0.0f ),
    //    asdx::Vector3( 0.0f, 0.5f, 0.0f ),
    //    asdx::Vector3( 0.5f, -0.5f, 0.0f ) );
    //m_ShapeList[0] = new Box( asdx::Vector3( 0.0f, 0.0f, 0.0f ), asdx::Vector3( 1.0f, 1.0f, 1.0f ) );
    //m_ShapeList[0] = new Plane( asdx::Vector3( 1.0f, 0.0f, 0.0f ), 5.0f );

    for( u32 i=0; i<m_ShapeList.size(); ++i )
    {
        m_ShapeList[i]->SetMaterial( &m_LambertList[i] );
    }

    // 正常終了.
    return true;
}

//------------------------------------------------------------------------------------
//      終了処理です.
//------------------------------------------------------------------------------------
void AppCPU::Term()
{
    m_FrameBuffer.Release();
    m_ShapeList.clear();
}

//------------------------------------------------------------------------------------
//      交差判定を行います.
//------------------------------------------------------------------------------------
bool AppCPU::Intersects( const asdx::Ray& ray, HitRecord& record )
{
    record.Distance = F32_MAX;
    record.Normal   = asdx::Vector3( 0.0f, 0.0f, 0.0f );
    record.Position = asdx::Vector3( 0.0f, 0.0f, 0.0f );
    record.pShape   = nullptr;

    HitRecord result;

    for( u32 i=0; i<m_ShapeList.size(); ++i )
    {
        if ( m_ShapeList[i]->Intersects( ray, result ) )
        {
            if ( result.Distance > 0.0f && result.Distance < record.Distance )
            {
                record = result;
            }
        }
    }

    return ( record.pShape != nullptr );
}

//------------------------------------------------------------------------------------
//      レイを追跡します.
//------------------------------------------------------------------------------------
asdx::Vector4 AppCPU::TraceRay( const asdx::Ray& ray, s32 bounce )
{
    HitRecord       record;
    asdx::Vector3   accColor( 0.0f, 0.0f, 0.0f );
    asdx::Vector3   reflectance( 1.0f, 1.0f, 1.0f );
    asdx::Vector3   outdir;
    IMaterial*      pMaterial = nullptr;
    f32             P = 1.0f;

    asdx::Ray curRay( ray );

    for( ;; bounce++ )
    {
        if ( !Intersects( curRay, record ) )
        {
            break;
        }


        pMaterial = record.pShape->GetMaterial();
        assert( pMaterial != nullptr );

        //printf( "#1 reflectance = %f, %f, %f\n", reflectance.x, reflectance.y, reflectance.z );

#if defined(DEBUG) || defined(_DEBUG)
        if ( record.pShape->GetType() == 0 )
        {
            //printf_s( "detect box\n" );
        }
#endif//


        accColor += reflectance * pMaterial->GetEmissive();

        if ( bounce > MAX_BOUNCE )
        {
            asdx::Vector3 color = pMaterial->GetColor();
            P = asdx::Max( color.x, asdx::Max( color.y, color.z ) );

            f32 rnd = m_Random.GetAsF32();
            if ( rnd >= P )
            {
                break;
            }
        }

        asdx::Vector3 result = pMaterial->Sample( record.Normal, curRay.direction, outdir );
        //printf( "#2 result = %f, %f, %f \n#2reflectance = %f, %f, %f\n#2 P = %f\n", result.x, result.y, result.z,
        //    reflectance.x, reflectance.y, reflectance.z, P );
        reflectance *= result;
        reflectance /= P;

        //printf( "#3 reflectance = %f, %f, %f\n", reflectance.x, reflectance.y, reflectance.z );


        curRay.direction = outdir;
        curRay.position  = record.Position;
    }

    return asdx::Vector4( accColor, 1.0f );
}

//------------------------------------------------------------------------------------
//      プロセスです.
//------------------------------------------------------------------------------------
void AppCPU::Process()
{
    s32 width  = m_FrameBuffer.GetWidth();
    s32 height = m_FrameBuffer.GetHeight();
    s32 sample = 16;

    asdx::Vector3 pos( 0.0f, 0.0f, 0.0f );    // スクリーン上の位置.
    asdx::Vector3 eye( 0.0f, 0.0f, 0.0f );    // ビュー空間でのカメラ位置.
    asdx::Vector3 worldPos;                   // ワールド空間でのスクリーン上の位置.
    asdx::Vector4 orig = asdx::Vector3::Transform( eye, m_InvView );

    asdx::Ray ray;
    ray.position = m_Camera.GetPosition();

    f32 divW = 1.0f / width;
    f32 divH = 1.0f / height;
    f32 divS = 1.0f / sample;
    u32 index = 0;

    asdx::StopWatch perHeight;
    asdx::StopWatch total;

    total.Start();

    for( s32 y=0; y<height; ++y )
    {
        perHeight.Start();

        for( s32 x=0; x<width; ++x )
        {
            s32 index = y * width + x;
            Pixel pixel( 0.0f, 0.0f, 0.0f, 0.0f );

            for( s32 s=0; s<sample; ++s )
            {
                f32 fx = ( static_cast<f32>( x ) + m_Random.GetAsF32() );
                f32 fy = ( static_cast<f32>( y ) + m_Random.GetAsF32() );

                pos.x = -2.0f * fx * divW + 1.0f;
                pos.y = -2.0f * fy * divH + 1.0f;

                worldPos = asdx::Vector3::TransformCoordinate( pos, m_InvViewProj );

                ray.direction.x = worldPos.x - orig.x;
                ray.direction.y = worldPos.y - orig.y;
                ray.direction.z = worldPos.z - orig.z;
                ray.direction.Normalize();

                pixel += TraceRay( ray, 0 ) * divS;
            }
            m_FrameBuffer[ index ] += pixel;
        }
        perHeight.End();
        printf_s( "y = %d time = %f(sec)\n", y, perHeight.GetElapsedTimeSec() );
    }

    total.End();
    printf_s( "Total Rendering time = %f(sec)\n", total.GetElapsedTimeSec() );

    // フレームバッファの内容をBMPファイルとして出力
    m_FrameBuffer.ExportToBmpA( "test.bmp" );
}

//------------------------------------------------------------------------------------
//      実行します.
//------------------------------------------------------------------------------------
void AppCPU::Run()
{
    // 初期化処理.
    if ( Init() )
    {
        // プロセスを実行.
        Process();
    }

    // 終了処理.
    Term();
}


} // namespace salty