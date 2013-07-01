//---------------------------------------------------------------------------------
// File : saltyMaterial.cpp
// Desc : Salty Material Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------------
#include <asdxRandom.h>
#include <saltyMaterial.h>


namespace /* anonymous */ {

static asdx::Random    g_Random( 123456789 );

} // namespace /* anonymous */


namespace salty {

Lambert::Lambert()
: Color ( 0.0f, 0.0f, 0.0f )
, Emissive( 0.0f, 0.0f, 0.0f )
{
}

Lambert::Lambert( const asdx::Vector3 color, const asdx::Vector3 emissive)
: Color ( color )
, Emissive( emissive )
{
}

asdx::Vector3 Lambert::Sample
(
    const asdx::Vector3& normal,
    const asdx::Vector3& indir,
    asdx::Vector3&       outdir
)
{
    f32 theta = asdx::F_PI  * g_Random.GetAsF32();
    f32 phi   = asdx::F_2PI * g_Random.GetAsF32();

    outdir.x = sinf( theta ) * cosf( phi );
    outdir.y = sinf( theta ) * sinf( phi );
    outdir.z = cosf( theta );

    if ( asdx::Vector3::Dot( normal, outdir ) < 0.0f )
    {
        outdir.x = -outdir.x;
        outdir.y = -outdir.y;
        outdir.z = -outdir.z;
    }

    return Color;
}

asdx::Vector3 Lambert::GetColor()
{
    return Color;
}

asdx::Vector3 Lambert::GetEmissive()
{
    return Emissive;
}

/////////////////////////////////////////////////////////////////////////////////////
//// Lambert structure
/////////////////////////////////////////////////////////////////////////////////////
//
////---------------------------------------------------------------------------------
////      コンストラクタです.
////---------------------------------------------------------------------------------
//Lambert::Lambert()
//{
//    /* DO_NOTHING */
//}
//
////---------------------------------------------------------------------------------
////      引数つきコンストラクタです.
////---------------------------------------------------------------------------------
//Lambert::Lambert( const asdx::Vector3 diffuse, const f32 alpha )
//: Diffuse( diffuse )
//, Alpha  ( alpha )
//{
//    /* DO_NOTHING */
//}
//
////---------------------------------------------------------------------------------
////      BRDFを計算します.
////---------------------------------------------------------------------------------
//void Lambert::ComputeBRDF
//(
//    const asdx::Vector3& normal,
//    const asdx::Vector3& input,
//    asdx::Vector3&       output,
//    asdx::Vector4&       color
//)
//{
//    //register f32 dot = asdx::Max( asdx::Vector3::Dot( input, normal ), 0.0f );
//    //color.x = Diffuse.x * dot;
//    //color.y = Diffuse.y * dot;
//    //color.z = Diffuse.z * dot;
//    //color.w = Alpha;
//
//    //register f32 rnd = g_Random.GetAsF32();
//    color.x = Diffuse.x;
//    color.y = Diffuse.y;
//    color.z = Diffuse.z;
//    color.w = Alpha;
//}
//
//////////////////////////////////////////////////////////////////////////////////////
//// Phong structure
//////////////////////////////////////////////////////////////////////////////////////
//
////---------------------------------------------------------------------------------
////      コンストラクタです.
////---------------------------------------------------------------------------------
//Phong::Phong()
//{
//    /* DO_NOTHING */
//}
//
////---------------------------------------------------------------------------------
////      引数つきコンストラクタです.
////---------------------------------------------------------------------------------
//Phong::Phong
//(
//    const asdx::Vector3 diffuse,
//    const f32           alpha,
//    const asdx::Vector3 specular,
//    const f32           power 
//)
//: Diffuse   ( diffuse )
//, Alpha     ( alpha )
//, Specular  ( specular )
//, Power     ( power )
//{
//    /* DO_NOTHING */
//}
//
////---------------------------------------------------------------------------------
////      BRDFを計算します.
////---------------------------------------------------------------------------------
//void Phong::ComputeBRDF
//(
//    const asdx::Vector3& normal,
//    const asdx::Vector3& input,
//    asdx::Vector3&       output,
//    asdx::Vector4&       color
//)
//{
//}
//
//////////////////////////////////////////////////////////////////////////////////////
//// CookTorrance structure
//////////////////////////////////////////////////////////////////////////////////////
//
////---------------------------------------------------------------------------------
////      コンストラクタです.
////---------------------------------------------------------------------------------
//CookTorrance::CookTorrance()
//{
//    /* DO_NOTHING */
//}
//
////---------------------------------------------------------------------------------
////      引数つきコンストラクタです.
////---------------------------------------------------------------------------------
//CookTorrance::CookTorrance
//(
//    const asdx::Vector3 diffuse,
//    const f32           alpha,
//    const asdx::Vector3 specular,
//    const f32           power,
//    const f32           refractive
//)
//: Diffuse   ( diffuse )
//, Alpha     ( alpha )
//, Specular  ( specular )
//, Power     ( power )
//, Refractive( refractive )
//{
//    /* DO_NOTHING */
//}
//
////---------------------------------------------------------------------------------
////      BRDFを計算します.
////---------------------------------------------------------------------------------
//void CookTorrance::ComputeBRDF
//(
//    const asdx::Vector3& normal,
//    const asdx::Vector3& input,
//    asdx::Vector3&       output,
//    asdx::Vector4&       color 
//)
//{
//}


} // namespace salty