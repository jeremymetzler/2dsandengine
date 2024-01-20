#pragma once

class Player
{
public:
//	b2Body* TopCircle = nullptr;
	b2Body* middleRect = nullptr;
	b2Body* bottomCircle = nullptr;

	b2MotorJoint* armJoint = nullptr;

	b2WheelJoint* wheelJoint;

	Player(int x, int y);

	~Player()
	{
		if (armJoint)
			I::worldBase->physWorld->DestroyJoint(armJoint);
		I::worldBase->physWorld->DestroyJoint(wheelJoint);
		I::worldBase->physWorld->DestroyBody(middleRect);
		I::worldBase->physWorld->DestroyBody(bottomCircle);
	}

	void Update();
	void Move(int nx, int ny);
	void Render();
	int x, y;
	double vx, vy;

	bool bOnGround = false;
	bool bInWater = false;

	Vec2Df rightFootPos = { 0,0 };
	Vec2Df leftFootPos = { 0,0 };

	void Teleport(int x, int y);

//	ParticleFormation* ptcls = nullptr;
};