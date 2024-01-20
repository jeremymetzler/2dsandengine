#include "header_stack.h"
Vec2Df::Vec2Df(Vec2Di Vec)
{
    x = static_cast<float>(Vec.x);
    y = static_cast<float>(Vec.y);
}

Vec2Df::Vec2Df(Vec2Dd Vec)
{
    x = static_cast<float>(Vec.x);
    y = static_cast<float>(Vec.y);
}

Vec2Di::Vec2Di(Vec2Dd Vec)
{
    x = static_cast<int>(Vec.x);
    y = static_cast<int>(Vec.y);
}

float Math::RadToDeg(float Radian)
{
    return Radian * (180.f / PI);
}

float Math::DegToRad(float Degree)
{
    return Degree * (PI / 180.f);
}

float Math::NormalizeAngle(float Angle)
{
    if (isnan(Angle))
        return 0;

    if (Angle > 180.f || Angle < -180.f)
    {
        auto revolutions = round(abs(Angle / 360.f));

        if (Angle < 0.f)
        {
            Angle += 360.f * revolutions;
        }
        else
        {
            Angle -= 360.f * revolutions;
        }
    }

    return Angle;
}

float Math::CalcAngle(Vec2Df Source, Vec2Df Destination)
{
    float angle = 0.f;
    float delta[] = { (Source.x - Destination.x), (Source.y - Destination.y) };

    angle = static_cast<float>(atanf(static_cast<float>(delta[1] / delta[0])) * 57.295779513082f);

    if (delta[0] >= 0.0)
        angle += 180.0f;

    return NormalizeAngle(angle);
}

float Math::CalcDistance(Vec2Df Point1, Vec2Df Point2)
{
    float diffY = Point1.y - Point2.y;
    float diffX = Point1.x - Point2.x;
    return sqrt(pow(diffY, 2) + pow(diffX, 2));
}

Vec2Df Math::ProjectPoint(Vec2Df Source, float Angle, float Distance)
{
    if (Distance == 0)
        return Source;
    else
        return Source + Vec2Df(Distance * cos(DegToRad(Angle)), Distance * sin(DegToRad(Angle)));
}

Vec2Df Math::SimpleProjectPoint(Vec2Df Source, Vec2Df Dest, float Dist)
{
    float ang = CalcAngle(Source, Dest);
    return ProjectPoint(Source, ang, Dist);
}

int Math::RoundToInt(float value)
{
    if (value < 0.f)
        return static_cast<int>(value - 0.5f);
    else
        return static_cast<int>(value + 0.5f);
}

ColorM Math::BlendColor(unsigned char r0, unsigned char g0, unsigned char b0, unsigned char a0, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1)
{

    float alpha = 254 - (254 - a0) * (254 - a1);
    float red = r0 * a0 / alpha + r1 * a1 * (254 - a0) / alpha;
    float green = g0 * a0 / alpha + g1 * a1 * (254 - a0) / alpha;
    float blue = b0 * a0 / alpha + b1 * a1 * (254 - a0) / alpha;

    return { red , green, blue, alpha };
}

ColorM Math::BlendColor2(ColorM& a, ColorM& b, float bias)
{

    ColorM retClr;

    retClr.r = bias * a.r + (1.0 - bias) * b.r;
    retClr.g = bias * a.g + (1.0 - bias) * b.g;
    retClr.b = bias * a.b + (1.0 - bias) * b.b;

    return retClr;
}