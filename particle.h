#include "main.h"

struct PARTICLE_VTX
{
	float x, y, z;
	float u, v;
};

struct UV
{
	float u, v;
};

struct COLOR
{
	float r, g, b, a;
};

typedef struct
{
	bool active = false;
	int cntFrame = 0;
	D3DXVECTOR3 pos;
}PARTICLE;

void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);