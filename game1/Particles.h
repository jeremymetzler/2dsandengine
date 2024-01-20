#pragma once

class DrawInfo
{
public:
    int pos[2];
    ColorM clr;
};

enum ParticleClass
{
    AirP = 0,
    SandP,
    WaterP,
    PeeP,
    FireP,
    SmokeP,
    WoodP,
    GunPowderP,
    BombP,
    RockGenP,
    UnkP
};

class Particle
{
public:
    int x, y;
    int index = 0;
    //    Vec2Df vel; // velocity
    unsigned char r, g, b;//ColorM clr; // color
    unsigned char classid = 0;
    //    Vec2Di prevPos; // previous position
    int formationIndex = 0;
    int subX = 0, subY = 0;
    char lastrnd = -1;
    unsigned char dormantTick = 0;

    Particle() { Clear(); }

    Particle(int xp, int yp, int type, int ind, float vxp = 0.f, float vyp = 0.f, int sx = 0, int sy = 0)
    {
        x = xp;
        y = yp;

     //   prevPos = pos;
        index = ind;
     //   vel.x = vxp;
      //  vel.y = vyp;

        formationIndex = 0;
        subX = sx;
        subY = sy;

        lastrnd = -1;

        dormantTick = 0;

        SetType(type);
    }

    inline void Set(int xp, int yp, int type, int ind, float vxp = 0.f, float vyp = 0.f, int sx = 0, int sy = 0)
    {
        x = xp;
        y = yp;

     //   prevPos = pos;
        index = ind;
     //   vel.x = vxp;
      //  vel.y = vyp;

        formationIndex = 0;
        subX = sx;
        subY = sy;

        lastrnd = -1;

        dormantTick = 0;

        SetType(type);
    }

    void Draw();

    bool BoundX(int& xp);

    int RetBoundX(int xp);

    bool BoundY(int& yp);
    int RetBoundY(int yp);

    bool SetPos(Vec2Di vPos, bool moveOnly = false);

    int AttemptPosition(int xp, int yp); // returns its own index if no collision else it returns the collided particles index

    int CheckPos(Vec2Di vPos);

    void Swap(Particle& targ);

    int GetDensity();

    bool CanMove(Particle& targ);

    void SetType(int id);

    void SandThink();
    void WaterThink();
    void FireThink();
    void SmokeThink();
    void WoodThink();
    void GunPowderThink();
    void BombThink();

    void Think();

    void Clear();

    void SetColor(unsigned char r, unsigned char g, unsigned char b);
};