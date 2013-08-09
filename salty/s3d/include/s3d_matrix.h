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
#include <s3d_typedef.h>
#include <s3d_vector3.h>


namespace s3d {

////////////////////////////////////////////////////////////////////////////////////
// Matric structure
////////////////////////////////////////////////////////////////////////////////////
struct Matrix
{
public:
    f64 _11, _12, _13, _14;
    f64 _21, _22, _23, _24;
    f64 _31, _32, _33, _34;
    f64 _41, _42, _43, _44;

    Matrix();
    Matrix( 
        const f64, const f64, const f64, const f64,
        const f64, const f64, const f64, const f64,
        const f64, const f64, const f64, const f64,
        const f64, const f64, const f64, const f64 );
    Matrix( const Matrix& );

    Matrix& operator =  ( const Matrix& );
    Matrix& operator += ( const Matrix& );
    Matrix& operator -= ( const Matrix& );
    Matrix& operator *= ( const Matrix& );
    Matrix& operator *= ( const f64 );

    Matrix operator + () const;
    Matrix operator - () const;
    Matrix operator + ( const Matrix& ) const;
    Matrix operator - ( const Matrix& ) const;
    Matrix operator * ( const Matrix& ) const;
    Matrix operator * ( const f64 )     const;

    f64 Det() const;

    static Matrix Invert    ( const Matrix& );
    static Matrix Transpose ( const Matrix& );

    static Matrix Identity  ();
    static Matrix Translate ( const f64, const f64, const f64 );
    static Matrix Scale     ( const f64, const f64, const f64 );
    static Matrix RotateX   ( const f64 );
    static Matrix RotateY   ( const f64 );
    static Matrix RotateZ   ( const f64 );
    static Matrix Rotate    ( const f64, const f64, const f64, const f64 );
    static Matrix LookAt    ( const Vector3&, const Vector3&, const Vector3& );
    static Matrix PersFov   ( const f64, const f64, const f64, const f64 );
};

} // namespace s3d


//--------------------------------------------------------------------------------
// Inlines
//---------------------------------------------------------------------------------
#include <s3d_matrix.inl>


#endif//__S3D_MATRIX_H__