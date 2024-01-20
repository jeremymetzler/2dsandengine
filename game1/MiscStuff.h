#pragma once

const float PI = 3.1415926f;

class ColorM
{
public:
    float r = 1.f, g = 1.f, b = 1.f, a = 1.f;

	ColorM(){}

    ColorM(float red, float green, float blue, float alpha = 1.f)
    {
        r = red;// / 255.f;
        g = green;// / 255.f;
        b = blue;// / 255.f;
		a = alpha;
    }

    void SetColor(float red, float green, float blue, float alpha = 1.f)
    {
        r = red;// / 255.f;
        g = green;// / 255.f;
        b = blue;// / 255.f;
		a = alpha;
    }

	constexpr ColorM& FromHSV(float h, float s, float v)
	{
		float colOut[3]{ };
		if (s == 0.0f)
		{
			r = g = b = v;
			return *this;
		}

		h = std::fmodf(h, 1.0f) / (60.0f / 360.0f);
		int   i = static_cast<int>(h);
		float f = h - static_cast<float>(i);
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0:
			colOut[0] = v;
			colOut[1] = t;
			colOut[2] = p;
			break;
		case 1:
			colOut[0] = q;
			colOut[1] = v;
			colOut[2] = p;
			break;
		case 2:
			colOut[0] = p;
			colOut[1] = v;
			colOut[2] = t;
			break;
		case 3:
			colOut[0] = p;
			colOut[1] = q;
			colOut[2] = v;
			break;
		case 4:
			colOut[0] = t;
			colOut[1] = p;
			colOut[2] = v;
			break;
		case 5: default:
			colOut[0] = v;
			colOut[1] = p;
			colOut[2] = q;
			break;
		}

		r = colOut[0] * 255;
		g = colOut[1] * 255;
		b = colOut[2] * 255;
		return *this;
	}

	ColorM operator -(float minus)
	{
		return { r - minus, g - minus, b - minus };
	}

	ColorM& setAlpha(float val)
	{
		a = val;
		return *this;
	}
};

class Vec2Di;
class Vec2Dd;

class Vec2Df
{
public:
    float x = 0.f, y = 0.f;
    Vec2Df()
    {      
    }
    Vec2Df(float xp, float yp)
    {
        x = xp;
        y = yp;
    }
    Vec2Df(int xp, int yp)
    {
        x = static_cast<float>(xp);
        y = static_cast<float>(yp);
    }
    Vec2Df(long xp, long yp)
    {
        x = static_cast<float>(xp);
        y = static_cast<float>(yp);
    }
	Vec2Df(double xp, double yp)
	{
		x = static_cast<float>(xp);
		y = static_cast<float>(yp);
	}

	Vec2Df(Vec2Di Vec);
	Vec2Df(Vec2Dd Vec);

    inline Vec2Df operator +(Vec2Df Add)
    {
		return { x + Add.x, y + Add.y };
    }
	inline void operator +=(Vec2Df Add)
	{
		x += Add.x;
		y += Add.y;
	}
	inline void operator -=(Vec2Df Minus)
	{
		x -= Minus.x;
		y -= Minus.y;
	}
	inline void operator *=(Vec2Df Multi)
	{
		x *= Multi.x;
		y *= Multi.y;
	}
	inline void operator /=(Vec2Df Divide)
	{
		x /= Divide.x;
		y /= Divide.y;
	}
	inline void operator /=(float Divide)
	{
		x /= Divide;
		y /= Divide;
	}
    inline Vec2Df operator -(Vec2Df Minus)
    {
		return { x - Minus.x, y - Minus.y };
    }
	inline bool operator ==(Vec2Df Vec)
	{
		return x == Vec.x && y == Vec.y;
	}
	inline bool operator !=(Vec2Df Vec)
	{
		return x != Vec.x || y != Vec.y;
	}
    inline Vec2Df operator /(float divide)
    {
		return { x / divide, y / divide };
    }
	inline Vec2Df operator *(float multi)
	{
		return { x * multi, y * multi };
	}
	inline void operator =(b2Vec2 Vec)
	{
		x = Vec.x;
		y = Vec.y;

	}
	inline float Length() const
	{
		return b2Sqrt(x * x + y * y);
	}
	inline void Clear()
	{
		x = 0;
		y = 0;
	}
	inline void Set(float iX, float iY)
	{
		x = iX;
		y = iY;
	}
	float Distance(Vec2Df to)
	{
		Vec2Df delta = to - *(Vec2Df*)this;
		return std::sqrt(delta.x * delta.x + delta.y * delta.y);
	}

};

class Vec2Di
{
public:
	int x = 0, y = 0;
	Vec2Di()
	{
	}
	Vec2Di(int xp, int yp)
	{
		x = xp;
		y = yp;
	}
	Vec2Di(float xp, float yp)
	{
		x = static_cast<int>(xp);
		y = static_cast<float>(yp);
	}
	Vec2Di(long xp, long yp)
	{
		x = static_cast<int>(xp);
		y = static_cast<int>(yp);
	}
	Vec2Di(double xp, double yp)
	{
		x = static_cast<int>(xp);
		y = static_cast<int>(yp);
	}

	Vec2Di(Vec2Df Vec)
	{
		x = static_cast<int>(Vec.x);
		y = static_cast<int>(Vec.y);
	}

	Vec2Di(Vec2Dd Vec);	

	inline Vec2Di operator +(Vec2Di Add)
	{
		return { x + Add.x, y + Add.y };
	}
	inline void operator +=(Vec2Di Add)
	{
		x += Add.x;
		y += Add.y;
	}
	inline void operator -=(Vec2Di Minus)
	{
		x -= Minus.x;
		y -= Minus.y;
	}
	inline void operator *=(Vec2Di Multi)
	{
		x *= Multi.x;
		y *= Multi.y;
	}
	inline void operator /=(Vec2Di Divide)
	{
		x /= Divide.x;
		y /= Divide.y;
	}
	inline void operator /=(int Divide)
	{
		x /= Divide;
		y /= Divide;
	}
	inline Vec2Di operator -(Vec2Di Minus)
	{
		return { x - Minus.x, y - Minus.y };
	}
	inline bool operator ==(Vec2Di Vec)
	{
		return x == Vec.x && y == Vec.y;
	}
	inline bool operator !=(Vec2Di Vec)
	{
		return x != Vec.x || y != Vec.y;
	}
	inline Vec2Di operator /(int divide)
	{
		return { x / divide, y / divide };
	}
	inline Vec2Di operator *(int multi)
	{
		return { x * multi, y * multi };
	}
	inline bool operator ==(b2Vec2 vec)
	{
		return (x == static_cast<int>(vec.x) && y == static_cast<int>(vec.y));
	}
	inline void operator =(b2Vec2 vec)
	{
		x = static_cast<int>(vec.x);
		y = static_cast<int>(vec.y);
	}

	inline void Clear()
	{
		x = 0;
		y = 0;
	}
	inline void Set(int iX, int iY)
	{
		x = iX;
		y = iY;
	}

	inline float Length() const
	{
		return b2Sqrt(x * x + y * y);
	}
};

class Vec2Dd
{
public:
	double x = 0, y = 0;
	Vec2Dd()
	{
	}
	Vec2Dd(float xp, float yp)
	{
		x = static_cast<double>(xp);
		y = static_cast<double>(yp);
	}
	Vec2Dd(int xp, int yp)
	{
		x = static_cast<double>(xp);
		y = static_cast<double>(yp);
	}
	Vec2Dd(long xp, long yp)
	{
		x = static_cast<double>(xp);
		y = static_cast<double>(yp);
	}
	Vec2Dd(double xp, double yp)
	{
		x = xp;
		y = yp;
	}

	Vec2Dd(Vec2Df Vec)
	{
		x = static_cast<double>(Vec.x);
		y = static_cast<double>(Vec.y);
	}

	inline Vec2Dd operator +(Vec2Dd Add)
	{
		return { x + Add.x, y + Add.y };
	}
	inline void operator +=(Vec2Dd Add)
	{
		x += Add.x;
		y += Add.y;
	}
	inline void operator -=(Vec2Dd Minus)
	{
		x -= Minus.x;
		y -= Minus.y;
	}
	inline void operator *=(Vec2Dd Multi)
	{
		x *= Multi.x;
		y *= Multi.y;
	}
	inline void operator /=(Vec2Dd Divide)
	{
		x /= Divide.x;
		y /= Divide.y;
	}
	inline void operator /=(double Divide)
	{
		x /= Divide;
		y /= Divide;
	}
	inline Vec2Dd operator -(Vec2Dd Minus)
	{
		return { x - Minus.x, y - Minus.y };
	}
	inline bool operator ==(Vec2Dd Vec)
	{
		return x == Vec.x && y == Vec.y;
	}
	inline bool operator !=(Vec2Dd Vec)
	{
		return x != Vec.x || y != Vec.y;
	}
	inline Vec2Dd operator /(double divide)
	{
		return { x / divide, y / divide };
	}
	inline Vec2Dd operator *(double multi)
	{
		return { x * multi, y * multi };
	}
	inline void operator =(b2Vec2 Vec)
	{
		x = Vec.x;
		y = Vec.y;

	}
	inline float Length() const
	{
		return b2Sqrt(x * x + y * y);
	}
	inline void Clear()
	{
		x = 0;
		y = 0;
	}
	inline void Set(double iX, double iY)
	{
		x = iX;
		y = iY;
	}
};


namespace Math
{
	float RadToDeg(float Radian);
	float DegToRad(float Degree);
	float NormalizeAngle(float Angle);
	float CalcAngle(Vec2Df Source, Vec2Df Destination);
	float CalcDistance(Vec2Df point1, Vec2Df point2);
    Vec2Df ProjectPoint(Vec2Df Source, float Angle, float Distance);
    Vec2Df SimpleProjectPoint(Vec2Df Source, Vec2Df Dest, float Dist);
	int RoundToInt(float value);
	ColorM BlendColor(unsigned char r0, unsigned char g0, unsigned char b0, unsigned char a0, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1);
	ColorM BlendColor2(ColorM& a, ColorM& b, float bias = 0.5f);
}
