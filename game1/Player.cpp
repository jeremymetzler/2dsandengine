#include "header_stack.h"

Player::Player(int nx, int ny)
{

	{
		b2BodyDef bd = b2BodyDef();
		bd.position.Set(nx, ny);
		bd.type = b2_dynamicBody;
		bd.fixedRotation = true;
		bd.allowSleep = false;

		middleRect = I::worldBase->physWorld->CreateBody(&bd); // middle rectangle

		b2PolygonShape shape = b2PolygonShape();
		shape.SetAsBox(6, 7); // this is half the size
		
		b2FixtureDef fd = b2FixtureDef();
		fd.shape = &shape;
		fd.density = 2.5f;
		fd.friction = 0.f;
		fd.userData.pointer = 1;
		middleRect->CreateFixture(&fd);
		
		b2CircleShape shapec = b2CircleShape();
		shapec.m_radius = 6.f;
		shapec.m_p.Set(0, 6.5f);

		middleRect->CreateFixture(&shapec, 2.5f);

	}


	{
		b2CircleShape shape = b2CircleShape();
		shape.m_radius = 6.f;

		b2BodyDef bd = b2BodyDef();
		bd.type = b2_dynamicBody;
		bd.position.Set(nx, ny - 10);
		bd.allowSleep = false;
		bottomCircle = I::worldBase->physWorld->CreateBody(&bd);
		b2FixtureDef fd = b2FixtureDef();
		fd.shape = &shape;
		fd.density = 5.0f;
		fd.friction = 4.f;
		fd.userData.pointer = 1;
		bottomCircle->CreateFixture(&fd);

		b2WheelJointDef jd = b2WheelJointDef();

		// Horizontal
		jd.Initialize(middleRect, bottomCircle, bd.position, b2Vec2(0.0f, 1.0f));

		jd.motorSpeed = 1000;
		jd.maxMotorTorque = 500000.0f;
		jd.enableMotor = true;
		jd.lowerTranslation = -3.0f;
		jd.upperTranslation = 3.0f;
		jd.enableLimit = true;

		float hertz = 2.5f;
		float dampingRatio = 0.7f;
		b2LinearStiffness(jd.stiffness, jd.damping, hertz, dampingRatio, middleRect, bottomCircle);

		wheelJoint = (b2WheelJoint*)I::worldBase->physWorld->CreateJoint(&jd);
	}


	
	x = nx;
	y = ny;
	vx = 0;
	vy = 0;

	rightFootPos.x = x;
	rightFootPos.y = y;
	leftFootPos.x = x;
	leftFootPos.y = y;
}


void Player::Move(int nx, int ny)
{
	float xMoves = nx - x;
	float yMoves = ny - y;

	int moveSum = abs(xMoves) + abs(yMoves);


	float xStep = xMoves / static_cast<float>(moveSum);
	float yStep = yMoves / static_cast<float>(moveSum);

/*	for (int i = 1; i <= moveSum; i++)
	{
		for (int i = 0; i < ptcls.size(); i++)
		{
			Particle* ptcl = ptcls[i];

			Vec2Df Pos = { ptcl->x + (i * xStep), ptcl->y + (i * yStep) };

			if (i == 1 && Particle().CheckPos(Pos.x, Pos.y))
				break;
			else
			{
				if (Particle().CheckPos(Pos.x, Pos.y))
				{
					for (int i = 0; i < ptcls.size(); i++)
					{
						Particle* ptcl = ptcls[i];
						if (!ptcl)
							continue;
						Pos = { ptcl->x + ((i - 1) * xStep), ptcl->y + ((i - 1) * yStep) };
						ptcl->OverridePixelPos(Pos.x, Pos.y);
					}
				}
			}
		}
	}*/

}


void Player::Update()
{
	Vec2Df curPos = { middleRect->GetPosition().x,middleRect->GetPosition().y };

	x = curPos.x;
	y = curPos.y;

	I::baseClass->cameraPos.Set( curPos.x, curPos.y );
	
	static int lastGroundTime = 0;
	for (int i = I::baseClass->milliDelta; i >= I::baseClass->msPerThink; i -= I::baseClass->msPerThink)
	{
		if (lastGroundTime > 0)
		{
			lastGroundTime -= I::baseClass->msPerThink;
		}
		else
			bOnGround = false;

		for (b2ContactEdge* contactList = bottomCircle->GetContactList(); contactList != nullptr; contactList = contactList->next)
		{
			if (contactList)
			{
				if (contactList->contact)
				{
					contactList->contact->SetEnabled(true);
					if (contactList->contact->IsTouching())
					{
						bOnGround = true;
						lastGroundTime = 100;
					}
				}
			}
		}

		Vec2Di cPos = I::chunkBase->TranslatePosToClumpPos({ curPos.x ,curPos.y });

		if (!I::particleBase->BoundClumpX(cPos.x) || !I::particleBase->BoundClumpY(cPos.y))
			return;
		

		int searchIndex = I::chunkBase->mainClump[cPos.x][cPos.y];
		bInWater = false;
		if (searchIndex)
		{
			Particle& p = I::particleBase->Particles->at(searchIndex - 1);

			if (p.classid == WaterP)
			{
				middleRect->SetLinearDamping(3.f);
				bInWater = true;
			}
		}
		else if (middleRect->GetLinearDamping() != 0.f)
			middleRect->SetLinearDamping(0.f);

		float maxSpeed = 30.f;
		if (I::inputs->IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
			maxSpeed = 50.f;

		if (I::inputs->IsKeyHeld(GLFW_KEY_S))
		{
			wheelJoint->SetLimits(5, 6);
		}
		else if (wheelJoint->GetUpperLimit() != 3)
			wheelJoint->SetLimits(-3, 3);

		if (I::inputs->IsKeyHeld(GLFW_KEY_A) && middleRect->GetLinearVelocity().Length() < maxSpeed && bOnGround)
		{
			b2Vec2 leftVec = { -5000.f, 0.f };
//			middleRect->ApplyLinearImpulseToCenter(leftVec, true);

			wheelJoint->SetMotorSpeed(-leftVec.x);
		}
		else if (I::inputs->IsKeyHeld(GLFW_KEY_D) && middleRect->GetLinearVelocity().Length() < maxSpeed && bOnGround)
		{
			b2Vec2 rightVec = { 5000.f, 0.f };
//			middleRect->ApplyLinearImpulseToCenter(rightVec, true);
			
			wheelJoint->SetMotorSpeed(-rightVec.x);
		}
		else if (!bOnGround)
		{
			wheelJoint->SetMotorSpeed(0);
			if (I::inputs->IsKeyHeld(GLFW_KEY_A) && middleRect->GetLinearVelocity().x > -30.f)
			{
				b2Vec2 leftVec = { -1000.f, 0.f };
							middleRect->ApplyLinearImpulseToCenter(leftVec, true);

			}
			else if (I::inputs->IsKeyHeld(GLFW_KEY_D) && middleRect->GetLinearVelocity().x < 30.f)
			{
				b2Vec2 rightVec = { 1000.f, 0.f };
							middleRect->ApplyLinearImpulseToCenter(rightVec, true);

			}
		}
		else
			wheelJoint->SetMotorSpeed(0);

		static bool bIsHolding = false;

		if (I::inputs->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			if (!bIsHolding)
			{
				RayCastClosestCallback callback(0);

				I::worldBase->physWorld->RayCast(&callback, { static_cast<float>(x),static_cast<float>(y) }, { I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y });

				if (callback.m_hit && callback.m_fixture)
				{			
					b2MotorJointDef mjd;
	
					mjd.Initialize2(middleRect, callback.m_fixture->GetBody(), callback.m_point);
					mjd.maxForce = 10000000.0f;
					mjd.maxTorque = 1000000.0f;
					
					armJoint = (b2MotorJoint*)I::worldBase->physWorld->CreateJoint(&mjd);

					bIsHolding = true;
				}
				
			}
			else
			{
				b2Vec2 HoldPos = (b2Vec2(I::inputs->particleMousePosition.x, I::inputs->particleMousePosition.y) - b2Vec2(static_cast<float>(x), static_cast<float>(y)));// + Offsethold;
				armJoint->SetLinearOffset(HoldPos);
			}
		}
		else if (bIsHolding)
		{

			I::worldBase->physWorld->DestroyJoint(armJoint);
			armJoint = nullptr;
			//letgo herer
			bIsHolding = false;
		}


		static bool LegSwitch = false;



	/*	if (Math::CalcDistance({x, y}, rightFootPos) > 15.f && LegSwitch)
		{
			Vec2Di newRPos = cameraPos + Vec2Df(middleRect->GetLinearVelocity().x * 0.3f, -14.f);
			if (Math::CalcDistance(newRPos, leftFootPos) > 12.f)
			{
				rightFootPos = ParticleHandle::TraceRay(x, y, newRPos.x, newRPos.y);
				LegSwitch = !LegSwitch;
			}
		}
		else if (Math::CalcDistance({ x, y }, leftFootPos) > 15.f && !LegSwitch)
		{
			Vec2Di newLPos = cameraPos + Vec2Df(middleRect->GetLinearVelocity().x * 0.3f, -14.f);
			if (Math::CalcDistance(rightFootPos, newLPos) > 12.f)
			{
				leftFootPos = ParticleHandle::TraceRay(x, y, newLPos.x, newLPos.y);
				LegSwitch = !LegSwitch;
			}
		}*/

		Vec2Df feetPivot = curPos - Vec2Df(0.f, 14.f);
		Vec2Df pelvisPos = curPos - Vec2Df(0.f, 4.f);
		static Vec2Df lastFootPos;

		if (LegSwitch) // right foot leading
		{
			Vec2Di newLPos = feetPivot + (feetPivot - leftFootPos);
			Vec2Di newLPosDown = newLPos - Vec2Df(0.f, 14.f);
			rightFootPos = I::particleBase->TraceRay(newLPos.x, newLPos.y, newLPosDown.x, newLPosDown.y);
			float distToDest = fabs(Math::CalcDistance(pelvisPos, rightFootPos));
			rightFootPos.y = rightFootPos.y + (14.f - distToDest) * 1.5f; // FOOT LIFTING
			if (distToDest > 14.f)
			{
				rightFootPos = Math::SimpleProjectPoint(pelvisPos, rightFootPos, 14);
				if (fabs(Math::CalcDistance(pelvisPos, leftFootPos)) > 14.f)
				{
					lastFootPos = leftFootPos;
					LegSwitch = !LegSwitch;
				}
			}
		}
		else// left foot leading
		{
			Vec2Di newRPos = feetPivot + (feetPivot - rightFootPos);
			Vec2Di newRPosDown = newRPos - Vec2Df(0.f, 14.f);
			leftFootPos = I::particleBase->TraceRay(newRPos.x, newRPos.y, newRPosDown.x, newRPosDown.y);

			float distToDest = fabs(Math::CalcDistance(pelvisPos, leftFootPos));
			leftFootPos.y = leftFootPos.y + (14.f - distToDest) * 1.5f;// FOOT LIFTING

			if (distToDest > 14.f)
			{
				leftFootPos = Math::SimpleProjectPoint(pelvisPos, leftFootPos, 14);
				if (fabs(Math::CalcDistance(pelvisPos, rightFootPos)) > 14.f)
				{
					lastFootPos = rightFootPos;
					LegSwitch = !LegSwitch;
				}
			}
		}
	}

	if ((bOnGround || bInWater) && I::inputs->IsKeyPressed(GLFW_KEY_W))
	{
		b2Vec2 upVec = { 0.f, 55000.f };
		middleRect->ApplyLinearImpulseToCenter(upVec, true);
	}

}

void renderLimb(Vec2Df pos, Vec2Df targ, ColorM clr , float limbLen, bool flip)
{
	float limbHalfLen = limbLen * 0.5f;

	float RightAngToPoint = Math::CalcAngle(pos, targ);

	Vec2Df RightMaxArmReach = Math::ProjectPoint(pos, RightAngToPoint, limbLen);

	float RightDistance = fabs(Math::CalcDistance(pos, targ));
	float RightReverseDistanceMaxArmReach = fabs(Math::CalcDistance(RightMaxArmReach, targ));

	float RightArmLength = limbLen + RightReverseDistanceMaxArmReach;

	float RightStepAmnt = (RightArmLength / 90.f);

	float StepAng = RightDistance / RightStepAmnt;

	float RightjointAngle0 = Math::NormalizeAngle(Math::NormalizeAngle(Math::NormalizeAngle((flip ? 180.f + StepAng : -StepAng) + 90.f)) + RightAngToPoint);
	float RightjointAngle1 = Math::NormalizeAngle(RightjointAngle0 + Math::NormalizeAngle(180.f + (2.f * (flip ? -StepAng : StepAng))));

	Vec2Df RightElboPosition = Math::ProjectPoint(pos, RightjointAngle0, limbHalfLen);

	CustRender::drawProjectedLine(pos.x, pos.y, RightjointAngle0, limbHalfLen, clr);
	CustRender::drawProjectedLine(RightElboPosition.x, RightElboPosition.y, RightjointAngle1, limbHalfLen, clr);
}

void Player::Render()
{
	b2Vec2 shoulderPos = middleRect->GetPosition() + b2Vec2(0, 4);
	b2Vec2 pelvisPos = middleRect->GetPosition() - b2Vec2(0,4);

	bool flip = false;
	if (I::inputs->particleMousePosition.x < pelvisPos.x)
		flip = true;

	renderLimb({ pelvisPos.x, pelvisPos.y }, rightFootPos, {1,0,0}, 14.f, flip);
	renderLimb({ pelvisPos.x, pelvisPos.y }, leftFootPos, { 0,1,0 },14.f, flip);

	renderLimb({ shoulderPos.x, shoulderPos.y }, I::inputs->particleMousePosition, { 1,0,0 },14.f, !flip);
	renderLimb({ shoulderPos.x, shoulderPos.y }, I::inputs->particleMousePosition, { 0,1,0 },14.f, !flip);
}

void Player::Teleport(int x, int y)
{
	b2Vec2 telePos = { static_cast<float>( x),static_cast<float>(y) };

	middleRect->SetTransform(telePos, 0.f);
	bottomCircle->SetTransform(telePos, 0.f);
}