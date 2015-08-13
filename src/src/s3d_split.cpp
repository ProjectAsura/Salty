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

    for( auto axis = 0; axis < 3; ++axis )
    {
        std::vector<IShape*> axisSortedL(shapes);
        std::vector<IShape*> axisSortedR;

        // 現在の軸においてボックスの重心座標を用いてソートを行う.
        std::sort(
            axisSortedL.begin(), 
            axisSortedL.end(),
            [axis](const IShape* l, const IShape* r)
            {
                auto boxL = l->GetBox();
                auto boxR = r->GetBox();
                assert( !boxL.isEmpty );
                assert( !boxR.isEmpty );
                auto cl = ( boxL.maxi.a[axis] + boxL.mini.a[axis] ) * 0.5f;
                auto cr = ( boxR.maxi.a[axis] + boxR.mini.a[axis] ) * 0.5f;
                return cl < cr;
            }
        );

        auto idx = static_cast<u32>(axisSortedL.size()) / 2 - 1;
        assert( idx >= 0 );
        auto select = axisSortedL[idx];
        auto left   = axisSortedL[0];
        auto right  = axisSortedL[axisSortedL.size() - 1];
        auto dist   = ( select->GetCenter() - left->GetCenter () ).LengthSq()
                    + ( select->GetCenter() - right->GetCenter() ).LengthSq();

        if ( dist < bestCost )
        {
            bestCost  = dist;
            bestIndex = idx;
            bestAxis  = axis;
        }
    }

    // 葉ノードよりも良いパーティションが見つからなかった.
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
            return cl < cr;
        }
    );

    return true;
}

//-------------------------------------------------------------------------------------------------
//      SAHによる分割を行います.
//-------------------------------------------------------------------------------------------------
bool SahSplit( std::vector<IShape*> shapes, u32& bestIndex, u32& bestAxis )
{
    if ( shapes.size() <= 2 )
    { return false; }

    auto bestCost = F32_MAX;
    bestAxis  = U32_MAX;
    bestIndex = U32_MAX;

    for( auto axis = 0; axis < 3; ++axis )
    {
        std::vector<IShape*> axisSortedL(shapes);
        std::vector<IShape*> axisSortedR;

        // 現在の軸においてボックスの重心座標を用いてソートを行う.
        std::sort(
            axisSortedL.begin(), 
            axisSortedL.end(),
            [axis](const IShape* l, const IShape* r)
            {
                auto boxL = l->GetBox();
                auto boxR = r->GetBox();
                assert( !boxL.isEmpty );
                assert( !boxR.isEmpty );
                auto cl = ( boxL.maxi.a[axis] + boxL.mini.a[axis] ) * 0.5f;
                auto cr = ( boxR.maxi.a[axis] + boxR.mini.a[axis] ) * 0.5f;
                return cl < cr;
            }
        );

        std::vector<f32> leftArea (axisSortedL.size(), F32_MAX);
        std::vector<f32> rightArea(axisSortedL.size(), F32_MAX);

        axisSortedR.push_back( axisSortedL.back() );
        axisSortedL.pop_back();

        BoundingBox box = CreateMergedBox( &axisSortedR[0], static_cast<u32>(axisSortedR.size()) );

        for( auto i=static_cast<int>(axisSortedL.size()) - 1; i>=0; i-- )
        {
            rightArea[i] = SurfaceArea( box );
            auto pShape = axisSortedL.back();
            box = BoundingBox::Merge( box, pShape->GetBox() );

            axisSortedR.push_back( pShape );
            axisSortedL.pop_back();
        }

        axisSortedL.push_back( axisSortedR.back() );
        axisSortedR.pop_back();

        box = CreateMergedBox( &axisSortedL[0], static_cast<u32>(axisSortedL.size()) );

        u32 idx = 0;
        for( auto i=static_cast<int>(axisSortedR.size()) - 1; i>=0; i--, idx++ )
        {
            leftArea[idx] = SurfaceArea( box );
            auto pShape = axisSortedR.back();
            box = BoundingBox::Merge( box, pShape->GetBox() );

            axisSortedL.push_back( pShape );
            axisSortedR.pop_back();

            // "Ray Tracing Deformable Scenes Using Dynamic Bounding Volume Hierarchies"
            //  ACM Transactions on Graphics 26(1), 2007
            // Equation(2) により評価する.
            auto cost = leftArea[idx] * axisSortedL.size() + rightArea[idx] * axisSortedR.size(); // 大小さえわかればいいので，定数を省いた.

            if ( cost < bestCost )
            {
                bestCost  = cost;
                bestIndex = idx;
                bestAxis  = axis;
            }
        }
    }

    // 葉ノードよりも良いパーティションが見つからなかった.
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
            return cl < cr;
        }
    );

    return true;
}

} // namespace s3d
