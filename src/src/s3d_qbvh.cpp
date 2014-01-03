//--------------------------------------------------------------------------------
// File : s3d_qbvh.cpp
// Desc : Quad Bounding Volume Hierarchy Module.
// Copyright(c) Project Asura. All right reserved.
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------------
#include <s3d_qbvh.h>
#include <new>
#include <cstdio>


namespace /* anonymous */ {

//---------------------------------------------------------------------------
//      �������܂�.
//---------------------------------------------------------------------------
s32 Split( s3d::IShape** ppShapes, s32 size, f64 pivotVal, s32 axis )
{
    s3d::BoundingBox bbox;
    f64 centroid;
    s32 result = 0;

    for( s32 i=0; i<size; ++i )
    {
        // AABB���擾.
        bbox = ppShapes[i]->GetBox();

        // ���S���擾.
        centroid = ( bbox.min.a[axis] + bbox.max.a[axis] ) / 2.0;

        // �s�{�b�g�Ɣ�r.
        if ( centroid < pivotVal )
        {
            s3d::IShape* pTemp = ppShapes[ i ];

            ppShapes[ i ]      = ppShapes[ result ];
            ppShapes[ result ] = pTemp;
            result++;
        }
    }

    if ( result == 0 || result == size )
    { result = size / 2; }

    return result;
}

//---------------------------------------------------------------------------
//      �������܂�(���b�V���p).
//---------------------------------------------------------------------------
s32 Split( s3d::Triangle* pShapes, s32 size, f64 pivotVal, s32 axis )
{
    s3d::BoundingBox bbox;
    f64 centroid;
    s32 result = 0;

    for( s32 i=0; i<size; ++i )
    {
        // AABB���擾.
        bbox = pShapes[i].GetBox();

        // ���S���擾.
        centroid = ( bbox.min.a[axis] + bbox.max.a[axis] ) / 2.0;

        // �s�{�b�g�Ɣ�r.
        if ( centroid < pivotVal )
        {
            s3d::Triangle pTemp = pShapes[ i ];

            pShapes[ i ]      = pShapes[ result ];
            pShapes[ result ] = pTemp;
            result++;
        }
    }

    if ( result == 0 || result == size )
    { result = size / 2; }

    return result;
}


//---------------------------------------------------------------------------
//      �}�[�W�����o�E���f�B���O�{�b�N�X�𐶐����܂�.
//---------------------------------------------------------------------------
s3d::BoundingBox CreateMergedBox
(
    s3d::IShape**   ppShapes,
    const u32       numShapes
)
{
    if ( numShapes == 0 )
    { return s3d::BoundingBox(); }

    s3d::BoundingBox box = ppShapes[0]->GetBox();

    for( u32 i=1; i<numShapes; ++i )
    { box = s3d::BoundingBox::Merge( box, ppShapes[i]->GetBox() ); }

    return box;
}

//---------------------------------------------------------------------------
//      �}�[�W�����o�E���f�B���O�{�b�N�X�𐶐����܂�(���b�V���p).
//---------------------------------------------------------------------------
s3d::BoundingBox CreateMergedBox
(
    s3d::Triangle*  pTriangles,
    const u32       numTriangles
)
{
    if ( numTriangles == 0 )
    { return s3d::BoundingBox(); }

    s3d::BoundingBox box = pTriangles[0].GetBox();

    for( u32 i=1; i<numTriangles; ++i )
    { box = s3d::BoundingBox::Merge( box, pTriangles[i].GetBox() ); }

    return box;
}

//---------------------------------------------------------------------------
//      �ł��������������C���f�b�N�X���擾���܂�.
//---------------------------------------------------------------------------
s32 GetAxisIndex( s3d::Vector3 size )
{
    s32 axis = 0;
    if ( size.x > size.y )
    { axis = ( size.x > size.z ) ? 0 : 2; }
    else
    { axis = ( size.y > size.z ) ? 1 : 2; }

    return axis;
}

} // namespace /* anonymous */


namespace s3d {

////////////////////////////////////////////////////////////////////////////
// QBVH structure
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
//      �R���X�g���N�^�ł�.
//--------------------------------------------------------------------------
QBVH::QBVH()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
    pShape[2] = nullptr;
    pShape[3] = nullptr;
    pHitFunc = nullptr;
}

//--------------------------------------------------------------------------
//      �����t���R���X�g���N�^�ł�.
//--------------------------------------------------------------------------
QBVH::QBVH
(
    bool node,
    IShape* pShape1,
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4
) 
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    pShape[2] = pShape3;
    pShape[3] = pShape4;

    BoundingBox box0 = pShape1->GetBox();
    BoundingBox box1 = pShape2->GetBox();
    BoundingBox box2 = ( pShape3 == nullptr ) ? BoundingBox() : pShape3->GetBox();
    BoundingBox box3 = ( pShape4 == nullptr ) ? BoundingBox() : pShape4->GetBox();

    box = BoundingBoxQuad( box0, box1, box2, box3 );
    pHitFunc = ( node ) ? HitFuncNode : HitFuncLeaf;
}

//--------------------------------------------------------------------------
//      �����t���R���X�g���N�^�ł�.
//--------------------------------------------------------------------------
QBVH::QBVH
(
    bool node,
    IShape* pShape1, 
    IShape* pShape2,
    IShape* pShape3,
    IShape* pShape4,
    const BoundingBoxQuad& quadBox
): box( quadBox )
{
    pShape[0] = pShape1;
    pShape[1] = pShape2;
    pShape[2] = pShape3;
    pShape[3] = pShape4;
    pHitFunc = ( node ) ? HitFuncNode : HitFuncLeaf;
}

//--------------------------------------------------------------------------
//      �f�X�g���N�^�ł�.
//--------------------------------------------------------------------------
QBVH::~QBVH()
{
    pShape[0] = nullptr;
    pShape[1] = nullptr;
    pShape[2] = nullptr;
    pShape[3] = nullptr;
}

//--------------------------------------------------------------------------
//      ����������s���܂�.
//--------------------------------------------------------------------------
bool QBVH::IsHit( const Ray& ray, HitRecord& record ) const
{ return (*pHitFunc)( this, ray, record ); }

//--------------------------------------------------------------------------
//      �}�e���A�����擾���܂�.
//--------------------------------------------------------------------------
IMaterial* QBVH::GetMaterial() const
{ return nullptr; }

//--------------------------------------------------------------------------
//      �o�E���f�B���O�{�b�N�X���擾���܂�.
//--------------------------------------------------------------------------
BoundingBox QBVH::GetBox() const
{ return box.GetBox(); }

//--------------------------------------------------------------------------
//      ��{�}�`���ǂ������肵�܂�.
//--------------------------------------------------------------------------
bool QBVH::IsPrimitive() const
{ return false; }

//--------------------------------------------------------------------------
//      �u�����`���\�z���܂�.
//--------------------------------------------------------------------------
IShape* QBVH::BuildBranch( IShape** ppShapes, const u32 numShapes )
{
    // ���̂܂ܕԋp.
    if ( numShapes == 1 )
    { return ppShapes[0]; }

    // 16byte�A���C�����g�Ń��������m��.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(QBVH), 16 );

    // �t�Ƃ��Đ���.
    if ( numShapes == 2 )
    { return new(pBuf) QBVH( false, ppShapes[0], ppShapes[1], nullptr, nullptr ); }
    else if ( numShapes == 3 )
    { return new(pBuf) QBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], nullptr ); }
    else if ( numShapes == 4 )
    { return new(pBuf) QBVH( false, ppShapes[0], ppShapes[1], ppShapes[2], ppShapes[3] ); }

    // AABB�����߂�.
    BoundingBox bbox = CreateMergedBox( ppShapes, numShapes );

    // �s�{�b�g�����߂�.
    Vector3 pivot = ( bbox.max + bbox.min ) / 2.0;

    // AABB�̊e�ӂ̒��������߂�.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = GetAxisIndex( size );

    // ���Ԓl.
    s32 midPoint = Split( ppShapes, numShapes, pivot.a[axis], axis );

    // �X�ɕ������邽�߂�AABB�����߂�.
    BoundingBox bboxL = CreateMergedBox( ppShapes, midPoint );
    BoundingBox bboxR = CreateMergedBox( &ppShapes[midPoint], numShapes - midPoint );

    // AABB�̊e�ӂ̒��������߂�.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // �X�ɕ�������s�{�b�g�����߂�.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) / 2.0;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) / 2.0;

    // ��������.
    s32 midPointL = Split( ppShapes, midPoint, pivotL.a[axisL], axisL );
    s32 midPointR = Split( &ppShapes[midPoint], numShapes - midPoint, pivotR.a[axisR], axisR );

    IShape** ppChild[4] = {
        &ppShapes[0],
        &ppShapes[midPointL],
        &ppShapes[midPoint],
        &ppShapes[midPoint + midPointR]
    };
    u32 numChild[4] = {
        midPointL,
        midPoint - midPointL,
        midPointR,
        numShapes - (midPoint + midPointR)
    };

    // �u�����`�𐶐�.
    IShape* child0 = BuildBranch( ppChild[0], numChild[0] );
    IShape* child1 = BuildBranch( ppChild[1], numChild[1] );
    IShape* child2 = BuildBranch( ppChild[2], numChild[2] );
    IShape* child3 = BuildBranch( ppChild[3], numChild[3] );

    // �`���C���h��AABB�����߂�.
    BoundingBox box0 = CreateMergedBox( ppChild[0], numChild[0] );
    BoundingBox box1 = CreateMergedBox( ppChild[1], numChild[1] );
    BoundingBox box2 = CreateMergedBox( ppChild[2], numChild[2] );
    BoundingBox box3 = CreateMergedBox( ppChild[3], numChild[3] );

    // �m�[�h�Ƃ��Đ���.
    return new(pBuf) QBVH( true, child0, child1, child2, child3, BoundingBoxQuad( box0, box1, box2, box3 ) );
}

//--------------------------------------------------------------------------
//      �u�����`���\�z���܂�(���b�V���p).
//--------------------------------------------------------------------------
IShape* QBVH::BuildBranch( Triangle* pShapes, const u32 numShapes )
{
    // ���̂܂ܕԋp.
    if ( numShapes == 1 )
    { return (IShape*)&pShapes[0]; }

    // 16byte�A���C�����g�Ń��������m��.
    u8* pBuf = (u8*)_aligned_malloc( sizeof(QBVH), 16 );

    // �t�Ƃ��Đ���.
    if ( numShapes == 2 )
    { return new(pBuf) QBVH( false, &pShapes[0], &pShapes[1], nullptr, nullptr ); }
    else if ( numShapes == 3 )
    { return new(pBuf) QBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], nullptr ); }
    else if ( numShapes == 4 )
    { return new(pBuf) QBVH( false, &pShapes[0], &pShapes[1], &pShapes[2], &pShapes[3] ); }

    // AABB�����߂�.
    BoundingBox bbox = CreateMergedBox( pShapes, numShapes );

    // �s�{�b�g�����߂�.
    Vector3 pivot = ( bbox.max + bbox.min ) * 0.5f;

    // AABB�̊e�ӂ̒��������߂�.
    Vector3 size  = bbox.max - bbox.min;
    s32 axis = GetAxisIndex( size );

    // ���Ԓl.
    s32 midPoint = Split( pShapes, numShapes, pivot.a[axis], axis );

    // �X�ɕ������邽�߂�AABB�����߂�.
    BoundingBox bboxL = CreateMergedBox( pShapes, midPoint );
    BoundingBox bboxR = CreateMergedBox( &pShapes[midPoint], numShapes - midPoint );

    // AABB�̊e�ӂ̒��������߂�.
    Vector3 sizeL = bboxL.max - bboxL.min;
    Vector3 sizeR = bboxR.max - bboxR.min;
    s32 axisL = GetAxisIndex( sizeL );
    s32 axisR = GetAxisIndex( sizeR );

    // �X�ɕ������邽�߂̃s�{�b�g�����߂�.
    Vector3 pivotL = ( bboxL.max + bboxL.min ) * 0.5f;
    Vector3 pivotR = ( bboxR.max + bboxR.min ) * 0.5f;

    // ��������.
    s32 midPointL = Split( pShapes, midPoint, pivotL.a[axisL], axisL );
    s32 midPointR = Split( &pShapes[midPoint], numShapes - midPoint, pivotR.a[axisR], axisR );

    Triangle* ppChild[4] = {
        &pShapes[0],
        &pShapes[midPointL],
        &pShapes[midPoint],
        &pShapes[midPoint + midPointR]
    };
    u32 numChild[4] = {
        midPointL,
        midPoint - midPointL,
        midPointR,
        numShapes - (midPoint + midPointR)
    };

    // �u�����`�𐶐�.
    IShape* child0 = BuildBranch( ppChild[0], numChild[0] );
    IShape* child1 = BuildBranch( ppChild[1], numChild[1] );
    IShape* child2 = BuildBranch( ppChild[2], numChild[2] );
    IShape* child3 = BuildBranch( ppChild[3], numChild[3] );

    // �`���C���h��AABB�����߂�.
    BoundingBox box0 = CreateMergedBox( ppChild[0], numChild[0] );
    BoundingBox box1 = CreateMergedBox( ppChild[1], numChild[1] );
    BoundingBox box2 = CreateMergedBox( ppChild[2], numChild[2] );
    BoundingBox box3 = CreateMergedBox( ppChild[3], numChild[3] );

    // �m�[�h�Ƃ��Đ���.
    return new(pBuf) QBVH( true, child0, child1, child2, child3, BoundingBoxQuad( box0, box1, box2, box3 ) );
}

//--------------------------------------------------------------------------
//      �u�����`��j�����܂�.
//--------------------------------------------------------------------------
void QBVH::DestroyBranch( QBVH* pShape )
{
    if ( pShape == nullptr )
    { return; }

    // ��{�}�`�̏ꍇ�́C�쐬���̕��Ń�����������s���̂�null�N���A.
    if ( pShape->IsPrimitive() )
    {
        pShape = nullptr;
        return;
    }

    // �u�����`��j������.
    DestroyBranch( (QBVH*)pShape->pShape[0] );
    DestroyBranch( (QBVH*)pShape->pShape[1] );
    DestroyBranch( (QBVH*)pShape->pShape[2] );
    DestroyBranch( (QBVH*)pShape->pShape[3] );

    // placemenet new�����̂Ŗ����I�Ƀf�X�g���N�^���Ăяo��.
    pShape->~QBVH();

    // _aligned_malloc()�Ń��������m�ۂ����̂ŁC_aligned_free()�ŉ��.
    _aligned_free( pShape );

    // null�N���A
    pShape = nullptr;
}

//--------------------------------------------------------------------------
//      �m�[�h�̌���������s���܂�.
//--------------------------------------------------------------------------
bool QBVH::HitFuncNode( const QBVH* pBVH, const Ray& ray, HitRecord& record )
{
    // �܂��q�̃o�E���f�B���O�{�b�N�X�ƌ�������.
    s32 mask = 0;
    if ( !pBVH->box.IsHit( RayQuad( ray ), mask ) )
    { return false; }

    // ���Ƀo�E���f�B���O�{�b�N�X�ƃq�b�g�����q�݂̂��ǂ��Ă���.
    bool isHit0 = false;
    bool isHit1 = false;
    bool isHit2 = false;
    bool isHit3 = false;

    if ( mask & 0x1 )
    { isHit0 = pBVH->pShape[0]->IsHit( ray, record ); }

    if ( mask & 0x2 )
    { isHit1 = pBVH->pShape[1]->IsHit( ray, record ); }

    if ( mask & 0x4 )
    { isHit2 = pBVH->pShape[2]->IsHit( ray, record ); }

    if ( mask & 0x8 )
    { isHit3 = pBVH->pShape[3]->IsHit( ray, record ); }

    return ( isHit0 || isHit1 || isHit2 || isHit3 );
}

//--------------------------------------------------------------------------
//      �t�̌���������s���܂�.
//--------------------------------------------------------------------------
bool QBVH::HitFuncLeaf( const QBVH* pBVH, const Ray& ray, HitRecord& record )
{
    bool isHit0 = pBVH->pShape[0]->IsHit( ray, record );
    bool isHit1 = pBVH->pShape[1]->IsHit( ray, record );
    bool isHit2 = false;
    bool isHit3 = false;

    if ( pBVH->pShape[2] != nullptr )
    { isHit2 = pBVH->pShape[2]->IsHit( ray, record ); }

    if ( pBVH->pShape[3] != nullptr )
    { isHit3 = pBVH->pShape[3]->IsHit( ray, record ); }

    return ( isHit0 || isHit1 || isHit2 || isHit3 );
}

} // namespace s3d
