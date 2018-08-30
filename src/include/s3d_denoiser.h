//-----------------------------------------------------------------------------
// File : s3d_denoiser.h
// Desc : Denoiser Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <s3d_math.h>

namespace s3d {

// Non-Local Means Filter.
void FilterNLM(int width, int height, float coeff, const Color4* src, Color4* dst);

} // namespace s3d
