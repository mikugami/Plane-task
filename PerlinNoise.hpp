# pragma once

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

float perlinNoise(float x, float y, float persistence, float Number_Of_Octaves) {

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
