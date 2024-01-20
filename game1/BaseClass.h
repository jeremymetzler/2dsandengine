#pragma once
struct Cfg
{
	bool bMenuOpen = false;
	bool bRandomGen = false;
	int iTest = 0;
};

class ThreadPool {
public:
	void Start(int threads);
	void QueueJob(const std::function<void()>& job);
	void FinishJobs();
	void Stop();
	bool busy();
	void ThreadLoop();
	int numThreads = 0;
private:

	bool should_terminate = false;           // Tells threads to stop looking for jobs
	// Prevents data races to the job queue
	std::condition_variable_any mutex_condition = std::condition_variable_any(); // Allows threads to wait on new jobs or termination 
	std::vector<std::thread> threads = std::vector<std::thread>();
	std::queue<std::function<void()>> jobs = std::queue<std::function<void()>>();
};

// this is where everything is held
class BaseClass
{
public:


	std::shared_mutex queue_mutex;

    GLFWwindow* window = nullptr;

	short maxThreads = 0;
	short curThreads = 0;

	float tickCount = 0.f;
	float fixedDeltaTime = 0.f;
	int msPerThink = 0;

	int lastMilli = 0;//getMilliCount()
	int curMilli = 0;//getMilliCount()
	int milliDelta = 0;

	float gravityFeetPerSec = 0.f; // x3.281 is the aprox conversion of meters to feet (changing this will change the scale of the physics movement)
	float gravityPerTick = 0.f;

	int screenResolution[2] = { 0,0 };

	float particleResolutionHalf[2] = {0,0};
	int particleResolution[2] = {0,0};//size of 1 chunk    
	int particleResolution3X[2] = {0,0};//size of 1 clump

	int particleSize = 0;

	C_BaseMenu* menuBase = nullptr;
	Cfg* cfgInfo = nullptr;

	float screenLeft = cameraPos.x - 160.f;
	float screenBottom = cameraPos.y - 90.f;
	float screenRight = cameraPos.x + 160.f;
	float screenTop = cameraPos.y + 90.f;

	float halfXParticleRes = 320.f / 2.f;
	float halfYParticleRes = 180.f / 2.f;

	Vec2Dd cameraPos = { halfXParticleRes, halfYParticleRes };
	Vec2Dd oldCameraPos = cameraPos;

	BaseClass();

	~BaseClass()
	{
		delete cfgInfo;
		delete menuBase;
	}

	void CalcTime();
	void TimeKetchup();

	bool ShouldClose();

    void NewFrame();
    void EndFrame();

    void Run();

	void RenderUI();
};

