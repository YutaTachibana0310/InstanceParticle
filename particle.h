#include "main.h"

//�P�ʃp�[�e�B�N���\����
struct PARTICLE_VTX
{
	float x, y, z;
	float u, v;
};

//�e�N�X�`�����W�\����
struct UV
{
	float u, v;
};

//�f�B�t���[�Y�\����
struct COLOR
{
	float r, g, b, a;
};

//�p�[�e�B�N���p�����[�^�\����
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

//�v���g�^�C�v�錾
void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);