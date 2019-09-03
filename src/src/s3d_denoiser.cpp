//-----------------------------------------------------------------------------
// File : s3d_denoiser.cpp
// Desc : Denoiser Module.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <s3d_denoiser.h>
#include <array>


namespace s3d {

//-----------------------------------------------------------------------------
//      Non-Local Meansフィルタ.
//-----------------------------------------------------------------------------
void FilterNLM(int width, int height, float coeff, const Color4* src, Color4* dst)
{
    // Original Code by ushio.
    // https://github.com/Ushio/lost-child-render/blob/master/core/image_processing.hpp
    auto param_h = s3d::Max(0.0001f, coeff);
    auto sigma   = s3d::Max(0.0001f, coeff);
    auto frac_param_h_squared = 1.0f / (param_h * param_h);
    auto sigma_squared = sigma * sigma;

    const int kKernel       = 5;
    const int kSupport      = 13;
    const int kHalfKernel   = kKernel / 2;
    const int kHalfSupport  = kSupport / 2;

    using Template = std::array<Color4, kKernel * kKernel>;

    auto sample_template = [width, height, src, kHalfKernel](int x, int y) 
    {
        Template t;
        int index = 0;
        for (int sx = x - kHalfKernel; sx <= x + kHalfKernel; ++sx)
        {
            for (int sy = y - kHalfKernel; sy <= y + kHalfKernel; ++sy)
            {
                auto sample_x = Clamp(sx, 0, width  - 1);
                auto sample_y = Clamp(sy, 0, height - 1);
                auto src_idx = sample_y * width + sample_x;
                t[index++] = src[src_idx];
            }
        }
        return t;
    };

    auto distance_sqared_template = [](const Template &lhs, const Template &rhs)
    {
        auto accum = 0.0f;
        for (int i = 0; i < lhs.size(); ++i)
        { accum += (lhs[i] - rhs[i]).LengthSq(); }
        return accum;
    };

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto sample = [=](int x, int y)
            {
                auto sample_x = Clamp(x, 0, width  - 1);
                auto sample_y = Clamp(y, 0, height - 1);
                auto src_idx  = sample_y * width + sample_x;
                return src[src_idx];
            };

            auto focus = sample_template(x, y);

            Color4 sum(0.0f, 0.0f, 0.0f, 0.0f);
            auto sum_weight = 0.0f;
            for (int sx = x - kHalfSupport; sx <= x + kHalfSupport; ++sx)
            {
                for (int sy = y - kHalfSupport; sy <= y + kHalfSupport; ++sy) 
                {
                    auto target = sample_template(sx, sy);
                    auto dist   = distance_sqared_template(focus, target);
                    auto arg    = -Max(dist - 2.0f * sigma_squared, 0.0f) * frac_param_h_squared;
                    auto weight = exp(arg);

                    sum_weight += weight;
                    sum += sample(sx, sy) * weight;
                }
            }

            auto color = sum / sum_weight;
            auto dst_idx = y * width + x;
            dst[dst_idx] = color;
        }
    }
}

} // namespace s3d
