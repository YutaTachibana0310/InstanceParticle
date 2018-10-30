#include "main.h"

//単位パーティクル構造体
struct PARTICLE_VTX
{
	float x, y, z;
	float u, v;
};

//テクスチャ座標構造体
struct UV
{
	float u, v;
};

//ディフューズ構造体
struct COLOR
{
	float r, g, b, a;
};

//パーティクルパラメータ構造体
typedef struct
{
	bool active = false;
	int cntFrame = 0;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 moveDir;
	float speed;
	int lifeFrame;
	float scale;

}PARTICLE;

//プロトタイプ宣言
void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);