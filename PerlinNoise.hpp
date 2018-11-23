//----------------------------------------------------------------------------------------
//
//  siv::PerlinNoise
//  Perlin noise library for modern C++
//
//  Copyright (C) 2013-2018 Ryo Suzuki <reputeless@gmail.com>
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files(the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions :
//  
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
//----------------------------------------------------------------------------------------

# pragma once
# include <cstdint>
# include <numeric>
# include <algorithm>
# include <random>

float Noise(float x, float y) {
    int n = int(x + y * 57);
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);  
}  

float SmoothedNoise(float x, float y) {
    float corners = (Noise(x-1, y-1) + Noise(x+1, y-1) + Noise(x-1, y+1) + Noise(x+1, y+1)) / 16;
    float sides   = (Noise(x-1, y) + Noise(x+1, y) + Noise(x, y-1) + Noise(x, y+1)) /  8;
    float center  =  Noise(x, y) / 4;
    return corners + sides + center;
}

float Interpolate(float a, float b, float x) {
    float ft = x * 3.1415927;
    float f = (1 - cos(ft)) * 0.5;
    return  a*(1-f) + b*f;
}


float InterpolatedNoise(float x, float y) {

      int integer_X    = int(x);
      float fractional_X = x - integer_X;

      int integer_Y    = int(y);
      float fractional_Y = y - integer_Y;

      float v1 = SmoothedNoise(integer_X,     integer_Y);
      float v2 = SmoothedNoise(integer_X + 1, integer_Y);
      float v3 = SmoothedNoise(integer_X,     integer_Y + 1);
      float v4 = SmoothedNoise(integer_X + 1, integer_Y + 1);

      float i1 = Interpolate(v1 , v2 , fractional_X);
      float i2 = Interpolate(v3 , v4 , fractional_X);

      return Interpolate(i1 , i2 , fractional_Y);
}

float PerlinNoise_2D(float x, float y, float persistence, float Number_Of_Octaves) {

      float total = 0;

      float p = persistence;
      float n = Number_Of_Octaves - 1;

      for (float i = 0.0; i < n; i+=1.0) {

        float frequency = pow(2,i);
        float amplitude= pow(p,i);

        total = total + InterpolatedNoise(x * frequency, y * frequency) * amplitude;

      }

      return total;

}


namespace siv
{
    class PerlinNoise
    {
    private:

        std::int32_t p[512];

        static double Fade(double t) noexcept
        {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        static double Lerp(double t, double a, double b) noexcept
        {
            return a + t * (b - a);
        }

        static double Grad(std::int32_t hash, double x, double y, double z) noexcept
        {
            const std::int32_t h = hash & 15;
            const double u = h < 8 ? x : y;
            const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }

    public:

        explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed)
        {
            reseed(seed);
        }

        template <class URNG>
        explicit PerlinNoise(URNG& urng)
        {
            reseed(urng);
        }

        void reseed(std::uint32_t seed)
        {
            for (size_t i = 0; i < 256; ++i)
            {
                p[i] = i;
            }

            std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

            for (size_t i = 0; i < 256; ++i)
            {
                p[256 + i] = p[i];
            }
        }

        template <class URNG>
        void reseed(URNG& urng)
        {
            for (size_t i = 0; i < 256; ++i)
            {
                p[i] = i;
            }

            std::shuffle(std::begin(p), std::begin(p) + 256, urng);

            for (size_t i = 0; i < 256; ++i)
            {
                p[256 + i] = p[i];
            }
        }

        double noise(double x) const
        {
            return noise(x, 0.0, 0.0);
        }

        double noise(double x, double y) const
        {
            return noise(x, y, 0.0);
        }

        double noise(double x, double y, double z) const
        {
            const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
            const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
            const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

            x -= std::floor(x);
            y -= std::floor(y);
            z -= std::floor(z);

            const double u = Fade(x);
            const double v = Fade(y);
            const double w = Fade(z);

            const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
            const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

            return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
                Grad(p[BA], x - 1, y, z)),
                Lerp(u, Grad(p[AB], x, y - 1, z),
                Grad(p[BB], x - 1, y - 1, z))),
                Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
                Grad(p[BA + 1], x - 1, y, z - 1)),
                Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
                Grad(p[BB + 1], x - 1, y - 1, z - 1))));
        }

        double octaveNoise(double x, std::int32_t octaves) const
        {
            double result = 0.0;
            double amp = 1.0;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += noise(x) * amp;
                x *= 2.0;
                amp *= 0.5;
            }

            return result;
        }

        double octaveNoise(double x, double y, std::int32_t octaves) const
        {
            double result = 0.0;
            double amp = 1.0;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += noise(x, y) * amp;
                x *= 2.0;
                y *= 2.0;
                amp *= 0.5;
            }

            return result;
        }

        double octaveNoise(double x, double y, double z, std::int32_t octaves) const
        {
            double result = 0.0;
            double amp = 1.0;

            for (std::int32_t i = 0; i < octaves; ++i)
            {
                result += noise(x, y, z) * amp;
                x *= 2.0;
                y *= 2.0;
                z *= 2.0;
                amp *= 0.5;
            }

            return result;
        }

        double noise0_1(double x) const
        {
            return noise(x) * 0.5 + 0.5;
        }

        double noise0_1(double x, double y) const
        {
            return noise(x, y) * 0.5 + 0.5;
        }

        double noise0_1(double x, double y, double z) const
        {
            return noise(x, y, z) * 0.5 + 0.5;
        }

        double octaveNoise0_1(double x, std::int32_t octaves) const
        {
            return octaveNoise(x, octaves) * 0.5 + 0.5;
        }

        double octaveNoise0_1(double x, double y, std::int32_t octaves) const
        {
            return octaveNoise(x, y, octaves) * 0.5 + 0.5;
        }

        double octaveNoise0_1(double x, double y, double z, std::int32_t octaves) const
        {
            return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
        }
    };
}