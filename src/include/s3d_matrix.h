//---------------------------------------------------------------------------------
// File : s3d_matrix.h
// Desc : Matrix Module.
// Copyright(c) Project Asura. All right reserved.
//---------------------------------------------------------------------------------

#ifndef __S3D_MATRIX_H__
#define __S3D_MATRIX_H__


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <s3d_math.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// Matric structure
////////////////////////////////////////////////////////////////////////////////////
struct Matrix
{
public:
    f32 _11, _12, _13, _14;
    f32 _21, _22, _23, _24;
    f32 _31, _32, _33, _34;
    f32 _41, _42, _43, _44;

    Matrix();
    Matrix( 
        const f32, const f32, const f32, const f32,
        const f32, const f32, const f32, const f32,
        const f32, const f32, const f32, const f32,
        const f32, const f32, const f32, const f32 );
    Matrix( const Matrix& );

    Matrix& operator =  ( const Matrix& );
    Matrix& operator += ( const Matrix& );
    Matrix& operator -= ( const Matrix& );
    Matrix& operator *= ( const Matrix& );
    Matrix& operator *= ( const f32 );

    Matrix operator + () const;
    Matrix operator - () const;
    Matrix operator + ( const Matrix& ) const;
    Matrix operator - ( const Matrix& ) const;
    Matrix operator * ( const Matrix& ) const;
    Matrix operator * ( const f32 )     const;

    f32 Det() const;

    static Matrix Invert    ( const Matrix& );
    static Matrix Transpose ( const Matrix& );

    static Matrix Identity  ();
    static Matrix Translate ( const f32, const f32, const f32 );
    static Matrix Scale     ( const f32, const f32, const f32 );
    static Matrix RotateX   ( const f32 );
    static Matrix RotateY   ( const f32 );
    static Matrix RotateZ   ( const f32 );
    static Matrix Rotate    ( const f32, const f32, const f32, const f32 );
    static Matrix LookAt    ( const Vector3&, const Vector3&, const Vector3& );
    static Matrix PersFov   ( const f32, const f32, const f32, const f32 );
};

} // namespace s3d


//--------------------------------------------------------------------------------
// Inlines
//---------------------------------------------------------------------------------
#include <s3d_matrix.inl>


#endif//__S3D_MATRIX_H__