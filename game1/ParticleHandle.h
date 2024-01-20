#pragma once

enum PEvent
{
	Delete,
	Swap
};

struct ParticleEvent
{
	int id, targ, var1, var2;
};

 

class ParticleBase
{
public:
	int pSize1 = 0;
	std::deque<Particle>* pBuffer1 = nullptr;
	int pSize2 = 0;
	std::deque<Particle>* pBuffer2 = nullptr;

	std::deque<Particle>* Particles = nullptr;
	int* pSize = nullptr;

	Particle* pParticleQueue = nullptr;
	int pQueueSize = 0;

	ParticleEvent* EventQueue = nullptr;
	int eventSize = 0;

	std::vector<ParticleObject> Formations = std::vector<ParticleObject>();
	std::vector<Vec2Df> debugDraw = std::vector<Vec2Df>();

	int brushSize = 0;
	int halfBrushSize = 0;
	int currentBrush = 0;
	std::string currentBrushString = "";

	ThreadPool particleBaseThreadPool = ThreadPool();

	ParticleBase()
	{
		particleBaseThreadPool.Start(3);

		pBuffer1 = new std::deque<Particle>;
		pBuffer2 = new std::deque<Particle>;

		pBuffer1->resize(I::baseClass->particleResolution3X[0] * I::baseClass->particleResolution3X[1]);
		pBuffer2->resize(I::baseClass->particleResolution3X[0] * I::baseClass->particleResolution3X[1]);

		pParticleQueue = new Particle[100'000];
		pQueueSize = 0;

		EventQueue = new ParticleEvent[200000];
		eventSize = 0;

		pSize1 = 0;
		pSize2 = 0;

		Particles = pBuffer1;
		pSize = &pSize1;

		brushSize = 0;
		halfBrushSize = 0;
		currentBrush = 1;
		currentBrushString = "sand";
	}

	~ParticleBase()
	{
		particleBaseThreadPool.Stop();

		for (int i = 0; i < Formations.size(); i++)
		{
			SoftDeleteFormation(Formations[i]);
		}

		Formations.clear();

		pBuffer1->clear();
		pBuffer2->clear();

		delete pBuffer1;
		delete pBuffer2;

		delete[] pParticleQueue;
		delete[] EventQueue;
	}

	void DoBrush();
	bool Think();
	void DeleteParticle(int& index);
	void DeleteParticle2(Particle& mainp);
	void DoEvent(ParticleEvent& pEvent);
	void QueueParticle(Vec2Di pos, int type);
	void QueueParticleEvent(int eventId, int targ, int var1 = 0, int var2 = 0);
	bool BoundX(int& xp);
	bool BoundY(int& yp);
	bool BoundClumpX(int xp);
	bool BoundClumpY(int yp);
	Vec2Di TraceRay(int x, int y, int x1, int y1, bool boundOnly = false);
	void SoftDeleteFormation(ParticleObject& form);
	void CleanFormationList(bool loadCheck = false);
	void WakeDormant(int x, int y);
};
