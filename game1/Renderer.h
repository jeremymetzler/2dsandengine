#pragma once 


class CharData
{
public:
	int x, y;
	ColorM clr;
};

class CompiledCharData
{ 
public:
	int width, height;
	int leftOffset, rightOffset, topOffset, bottomOffset;

	std::vector<Vec2Df> charData;

	void MoveChar(float xsteps, float ysteps);
	void ScaleChar(float scale);
};

class StringFormation
{
public:
	int index = 0;
	bool manualRender = false;
	std::vector<CompiledCharData> stringData;
	CompiledCharData* chosenFont = nullptr;
	float x, y;
	std::string textData;
	int pixelSize = 0;
	float textScale = 0.f;
	float width = 0.f;
	float height = 0.f;
	int mode = 0;
	ColorM clr;
	StringFormation(int xPos, int yPos, std::string sText, CompiledCharData* cFont, ColorM clrc, float fTextScale = 1.f, int iPixelSize = 1, int iMode = 0);

	void Render();
	void SetString(std::string sText);
	void SetPos(float x, float y);
	StringFormation* setManualRender(bool val);
};

class C_BaseUIComponent;

namespace CustRender
{
	struct ClipInfo_t
	{
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		bool b = false;
	};

	extern std::vector<ClipInfo_t>clipList;

	void DebugPrint(int x, int y, std::string text, ColorM clr, float textScale = 1.f, int pixelSize = 1);
	void CompileFont(const unsigned char** cFont, int byteCount, CompiledCharData* compiledFont, int iWidth, int iHeight);
	void CompileFonts();
	CompiledCharData* getFirstFont();
	extern std::vector<StringFormation*> StringStack;
	StringFormation* pushString(int xPos, int yPos, std::string sText, CompiledCharData* cFont, ColorM clrc, float fTextScale = 1.f, int iPixelSize = 1);
	void deleteString(StringFormation*& sString);
	void deleteStringByIndex(int index = 0);

	void drawRect(double x1, double y1, double x2, double y2, ColorM clr);
	void drawProjectedLine(float x1, float y1, float ang, float len, ColorM clr);
	void drawPoint(float x, float y, int size, ColorM clr);

	void FilledRect(float x, float y, float w, float h, ColorM clr);
	void Rect(float x, float y, float w, float h, ColorM clr);
	void Line(float x, float y, float x1, float y1, ColorM clr);
	void GradRect(int x, int y, int w, int h, int gradw, float scale, ColorM clr);


	extern void DefaultTheme(C_BaseUIComponent* ecx);
	using DefaultThemeFN = decltype(&DefaultTheme);

	extern void RedTheme(C_BaseUIComponent* ecx);

//	extern void PushClip(int x, int y, int w, int h);// create clip bounds
//	extern void PopClip();// delete clip bounds
}

class CDraw
{
public:
	virtual const std::string TypeName() { return "CDraw"; }
	virtual void Render() {};

	int x = 0, y = 0, w = 0, h = 0;
	ColorM clr = ColorM();
	std::string text = "";

	CDraw() {};

	virtual void Update(int ix, int iy, int iw, int ih, ColorM cclr = ColorM());
	virtual void Update(int ix, int iy, int iw, int ih);
	virtual void Update(int ix, int iy);
	virtual void Update(ColorM cclr);
	virtual void Update(std::string ctext);

private:
};

class CDRect :public CDraw
{
public:
	virtual const std::string TypeName() { return "CDRect"; }
	CDRect(int ix, int iy, int iw, int ih, ColorM cclr, std::string ctext = "")
	{
		x = ix;
		y = iy;
		w = iw;
		h = ih;
		clr = cclr;
		text = ctext;
	}

	void Render();
private:
};

class CDFilledRect : public CDraw
{
public:
	virtual const std::string TypeName() { return "CDFilledRect"; }
	CDFilledRect(int ix, int iy, int iw, int ih, ColorM cclr, std::string ctext = "")
	{
		x = ix;
		y = iy;
		w = iw;
		h = ih;
		clr = cclr;
		text = ctext;
	}

	void Render();
private:
};

class CDGradRect : public CDraw
{
public:
	virtual const std::string TypeName() { return "CDGradRect"; }
	CDGradRect(int ix, int iy, int iw, int ih, int igradw, ColorM cclr, std::string ctext = "")
	{
		x = ix;
		y = iy;
		w = iw;
		h = ih;
		gradw = igradw;
		clr = cclr;
		text = ctext;
		scale = 300.f / fabs(static_cast<float>(igradw)); // ik its 255
	}

	void Render();
	int gradw = 0;
	float scale = 0;
private:
};

class CDLine :public CDraw
{
public:
	virtual const std::string TypeName() { return "CDLine"; }
	CDLine(int ix, int iy, int ix2, int iy2, ColorM cclr, std::string ctext = "")
	{
		x = ix;
		y = iy;
		w = ix2;
		h = iy2;
		clr = cclr;
		text = ctext;
	}

	void Update(int ix, int iy);
	void Render();
private:
};

class CDText :public CDraw
{
public:
	virtual const std::string TypeName() { return "CDText"; }


	CDText(int ix, int iy, std::string ctext, CompiledCharData* lfont, ColorM cclr, float ftextscale = 1.f, int ipixelsize = 1,int imode = 0)
	{
		x = ix;
		y = iy;

		text = ctext;
		font = lfont;
		clr = cclr;
		mode = imode;

		textscale = ftextscale;
		pixelsize = ipixelsize;

		strForm = new StringFormation(ix, iy, ctext, lfont, cclr, ftextscale, ipixelsize, imode);
		strForm->manualRender = true;
	}

	void Render();

	float GetWidth()
	{
		return strForm->width;
	}
	float GetHeight()
	{
		return strForm->height;
	}
	void ChangeText(std::string txt)
	{
		strForm->SetString(txt);
	}
private:
	StringFormation* strForm = nullptr;
	CompiledCharData* font = nullptr;
	int mode = 0;
	float textscale = 1.f;
	int pixelsize = 1;
};