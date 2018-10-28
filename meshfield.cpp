//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "meshfield.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define	TEXTURE_FILENAME	"data/TEXTURE/field004.jpg"		// �ǂݍ��ރe�N�X�`���t�@�C����
#define TEXTURE_FILENAME "field000.jpg"
#define MESHFIELD_BLOCK_NUM		(256)
#define MESHFIELD_BLOCK_SIZE	(10)

typedef struct
{
	Vector3 topL, bottomL, bottomR, topR, nor;
}PLANE;

typedef struct
{
	Vector3 a, b, c, nor;
}TRIANGLE;

void SetVertexBuffer(int cnt);
void SetIndexBuffer(void);
bool CheckHitPlaneAndLine(Vector3 start, Vector3 goal, PLANE plane, Vector3 *out);
bool CheckHitTriangleAndLine(Vector3 start, Vector3 end, TRIANGLE tri, Vector3 *out);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DTextureField;			// �e�N�X�`���ǂݍ��ݏꏊ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffField;		// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
LPDIRECT3DINDEXBUFFER9 g_pD3DIdxBuffField;		// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

D3DXMATRIX g_mtxWorldField;						// ���[���h�}�g���b�N�X
D3DXVECTOR3 g_posField;							// �|���S���\���ʒu�̒��S���W
D3DXVECTOR3 g_rotField;							// �|���S���̉�]�p

int numBlockX, numBlockZ;		// �u���b�N��
int g_nNumVertexField;							// �����_��	
int g_nNumVertexIndexField;						// ���C���f�b�N�X��
int g_nNumPolygonField;							// ���|���S����
float sizeBlockX, sizeBlockZ;	// �u���b�N�T�C�Y

static int cntFrame = 0;

static int cntProcess = 0;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �|���S���\���ʒu�̒��S���W��ݒ�
	g_posField = pos;

	g_rotField = rot;

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		TEXTURE_FILENAME,		// �t�@�C���̖��O
		&g_pD3DTextureField);	// �ǂݍ��ރ������[

// �u���b�N���̐ݒ�
	numBlockX = nNumBlockX;
	numBlockZ = nNumBlockZ;

	// ���_���̐ݒ�
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// �|���S�����̐ݒ�
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// �u���b�N�T�C�Y�̐ݒ�
	sizeBlockX = fBlockSizeX;
	sizeBlockZ = fBlockSizeZ;

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_nNumVertexField,	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,					// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_3D,						// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&g_pD3DVtxBuffField,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))								// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateIndexBuffer(sizeof(DWORD) * g_nNumVertexIndexField,		// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,					// ���_�o�b�t�@�̎g�p�@�@
		D3DFMT_INDEX32,						// �g�p����C���f�b�N�X�t�H�[�}�b�g
		D3DPOOL_MANAGED,					// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&g_pD3DIdxBuffField,				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))								// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	SetVertexBuffer(0);
	SetIndexBuffer();

	return S_OK;
}

void SetVertexBuffer(int cnt)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;
#if 0
		const float texSizeX = 1.0f / g_nNumBlockX;
		const float texSizeZ = 1.0f / g_nNumBlockZ;
#else
		const float texSizeX = 1.0f;
		const float texSizeZ = 1.0f;
#endif

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtxZ = 0; nCntVtxZ < (numBlockZ + 1); nCntVtxZ++)
		{
			for (int nCntVtxX = 0; nCntVtxX < (numBlockX + 1); nCntVtxX++)
			{
				// ���_���W�̐ݒ�
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].vtx.x = -(numBlockX / 2.0f) * sizeBlockX + nCntVtxX * sizeBlockX;

				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].vtx.y = 0.0f;		//�����������_���ɂ���΃{�R�{�R�ɂȂ�
				//pVtx[nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX].vtx.y = sinf(nCntVtxX * 50.0f + cnt * 0.1f) * nCntVtxX * 1.0f;
				//pVtx[nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX].vtx.y = heightMap[nCntVtxZ][nCntVtxX];
				//pVtx[nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX].vtx.y = map1[nCntVtxX];

				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].vtx.z = (numBlockZ / 2.0f) * sizeBlockZ - nCntVtxZ * sizeBlockZ;

				// ���ˌ��̐ݒ�
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// �e�N�X�`�����W�̐ݒ�
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxZ;
			}
		}

		for (int nZ = 0; nZ < (numBlockZ + 1); nZ++)
		{
			for (int nX = 0; nX < (numBlockX + 1); nX++)
			{
				// �@���̐ݒ�
				if (nZ == numBlockZ || nX == numBlockX)
				{
					pVtx[nZ * (numBlockX + 1) + nX].nor = D3DXVECTOR3(0.0f, 1.0, 0.0f);
				}
				else {
					int z = nZ * (numBlockX + 1);
					int x = nX;
					Vector3 vec1 = pVtx[z + x + 1].vtx - pVtx[z + x].vtx;
					Vector3 vec2 = pVtx[z + x + numBlockX + 1].vtx - pVtx[z + x].vtx;
					Vector3 normal;
					D3DXVec3Cross(&normal, &vec2, &vec1);
					D3DXVec3Normalize(&pVtx[z + x].nor, &normal);
				}
			}
		}

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffField->Unlock();
	}
}

void SetIndexBuffer(void)
{
	{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
		DWORD *pIdx;

		// �C���f�b�N�X�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DIdxBuffField->Lock(0, 0, (void**)&pIdx, 0);

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < numBlockZ; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (numBlockX + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (numBlockX + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (numBlockX + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (numBlockX + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxZ < (numBlockZ - 1))
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = nCntVtxZ * (numBlockX + 1) + numBlockX;
				nCntIdx++;
			}
		}

		// �C���f�b�N�X�f�[�^���A�����b�N����
		g_pD3DIdxBuffField->Unlock();
	}
}
//=============================================================================
// �I������
//=============================================================================
void UninitMeshField(void)
{
	if (g_pD3DVtxBuffField)
	{// ���_�o�b�t�@�̊J��
		g_pD3DVtxBuffField->Release();
		g_pD3DVtxBuffField = NULL;
	}

	if (g_pD3DIdxBuffField)
	{// �C���f�b�N�X�o�b�t�@�̊J��
		g_pD3DIdxBuffField->Release();
		g_pD3DIdxBuffField = NULL;
	}

	if (g_pD3DTextureField)
	{// �e�N�X�`���̊J��
		g_pD3DTextureField->Release();
		g_pD3DTextureField = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMeshField(void)
{
#if 0
	cntFrame++;
	SetVertexBuffer(cntFrame);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeshField(void)
{return;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTranslate;

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldField);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField.y, g_rotField.x, g_rotField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&mtxTranslate, g_posField.x, g_posField.y, g_posField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField);

	// ���_�o�b�t�@�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetStreamSource(0, g_pD3DVtxBuffField, 0, sizeof(VERTEX_3D));

	// �C���f�b�N�X�o�b�t�@�������_�����O�p�C�v���C���ɐݒ�
	pDevice->SetIndices(g_pD3DIdxBuffField);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_pD3DTextureField);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_nNumVertexField, 0, g_nNumPolygonField);
}

#if 0
bool CheckHitMeshField(Vector3 start, Vector3 end, Vector3 *out)
{
	VERTEX_3D *pVtx;
	PLANE p;
	static int cntFrame = 0, loopx1 = 0, loopx2 = 0, loopz1 = 0, loopz2 = 0;

	//�n�_�ƏI�_�𒲐�
	float sx = start.x + (numBlockX) * sizeBlockX / 2;
	float sz = -start.z + (numBlockZ) * sizeBlockZ;
	float ex = end.x + (numBlockX) * sizeBlockX / 2;
	float ez = -end.z + (numBlockZ) * sizeBlockZ;

	//�e���W������������
	int x1 = (int)(sx * 8 / (sizeBlockX * (numBlockX)));
	int z1 = (int)(sz * 8 / (sizeBlockZ * (numBlockZ)));
	int x2 = (int)(ex * 8 / (sizeBlockX * (numBlockX)));
	int z2 = (int)(ez * 8 / (sizeBlockZ * (numBlockZ)));

	/*
	�܂���4���ؒT���Ō��o�����Ԃ��i�荞��
	*/
	DWORD mortonS = GetMortonNum((WORD)x1, (WORD)z1);	//�n�_�̃��[�g���ԍ����v�Z
	DWORD mortonE = GetMortonNum((WORD)x2, (WORD)z2);	//�I�_�̃��[�g���ԍ����v�Z
	SpaceLevel spaceLevel = CalcSpaceLevel(mortonS ^ mortonE);	//��ԕ������x�����v�Z

	/*
	�i��񍬂񂾂烋�[�v�̎n�_�ƏI�_���v�Z
	*/
	float zLoopStart = (CalcLoopStartZ(spaceLevel, mortonS));
	float zLoopEnd = (CalcLoopEndZ(spaceLevel, mortonS));
	float xLoopStart = (CalcLoopStartX(spaceLevel, mortonS));
	float xLoopEnd = (CalcLoopEndX(spaceLevel, mortonS));

	zLoopStart *= numBlockZ;
	zLoopEnd *= numBlockZ;
	xLoopStart *= numBlockX;
	xLoopEnd *= numBlockX;

	if (cntFrame++ % 10 == 0)
	{
		loopz1 = zLoopStart;
		loopz2 = zLoopEnd;
		loopx1 = xLoopStart;
		loopx2 = xLoopEnd;
	}
	PrintDebugProc("XLOOP : %d �` %d\n", loopx1, loopx2);
	PrintDebugProc("ZLOOP : %d �` %d\n", loopz1, loopz2);

#if 0
	zLoopStart = 0;
	zLoopEnd = numBlockZ;
	xLoopStart = 0;
	xLoopEnd = numBlockX;
#endif

	// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	g_pD3DVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);


	for (int nz = (int)zLoopStart; nz < (int)zLoopEnd; nz++)
	{
		for (int nx = (int)xLoopStart; nx < (int)xLoopEnd + 1; nx++)
		{
			TRIANGLE t = {
				pVtx[nz * (numBlockX + 1) + nx].vtx + g_posField,
				pVtx[(nz + 1) * (numBlockX + 1) + nx].vtx + g_posField,
				pVtx[nz * (numBlockX + 1) + nx + 1].vtx + g_posField,
				Vector3(0, 0, 0)
			};

			D3DXVec3Cross(&t.nor, &(t.b - t.a), &(t.c - t.a));

			if (CheckHitTriangleAndLine(start, end, t, out))
			{
				return true;
			}
		}
	}

	for (int nz = (int)zLoopStart + 1; nz < (int)zLoopEnd + 1; nz++)
	{
		for (int nx = (int)xLoopStart; nx < (int)xLoopEnd + 1; nx++)
		{
			TRIANGLE t =
			{
				pVtx[nz * (numBlockX + 1) + nx].vtx + g_posField,
				pVtx[nz * (numBlockX + 1) + nx + 1].vtx + g_posField,
				pVtx[(nz - 1) * (numBlockX + 1) + nx + 1].vtx + g_posField,
				Vector3(0, 0, 0)
			};

			D3DXVec3Cross(&t.nor, &(t.b - t.a), &(t.c - t.a));

			if (CheckHitTriangleAndLine(start, end, t, out))
			{
				return true;
			}
		}
	}

	return false;
}

bool CheckHitPlaneAndLine(Vector3 start, Vector3 goal, PLANE plane, Vector3 *out)
{

	//�t�B�[���h�̊e���_(�����v���̏��ԂŐݒ肵�Ȃ��ƃo�O��)


	//�����̎n�_�A�I�_�ƃt�B�[���h�̖@���x�N�g���Ƃ̓��ς��v�Z
	Vector3 v1 = start - plane.topL;
	Vector3 v2 = goal - plane.topL;

	float dotV1 = D3DXVec3Dot(&plane.nor, &v1);
	float dotV2 = D3DXVec3Dot(&plane.nor, &v2);

	//���ʂƐ������������Ă��Ȃ�(���ϓ��m�̐ς̕��������ł���)�ꍇ�Afalse�����^�[��
	if (dotV1 * dotV2 > 0)
	{
		return false;
	}

	//�����ƕ��ʂ̌�_���v�Z
	Vector3 line = goal - start;	//������\���x�N�g��

	float ratio = fabsf(dotV1) / (fabsf(dotV1) + fabsf(dotV2));

	float x = start.x + (line.x * ratio);	//x���W
	float y = start.y + (line.y * ratio);	//y���W
	float z = start.z + (line.z * ratio);	//z���W
	Vector3 p = Vector3(x, y, z);			//��_

	//��_���|���S���̈�����`�F�b�N
	Vector3 n1, n2, n3, n4;

	D3DXVec3Cross(&n1, &(plane.bottomL - plane.topL), &(p - plane.bottomL));	//�t�B�[���h�̕ӂƁA���_����_�x�N�g���̖@���x�N�g�����v�Z
	D3DXVec3Cross(&n2, &(plane.bottomR - plane.bottomL), &(p - plane.bottomR));
	D3DXVec3Cross(&n3, &(plane.topR - plane.bottomR), &(p - plane.topR));
	D3DXVec3Cross(&n4, &(plane.topL - plane.topR), &(p - plane.topL));

	float dot_n12 = D3DXVec3Dot(&n1, &n2);
	float dot_n23 = D3DXVec3Dot(&n2, &n3);
	float dot_n34 = D3DXVec3Dot(&n3, &n4);

	//�S�Ă̖@���x�N�g�������������i���ς����ɂȂ�Ȃ��j�ł���Ό�_�̓t�B�[���h���ɂ���
	if (dot_n12 >= 0 && dot_n23 >= 0 && dot_n34 >= 0)
	{
		*out = p;
		return true;
	}

	return false;
}

bool CheckHitTriangleAndLine(Vector3 start, Vector3 end, TRIANGLE tri, Vector3 *out)
{
	//�����̎n�_�A�I�_�ƃt�B�[���h�̖@���x�N�g���Ƃ̓��ς��v�Z
	Vector3 v1 = start - tri.a;
	Vector3 v2 = end - tri.a;

	float dotV1 = D3DXVec3Dot(&tri.nor, &v1);
	float dotV2 = D3DXVec3Dot(&tri.nor, &v2);

	//���ʂƐ������������Ă��Ȃ�(���ϓ��m�̐ς̕��������ł���)�ꍇ�Afalse�����^�[��
	if (dotV1 * dotV2 > 0)
	{
		return false;
	}

	//�����ƕ��ʂ̌�_���v�Z
	Vector3 line = end - start;	//������\���x�N�g��

	float ratio = fabsf(dotV1) / (fabsf(dotV1) + fabsf(dotV2));

	float x = start.x + (line.x * ratio);	//x���W
	float y = start.y + (line.y * ratio);	//y���W
	float z = start.z + (line.z * ratio);	//z���W
	Vector3 p = Vector3(x, y, z);			//��_

											//��_���|���S���̈�����`�F�b�N
	Vector3 n1, n2, n3;

	D3DXVec3Cross(&n1, &(tri.b - tri.a), &(p - tri.a));	//�t�B�[���h�̕ӂƁA���_����_�x�N�g���̖@���x�N�g�����v�Z
	D3DXVec3Cross(&n2, &(tri.c - tri.b), &(p - tri.b));
	D3DXVec3Cross(&n3, &(tri.a - tri.c), &(p - tri.c));

	float dot_n12 = D3DXVec3Dot(&n1, &n2);
	float dot_n23 = D3DXVec3Dot(&n2, &n3);

	//�S�Ă̖@���x�N�g�������������i���ς����ɂȂ�Ȃ��j�ł���Ό�_�̓t�B�[���h���ɂ���
	if (dot_n12 >= 0 && dot_n23 >= 0)
	{
		*out = p;
		return true;
	}

	return false;
}

#endif

