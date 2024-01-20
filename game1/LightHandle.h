#pragma once

class PixelInfo
{
public:
	int x, y;
	unsigned char r, g, b;
};

class SpotLight
{
public:
	int x, y;
	int radius;
	ColorM clr = { 254.f,254.f,254.f};
	float brightness = 0.7f;
};

 

class LightBase
{
public:
	int** lightBuffer = nullptr;

	float sunAngle = 0.f;
	PixelInfo* pixelBuffer1 = nullptr;
	int bufferSize1 = 0;
	PixelInfo* pixelBuffer2 = nullptr;
	int bufferSize2 = 0;
	PixelInfo* drawStack = nullptr;
	int* drawSize = nullptr;

	bool bufferSwitch = false;
	PixelInfo* oppositeDrawStack = nullptr;
	int* oppositeDrawSize = nullptr;

	std::vector<SpotLight> spotLights = std::vector<SpotLight>();

	int renderQueue[5] = {0,0,0,0,0};

	unsigned int maxBufferSize = 0;

	int XBoundsMax = 0;
	int XBoundsMin = 0;
	int YBoundsMax = 0;
	int YBoundsMin = 0;

	float xCos = 0;
	float ySin = 0;

	int offStep = 0;
	int whiteStep = 0;

	float shadowFadeLen = 100.f;
	float scale = 254.f / shadowFadeLen;

	LightBase()
	{
		spotLights.clear();

		sunAngle = -35.f;

		lightBuffer = new int* [I::baseClass->particleResolution[0]];

		for (int i = 0; i < I::baseClass->particleResolution[0]; i++)
		{
			lightBuffer[i] = new int[I::baseClass->particleResolution[1]];
			for (int d = 0; d < I::baseClass->particleResolution[1]; d++)
			{
				lightBuffer[i][d] = 0;
			}
		}

		maxBufferSize = (I::baseClass->particleResolution[0] * I::baseClass->particleResolution[1]) + 1000;

		pixelBuffer1 = new PixelInfo[maxBufferSize];
		bufferSize1 = 0;
		pixelBuffer2 = new PixelInfo[maxBufferSize];
		bufferSize2 = 0;

		drawSize = &bufferSize1;
		drawStack = pixelBuffer1;

		renderQueue[0] = 0;
		renderQueue[1] = 0;
		renderQueue[2] = 0;
		renderQueue[3] = 0;
		renderQueue[4] = -1;

	}

	~LightBase()
	{
		spotLights.clear();

		delete[] pixelBuffer1;
		delete[] pixelBuffer2;		

		for (int i = 0; i < I::baseClass->particleResolution[0]; i++)
		{
			delete[] lightBuffer[i];
		}

		delete[] lightBuffer;
	}

	Vec2Di PosToPixel(Vec2Df pos);
	void DoLighting(int xP, int yP);
	void DoSpotLights();
	//void DoSpotLights();
	void Render(bool Update = false);
};