//-------------------------------------------------------------------------------------------
// File : s3d_ptr.h
// Desc : Smart Pointer Module.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------

#ifndef __S3D_PTR_H__
#define __S3D_PTR_H__

//-------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------
#include <s3d_Typedef.h>


namespace s3d {

/////////////////////////////////////////////////////////////////////////////////////////////
// SinglePtr class
/////////////////////////////////////////////////////////////////////////////////////////////
S3D_TEMPLATE(T)
class SinglePtr
{
    //=======================================================================================
    // list of friend classes and methods.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // public methods
    //=======================================================================================
    SinglePtr() throw()
    : m_pPtr( nullptr )
    , m_Counter( 0 )
    { /* DO_NOTHING */ }

    SinglePtr( std::nullptr_t ) throw()
    : m_pPtr( nullptr )
    , m_Counter( 0 )
    { /* DO_NOTHING */ }

    SinglePtr( T* ptr ) throw()
    : m_pPtr( ptr )
    , m_Counter( 0 )
    { AddRef(); }

    SinglePtr( const SinglePtr& value ) throw()
    : m_pPtr( value.m_pPtr )
    , m_Counter( 0 )
    { AddRef(); }

    S3D_TEMPLATE(U)
    SinglePtr( U* value ) throw()
    : m_pPtr( value )
    , m_Counter( 0 )
    { AddRef(); }

    ~SinglePtr() throw()
    { Release(); }

    SinglePtr& operator = ( T* value ) throw()
    {
        if ( m_pPtr != value )
        { SinglePtr( value ).Swap( *this ); }
        return (*this);
    }

    SinglePtr& operator = ( const SinglePtr& value ) throw()
    {
        if ( m_pPtr != value.m_pPtr )
        { SinglePtr( value ).Swap( *this ); }
        return (*this);
    }

    S3D_TEMPLATE(U)
    SinglePtr& operator = ( U* value ) throw()
    {
        SinglePtr( value ).Swap( *this );
        return (*this);
    }

    S3D_TEMPLATE(U)
    SinglePtr& operator = ( const SinglePtr<U>& value ) throw()
    {
        SinglePtr( value ).Swap( *this );
        return (*this);
    }

    SinglePtr& operator = ( std::nullptr_t ) throw()
    {
        Release();
        return (*this);
    }

    void Reset() throw()
    { Release(); }

    void Swap( SinglePtr& value ) throw()
    {
        T*  tempPtr   = m_pPtr;
        u32 tempCount = m_Counter;

        m_pPtr    = value.m_pPtr;
        m_Counter = value.m_Counter;
        
        value.m_pPtr    = tempPtr;
        value.m_Counter = tempCount;
    }

    void Swap( SinglePtr&& value ) throw()
    {
        T*  tempPtr   = m_pPtr;
        u32 tempCount = m_Counter;
        
        m_pPtr    = value.m_pPtr;
        m_Counter = value.m_Counter;
        
        value.m_pPtr    = tempPtr;
        value.m_Counter = tempCount;
    }

    T* GetPtr() const throw()
    { return m_pPtr; }

    T** GetAddress() const throw()
    { return &m_pPtr; }

    T& operator * () throw()
    { return (*m_pPtr); }

    const T& operator * () const throw()
    { return (*m_pPtr); }

    T* operator -> () const throw()
    { return m_pPtr; }

    operator bool () const throw()
    { return m_pPtr != nullptr; }

protected:
    //=======================================================================================
    // protected variables.
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // protected methods
    //=======================================================================================
    /* NOTHING */

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    T*      m_pPtr;
    s32*    m_Counter;

    //=======================================================================================
    // private methods.
    //=======================================================================================
    void AddRef()
    {
        if ( m_pPtr != nullptr )
        { m_Counter++; }
    }

    void Release()
    {
        if ( m_pPtr )
        {
            m_Counter--;
            if ( m_Counter <= 0 )
            {
                delete m_pPtr;
                m_pPtr = nullptr;
            }
        }
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////
// ArrayPtr class
/////////////////////////////////////////////////////////////////////////////////////////////
S3D_TEMPLATE(T)
class ArrayPtr
{
    //=======================================================================================
    // list of friend classes and method.
    //=======================================================================================
    /* NOTHING */

public:
    //=======================================================================================
    // public variables
    //=======================================================================================

    //=======================================================================================
    // public methods
    //=======================================================================================
    ArrayPtr() throw()
    : m_pPtr( nullptr )
    , m_Counter( 0 )
    { /* DO_NOTHING */ }

    ArrayPtr( std::nullptr_t ) throw()
    : m_pPtr( nullptr )
    , m_Counter( 0 )
    { /* DO_NOTHING */ }

    ArrayPtr( T* ptr ) throw()
    : m_pPtr( ptr )
    , m_Counter( 0 )
    { AddRef(); }

    ArrayPtr( const ArrayPtr& value ) throw()
    : m_pPtr( value.m_pPtr )
    , m_Counter( 0 )
    { AddRef(); }

    S3D_TEMPLATE(U)
    ArrayPtr( U* value ) throw()
    : m_pPtr( value )
    , m_Counter( 0 )
    { AddRef(); }

    ~ArrayPtr() throw()
    { Release(); }

    ArrayPtr& operator = ( T* value ) throw()
    {
        if ( m_pPtr != value )
        { ArrayPtr( value ).Swap( *this ); }
        return (*this);
    }

    ArrayPtr& operator = ( const ArrayPtr& value ) throw()
    {
        if ( m_pPtr != value.m_pPtr )
        { ArrayPtr( value ).Swap( *this ); }
        return (*this);
    }

    S3D_TEMPLATE(U)
    ArrayPtr& operator = ( U* value ) throw()
    {
        ArrayPtr( value ).Swap( *this );
        return (*this);
    }

    S3D_TEMPLATE(U)
    ArrayPtr& operator = ( const ArrayPtr<U>& value ) throw()
    {
        ArrayPtr( value ).Swap( *this );
        return (*this);
    }

    ArrayPtr& operator = ( std::nullptr_t ) throw()
    {
        Release();
        return (*this);
    }

    void Reset() throw()
    { Release(); }

    void Swap( ArrayPtr& value ) throw()
    {
        T*  tempPtr   = m_pPtr;
        u32 tempCount = m_Counter;

        m_pPtr    = value.m_pPtr;
        m_Counter = value.m_Counter;

        value.m_pPtr    = tempPtr;
        value.m_Counter = tempCount;
    }

    void Swap( ArrayPtr&& value ) throw()
    {
        T*  tempPtr   = m_pPtr;
        u32 tempCount = m_Counter;

        m_pPtr    = value.m_pPtr;
        m_Counter = value.m_Counter;

        value.m_pPtr    = tempPtr;
        value.m_Counter = tempCount;
    }

    T* GetPtr() const throw()
    { return m_pPtr; }

    T& operator [] ( s32 index ) throw()
    {
        return m_pPtr[ index ]; /* 配列アクセスは自己責任で! */
    }

    const T& operator [] ( s32 index ) const throw()
    {
        return m_pPtr[ index ]; /* 配列アクセスは自己責任で! */
    }

    operator bool () const throw()
    { return m_pPtr != nullptr; }

protected:
    //=======================================================================================
    // protected variables
    //=======================================================================================
    /* NOTHING */

    //=======================================================================================
    // protected methods
    //=======================================================================================
    /* NOTHING */

private:
    //=======================================================================================
    // private variables.
    //=======================================================================================
    T*      m_pPtr;
    s32     m_Counter;

    //=======================================================================================
    // private methods
    //=======================================================================================
    void AddRef()
    {
        if ( m_pPtr != nullptr )
        { m_Counter++; }
    }

    void Release()
    {
        if ( m_pPtr )
        {
            m_Counter--;
            if ( m_Counter <= 0 )
            {
                delete [] m_pPtr;
                m_pPtr = nullptr;
            }
        }
    }
};

} // namespace s3d

#endif//__S3D_PTR_H__
