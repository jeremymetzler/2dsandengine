#pragma once

class PerlinNoise {
private:
    double Noise(int i, int x, int y);
    double SmoothedNoise(int i, int x, int y);
    double Interpolate(double a, double b, double x);
    double InterpolatedNoise(int i, double x, double y);
public:

    double ValueNoise(double x, double y);
};