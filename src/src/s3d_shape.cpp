//--------------------------------------------------------------------------------------
// File : s3d_shape.cpp
// Desc : Shape Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------------
//
////--------------------------------------------------------------------------------------
//// Includes
////--------------------------------------------------------------------------------------
//#include <s3d_shape.h>
//
//
//namespace s3d {
//
////---------------------------------------------------------------------------
////      マージしたバウンディングボックスを生成します.
////---------------------------------------------------------------------------
//BoundingBox CreateMergedBox( IShape** ppShapes, const u32 numShapes )
//{
//    if ( numShapes == 0 || ppShapes == nullptr )
//    { return BoundingBox(); }
//
//    BoundingBox box = ppShapes[0]->GetBox();
//
//    for( u32 i=1; i<numShapes; ++i )
//    { box = BoundingBox::Merge( box, ppShapes[i]->GetBox() ); }
//
//    return box;
//}
//
////---------------------------------------------------------------------------
////      マージしたバウンディングボックスを生成します(メッシュ用).
////---------------------------------------------------------------------------
//BoundingBox CreateMergedBox( Triangle* pTriangles, const u32 numTriangles )
//{
//    if ( numTriangles == 0 || pTriangles == nullptr )
//    { return BoundingBox(); }
//
//    BoundingBox box = pTriangles[0].GetBox();
//
//    for( u32 i=1; i<numTriangles; ++i )
//    { box = BoundingBox::Merge( box, pTriangles[i].GetBox() ); }
//
//    return box;
//}
/////////////////////////////////////////////////////////////////////////////////////////
//// Quad structure
/////////////////////////////////////////////////////////////////////////////////////////
//
////-------------------------------------------------------------------------------------
////      コンストラクタです.
////-------------------------------------------------------------------------------------
//Quad::Quad
//(
//    const Vertex&       _v0,
//    const Vertex&       _v1,
//    const Vertex&       _v2,
//    const Vertex&       _v3,
//    const IMaterial*    _pMaterial
//)
//: v0        ( _v0 )
//, v1        ( _v1 )
//, v2        ( _v2 )
//, v3        ( _v3 )
//, pMaterial ( _pMaterial )
//{ /* DO_NOTHING */ }
//
//
////-------------------------------------------------------------------------------------
////      コンストラクタです.
////-------------------------------------------------------------------------------------
//Quad::Quad
//(
//    const Face4&        _face,
//    const IMaterial*    _pMaterial
//)
//: v0        ( _face.v0.pos, _face.v0.texcoord, _face.normal )
//, v1        ( _face.v1.pos, _face.v1.texcoord, _face.normal )
//, v2        ( _face.v2.pos, _face.v2.texcoord, _face.normal )
//, v3        ( _face.v3.pos, _face.v3.texcoord, _face.normal )
//, pMaterial ( _pMaterial )
//{ /* DO_NOTHING */ }
//
////--------------------------------------------------------------------------------
////      三角形ポリゴンと衝突しているかチェックします.
////--------------------------------------------------------------------------------
//bool Quad::IsHitTriangle
//(
//    const Ray&      ray,
//    const Vertex&   a,
//    const Vertex&   b,
//    const Vertex&   c,
//    HitRecord&      record
//) const
//{
//    Vector3 e1 = b.pos - a.pos;
//    Vector3 e2 = c.pos - a.pos;
//    Vector3 s1 = Vector3::Cross( ray.dir, e2 );
//    f32 div = Vector3::Dot( s1, e1 );
//    
//    if ( -FLT_EPSILON <= div && div <= FLT_EPSILON )
//    { return false; }
//        
//    Vector3 d = ray.pos - a.pos;
//    f32 beta = Vector3::Dot( d, s1 ) / div;
//    if ( beta <= 0.0 || beta >= 1.0 )
//    { return false; }
//    
//    Vector3 s2 = Vector3::Cross( d, e1 );
//    f32 gamma = Vector3::Dot( ray.dir, s2 ) / div;
//    if ( gamma <= 0.0 || ( beta + gamma ) >= 1.0 )
//    { return false; }
//    
//    f32 dist = Vector3::Dot( e2, s2 ) / div;
//    
//    if ( dist < F_HIT_MIN || dist > F_HIT_MAX )
//    { return false; }
//
//    if ( dist > record.distance )
//    { return false; }
//
//    record.position  = ray.pos + ray.dir * dist;
//    record.distance  = dist;
//    record.pShape    = this;
//    record.pMaterial = pMaterial;
//
//    f32 alpha = 1.0f - beta - gamma;
//    record.normal   = Vector3(
//        a.normal.x * alpha + b.normal.x * beta + c.normal.x * gamma,
//        a.normal.y * alpha + b.normal.y * beta + c.normal.y * gamma,
//        a.normal.z * alpha + b.normal.z * beta + c.normal.z * gamma );
//    record.normal.Normalize();
//
//    record.texcoord = Vector2(
//        a.texcoord.x * alpha + b.texcoord.x * beta + c.texcoord.x * gamma,
//        a.texcoord.y * alpha + b.texcoord.y * beta + c.texcoord.y * gamma );
//
//    return true;
//}
//
////-------------------------------------------------------------------------------------
////      交差判定を行います.
////-------------------------------------------------------------------------------------
//bool Quad::IsHit( const Ray& ray, HitRecord& record ) const
//{
//    if ( IsHitTriangle( ray, v0, v1, v2, record ) )
//    { return true; }
//    
//    if ( IsHitTriangle( ray, v2, v3, v0, record ) )
//    { return true; }
//
//    return false;
//}
//
////-------------------------------------------------------------------------------------
////      バウンディングボックスを取得します.
////-------------------------------------------------------------------------------------
//BoundingBox Quad::GetBox() const
//{
//    Vector3 min = v0.pos;
//    Vector3 max = v0.pos;
//    min = Vector3::Min( min, v1.pos );
//    min = Vector3::Min( min, v2.pos );
//    min = Vector3::Min( min, v3.pos );
//
//    max = Vector3::Max( max, v1.pos );
//    max = Vector3::Max( max, v2.pos );
//    max = Vector3::Max( max, v3.pos );
//
//    return BoundingBox( min, max );
//}
//
////-------------------------------------------------------------------------------------
////      基本図形であるかどうかチェックします.
////-------------------------------------------------------------------------------------
//bool Quad::IsPrimitive() const
//{ return true; }
//
////-------------------------------------------------------------------------------------
////      中心座標を取得します.
////-------------------------------------------------------------------------------------
//Vector3 Quad::GetCenter() const
//{ return ( v0.pos + v1.pos + v2.pos + v3.pos ) / 4.0f; }
//
//
/////////////////////////////////////////////////////////////////////////////////////////
//// Leaf structure
/////////////////////////////////////////////////////////////////////////////////////////
//

//} // namespace s3d

