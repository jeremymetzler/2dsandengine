#pragma once


class RayCastClosestCallback : public b2RayCastCallback
{
public:
	RayCastClosestCallback(int onlycollide = -1)
	{
		m_hit = false;
		m_fixture = nullptr;
		m_collision_index = onlycollide;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override
	{

		if (m_collision_index != -1)
		{
			if (fixture->GetUserData().pointer != m_collision_index)
				return -1.0f;
		}

		m_hit = true;
		m_point = point;
		m_normal = normal;
		m_fixture = fixture;

		// By returning the current fraction, we instruct the calling code to clip the ray and
		// continue the ray-cast to the next fixture. WARNING: do not assume that fixtures
		// are reported in order. However, by clipping, we can always get the closest fixture.
		return fraction;
	}

	bool m_hit;
	b2Vec2 m_point;
	b2Vec2 m_normal;
	b2Fixture* m_fixture;
	int m_collision_index;
};

 

class PhysWorldBase
{
public:
	b2World* physWorld = nullptr;
	b2Body* ground = nullptr;

	b2Body** terrainBodies = nullptr;
	int terrainBodySize = 0;

	int** particleGroups = nullptr;
	int curGroupSize = 0;

	bool curBuffer = false;
	int posSize1 = 0;
	Vec2Di* checkPosBuffer1 = nullptr;
	int posSize2 = 0;
	Vec2Di* checkPosBuffer2 = nullptr;

	b2Vec2* vertices = nullptr;

	const int maxCheckBufferSize = 8000;

	PhysWorldBase()
	{
		curBuffer = false;
		terrainBodies = new b2Body* [100];

		particleGroups = new int* [I::baseClass->particleResolution3X[0]];

		for (int i = 0; i < I::baseClass->particleResolution3X[0]; ++i)
		{
			particleGroups[i] = new int[I::baseClass->particleResolution3X[1]];

			for (int d = 0; d < I::baseClass->particleResolution3X[1]; d++)
			{
				particleGroups[i][d] = 0;
			}
		}

		curGroupSize = 0;

		posSize1 = 0;
		posSize2 = 0;

		vertices = new b2Vec2[maxCheckBufferSize];

		checkPosBuffer1 = new Vec2Di[maxCheckBufferSize];
		checkPosBuffer2 = new Vec2Di[4000];

		b2Vec2 gravity;
		gravity.Set(0.0f, -I::baseClass->gravityFeetPerSec); // -10

		physWorld = new b2World(gravity);

		{//ground
			b2BodyDef bd = b2BodyDef();
			ground = physWorld->CreateBody(&bd);

			b2EdgeShape shape;

			int clumpBottomSize = I::baseClass->particleResolution3X[0];
			shape.SetTwoSided(b2Vec2(-clumpBottomSize, 0.0f), b2Vec2(clumpBottomSize, 0.0f));

			b2FixtureDef fd = b2FixtureDef();
			fd.shape = &shape;
			fd.density = 0.0f;
			fd.friction = 1.f;
			fd.userData.pointer = 2;
			ground->CreateFixture(&fd);

			clumpBottomSize = I::baseClass->particleResolution3X[1] * 8.f;
			shape.SetTwoSided(b2Vec2(-(I::baseClass->particleResolution[0] * 1.5f), clumpBottomSize), b2Vec2(-(I::baseClass->particleResolution[0] * 1.5f), -clumpBottomSize));

			b2FixtureDef fd1 = b2FixtureDef();
			fd1.shape = &shape;
			fd1.density = 0.0f;
			fd1.friction = 1.f;
			fd1.userData.pointer = 2;
			ground->CreateFixture(&fd);

		//	clumpBottomSize = parent->particleResolution3X[1] * 8.f;
			shape.SetTwoSided(b2Vec2((I::baseClass->particleResolution[0] * 1.5f), clumpBottomSize), b2Vec2((I::baseClass->particleResolution[0] * 1.5f), -clumpBottomSize));

			b2FixtureDef fd2 = b2FixtureDef();
			fd2.shape = &shape;
			fd2.density = 0.0f;
			fd2.friction = 1.f;
			fd2.userData.pointer = 2;
			ground->CreateFixture(&fd);
		}

		terrainBodySize = 0;
	}

	~PhysWorldBase()
	{
		for (int i = 0; i < terrainBodySize; i++)
		{
			if (!terrainBodies[i])
				continue;

			physWorld->DestroyBody(terrainBodies[i]);		
		}

		delete[] terrainBodies;

		physWorld->DestroyBody(ground);


		for (int i = 0; i < I::baseClass->particleResolution3X[0]; ++i)
		{
			delete[] particleGroups[i];
		}

		delete[] particleGroups;
		delete[] vertices;
		delete[] checkPosBuffer1;
		delete[] checkPosBuffer2;

		delete physWorld;
	}

	bool BoundX(int& x);
	bool BoundY(int& y);

	bool BoundXY(int& x, int& y);

	void DebugRender();

	void BuildWorldGroups();
	void BuildWorldCollision();
};