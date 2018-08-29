﻿//-------------------------------------------------------------------------------------------------
// File : s3d_ireference.h
// Desc : Reference Counter Interface.
// Copyright(c) Project Asura. All right reserved.
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------------------------------
#include <s3d_typedef.h>


namespace s3d {

///////////////////////////////////////////////////////////////////////////////////////////////////
// IReference interface
///////////////////////////////////////////////////////////////////////////////////////////////////
struct IReference
{
    virtual ~IReference() {}
    virtual void AddRef  () = 0;
    virtual void Release () = 0;
    virtual u32  GetCount() const = 0;
};

} // namespace s3d
