//-------------------------------------------------------------------------------------------------
// File : s3d_split.cpp
// Desc : Volume Splitter Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <algorithm>
#include <s3d_split.h>


namespace s3d {

//-------------------------------------------------------------------------------------------------
//      中間値による分割を行います.
//-------------------------------------------------------------------------------------------------
bool MedianSplit( std::vector<IShape*> shapes, u32& bestIndex, u32& bestAxis )
{
    if ( shapes.size() <= 2 )
    { return false; }

    auto bestCost = F32_MAX;
    bestAxis  = U32_MAX;
    bestIndex = U32_MAX;

    auto box = CreateMergedBox( &shapes[0], static_cast<u32>(shapes.size()) );

    auto size = box.maxi - box.mini;
    //size.x = fabs( size.x );
    //size.y = fabs( size.y );
    //size.z = fabs( size.z );

    u32 axis = 0;
    if ( size.x > size.y )
    { axis = ( size.x > size.z ) ? 0 : 2; }
    else
    { axis = ( size.y > size.z ) ? 1 : 2; }

    auto pivot = ( box.maxi.a[axis] + box.mini.a[axis] ) * 0.5f;

    //std::sort( 
    //    shapes.begin(),
    //    shapes.end(),
    //    [axis]( const IShape* l, const IShape* r)
    //    {
    //        auto boxL = l->GetBox();
    //        auto boxR = r->GetBox();
    //        assert( !boxL.isEmpty );
    //        assert( !boxR.isEmpty );
    //        auto cl = ( boxL.maxi.a[axis] + boxL.mini.a[axis] ) * 0.5f;
    //        auto cr = ( boxR.maxi.a[axis] + boxR.mini.a[axis] ) * 0.5f;
    //        return cl < cr;
    //    }
    //);

    u32 result = 0;
    for( size_t i=0; i<shapes.size(); ++i )
    {
        auto center = shapes[i]->GetCenter().a[axis];

        if ( center < pivot )
        {
            auto temp = shapes[i];
            shapes[i] = shapes[result];
            shapes[result] = temp;
            result++;
        }
    }

    if ( result == 0 || result == shapes.size() )
    { result = static_cast<u32>(shapes.size()) / 2; }

    bestIndex = result;
    bestAxis  = axis;

    return true;
}

//-------------------------------------------------------------------------------------------------
//      SAHによる分割を行います.
//-------------------------------------------------------------------------------------------------
bool SahSplit( std::vector<IShape*> shapes, u32& bestIndex, u32& bestAxis )
{
   if ( shapes.size() <= 4 )
    { return false; }

    auto bestCost  = F32_MAX;
    bestAxis  = U32_MAX;
    bestIndex = U32_MAX;

    for( auto axis = 0; axis < 3; ++axis )
    {
        // 現在の軸においてボックスの重心座標を用いてソートを行う.
        std::sort(
            shapes.begin(), 
            shapes.end(),
            [axis](const IShape* l, const IShape* r)
            {
                auto boxL = l->GetBox();
                auto boxR = r->GetBox();
                assert( !boxL.isEmpty );
                assert( !boxR.isEmpty );
                auto cl = ( boxL.maxi.a[axis] + boxL.mini.a[axis] ) * 0.5f;
                auto cr = ( boxR.maxi.a[axis] + boxR.mini.a[axis] ) * 0.5f;
                return cl > cr;
            }
        );


        std::vector<f32> leftArea (shapes.size() + 1, F32_MAX);
        std::vector<f32> rightArea(shapes.size() + 1, F32_MAX);

        BoundingBox s1Box;
        BoundingBox s2Box;

        // 左からなめる.
        std::vector<IShape*> s1;
        std::vector<IShape*> s2(shapes);
        for( u32 i=0; i<shapes.size(); ++i )
        {
            // 表面積を計算.
            leftArea[i] = SurfaceArea( s1Box );

            // 三角形 i を S2 から S1 へ移動.
            if ( !s2.empty() )
            {
                auto p = s2.back();
                s1.push_back(p);
                s2.pop_back();
                s1Box = BoundingBox::Merge( s1Box, p->GetBox() );
            }
        }

        // 右からなめる.
        for( auto i=static_cast<s32>(shapes.size())-1; i >=0; i-- )
        {
            // 表面積を計算.
            rightArea[i] = SurfaceArea( s2Box );

            // 三角形 i を S1 から S2 へ移動.
            if (!s1.empty())
            {
                auto p = s1.back();
                s2.push_back(p);
                s1.pop_back();
                s2Box = BoundingBox::Merge( s2Box, p->GetBox() );
            }

            if ( !s1.empty() && !s2.empty() )
            {
                // "Ray Tracing Deformable Scenes Using Dynamic Bounding Volume Hierarchies"
                //  ACM Transactions on Graphics 26(1), 2007
                // Equation(2) により評価する.
                auto cost = leftArea[i] * s1.size() + rightArea[i] * s2.size(); // 大小さえわかればいいので，定数を省いた.

                // 最良コストを更新.
                if (cost < bestCost)
                {
                    bestCost  = cost;
                    bestIndex = i;
                    bestAxis  = axis;
                }
            }
        }
    }


    // 葉ノードよりもよりパーティションが見つからなかった.
    if ( bestAxis == U32_MAX )
    { return false; }

    // "最良の軸" で S をソートする.
    std::sort( 
        shapes.begin(),
        shapes.end(),
        [bestAxis]( const IShape* l, const IShape* r)
        {
            auto boxL = l->GetBox();
            auto boxR = r->GetBox();
            assert( !boxL.isEmpty );
            assert( !boxR.isEmpty );
            auto cl = ( boxL.maxi.a[bestAxis] + boxL.mini.a[bestAxis] ) * 0.5f;
            auto cr = ( boxR.maxi.a[bestAxis] + boxR.mini.a[bestAxis] ) * 0.5f;
            return cl > cr;
        }
    );

    return true;
}

} // namespace s3d
