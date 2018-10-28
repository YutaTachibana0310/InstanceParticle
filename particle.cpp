#include "particle.h"
#include "camera.h"

#define PARTICLE_MAX (2048)
#define PARTICLE_TEXNAME "explosionFlare.png"
#define PARTICLE_SIZE (64)
#define USE_SHADER (1)

static D3DXMATRIX pos[PARTICLE_MAX];
static COLOR vtxColor[PARTICLE_MAX];
static UV vtxUV[PARTICLE_MAX];

static PARTICLE_VTX vtx[4] = {
#if 1
	{-PARTICLE_SIZE, 0.0f, PARTICLE_SIZE, 0.0f, 0.0f},
	{PARTICLE_SIZE, 0.0f, PARTICLE_SIZE, 1.0f, 0.0f},
	{-PARTICLE_SIZE, 0.0f, -PARTICLE_SIZE,  0.0f, 1.0f},
	{PARTICLE_SIZE, 0.0f, -PARTICLE_SIZE, 1.0f, 1.0f}
#else
	{-PARTICLE_SIZE, PARTICLE_SIZE, 0.0f, 0.0f, 0.0f},
	{ PARTICLE_SIZE, PARTICLE_SIZE, 0.0f, 1.0f, 0.0f },
	{ -PARTICLE_SIZE, -PARTICLE_SIZE, 0.0f,  0.0f, 1.0f },
	{ PARTICLE_SIZE, -PARTICLE_SIZE, 0.0f, 1.0f, 1.0f }
#endif
};

static LPDIRECT3DVERTEXBUFFER9 vtxBuff = NULL, uvBuff = NULL, posBuff = NULL, colorBuff = NULL;
static LPDIRECT3DVERTEXDECLARATION9 declare;
static LPDIRECT3DTEXTURE9 texture;
static LPD3DXEFFECT effect;
static LPDIRECT3DINDEXBUFFER9 indexBuff;

static PARTICLE particle[PARTICLE_MAX];

void CopyVtxBuff(unsigned size, void *src, LPDIRECT3DVERTEXBUFFER9 buff);

LPDIRECT3DVERTEXBUFFER9 hoge = NULL;

void InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//配列初期化
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		particle[i].pos = D3DXVECTOR3(rand() % 1000 - 500, rand() % 1000 - 500, rand() % 1000 + 500);
		D3DXMatrixIdentity(&pos[i]);
		vtxUV[i].u = vtxUV[i].v = 0.0f;
		vtxColor[i].r = vtxColor[i].g = vtxColor[i].g = vtxColor[i].a = 1.0f;
	}

	//頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(vtx), 0, 0, D3DPOOL_MANAGED, &vtxBuff, 0);
	pDevice->CreateVertexBuffer(sizeof(D3DXMATRIX) * PARTICLE_MAX, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &posBuff, 0);
	pDevice->CreateVertexBuffer(sizeof(UV) * PARTICLE_MAX, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &uvBuff, 0);
	pDevice->CreateVertexBuffer(sizeof(COLOR) * PARTICLE_MAX, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &colorBuff, 0);

#if !USE_SHADER
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &hoge, 0);
	VERTEX_3D *pVtx = NULL;
	hoge->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].vtx = D3DXVECTOR3(-PARTICLE_SIZE, 0.0f, PARTICLE_SIZE);
	pVtx[1].vtx = D3DXVECTOR3(PARTICLE_SIZE, 0.0f, PARTICLE_SIZE);
	pVtx[2].vtx = D3DXVECTOR3(-PARTICLE_SIZE, 0.0f, -PARTICLE_SIZE);
	pVtx[3].vtx = D3DXVECTOR3(PARTICLE_SIZE, 0.0f, -PARTICLE_SIZE);

	pVtx[0].nor = pVtx[1].nor = pVtx[2].nor = pVtx[3].nor = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	pVtx[0].diffuse = pVtx[1].diffuse = pVtx[2].diffuse = pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	hoge->Unlock();
#endif

	CopyVtxBuff(sizeof(vtx), vtx, vtxBuff);
	CopyVtxBuff(sizeof(D3DXMATRIX) * PARTICLE_MAX, pos, posBuff);
	CopyVtxBuff(sizeof(UV) * PARTICLE_MAX, vtxUV, uvBuff);
	CopyVtxBuff(sizeof(COLOR) * PARTICLE_MAX, vtxColor, colorBuff);

	//頂点宣言作成
#if 0
	D3DVERTEXELEMENT9 declareElems[] = {
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
		{3, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
#else
	D3DVERTEXELEMENT9 declareElems[] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 1, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		{ 2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5 },
		{ 3, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
#endif
	pDevice->CreateVertexDeclaration(declareElems, &declare);

	//インデックスバッファ作成
	WORD index[6] = { 0, 1, 2, 2, 1, 3 };
	pDevice->CreateIndexBuffer(sizeof(index), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuff, 0);
	void *p = NULL;
	indexBuff->Lock(0, 0, &p, 0);
	memcpy(p, index, sizeof(index));
	indexBuff->Unlock();

	//シェーダー読み込み
	ID3DXBuffer *errBuf = 0;
	D3DXCreateEffectFromFile(pDevice, "particle.fx", 0, 0, 0, 0, &effect, &errBuf);
	if (errBuf)
	{
		//エラー処理
	}

	//テクスチャ読み込み
	texture = CreateTextureFromFile((LPSTR)PARTICLE_TEXNAME, pDevice);
}

void UninitParticle(void)
{
	SAFE_RELEASE(vtxBuff);
	SAFE_RELEASE(uvBuff);
	SAFE_RELEASE(posBuff);
	SAFE_RELEASE(colorBuff);
	SAFE_RELEASE(declare);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(effect);
	SAFE_RELEASE(hoge);
}

void UpdateParticle(void)
{
	PARTICLE *ptr = &particle[0];
	D3DXMATRIX mtxTranslate;

	for (int i = 0; i < PARTICLE_MAX; i++, ptr++)
	{
		D3DXMatrixIdentity(&pos[i]);

		GetInvCameraRotMtx(&pos[i]);

		D3DXMatrixTranslation(&mtxTranslate, ptr->pos.x, ptr->pos.y, ptr->pos.z);
		D3DXMatrixMultiply(&pos[i], &pos[i], &mtxTranslate);
	}

	CopyVtxBuff(sizeof(D3DXMATRIX) * PARTICLE_MAX, pos, posBuff);
}

void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX world, translate, mtxRot;

	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

#if USE_SHADER
	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | (PARTICLE_MAX));
	pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);
	pDevice->SetStreamSourceFreq(2, D3DSTREAMSOURCE_INSTANCEDATA | 1);
	pDevice->SetStreamSourceFreq(3, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	pDevice->SetVertexDeclaration(declare);
	pDevice->SetStreamSource(0, vtxBuff, 0, sizeof(PARTICLE_VTX));
	pDevice->SetStreamSource(1, posBuff, 0, sizeof(D3DXMATRIX));
	pDevice->SetStreamSource(2, uvBuff, 0, sizeof(UV));
	pDevice->SetStreamSource(3, colorBuff, 0, sizeof(COLOR));
	pDevice->SetIndices(indexBuff);

	effect->SetTechnique("tech");
	UINT passNum = 0;
	effect->Begin(&passNum, 0);
	effect->BeginPass(0);

	effect->SetTexture("tex", texture);
	effect->SetMatrix("mtxView", &GetMtxView());
	effect->SetMatrix("mtxProj", &GetMtxProjection());
	effect->SetMatrix("world", &world);

	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	effect->EndPass();
	effect->End();

	pDevice->SetStreamSourceFreq(0, 1);
	pDevice->SetStreamSourceFreq(1, 1);
	pDevice->SetStreamSourceFreq(2, 1);


#else

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		D3DXMatrixIdentity(&world);

		//D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
		//D3DXMatrixMultiply(&world, &world, &mtxRot);

		//D3DXMatrixTranslation(&translate, 0.0f, 10.0f, 500.0f);
		//D3DXMatrixMultiply(&world, &world, &translate);

		D3DXMatrixMultiply(&world, &world, &pos[i]);

		pDevice->SetTransform(D3DTS_WORLD, &world);

		pDevice->SetStreamSource(0, hoge, 0, sizeof(VERTEX_3D));

		pDevice->SetIndices(indexBuff);

		pDevice->SetFVF(FVF_VERTEX_3D);

		pDevice->SetTexture(0, texture);

		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
#endif
	pDevice->SetRenderState(D3DRS_ZENABLE, true);
}

void CopyVtxBuff(unsigned size, void *src, LPDIRECT3DVERTEXBUFFER9 buff)
{
	void *p = NULL;
	buff->Lock(0, 0, &p, 0);
	memcpy(p, src, size);
	buff->Unlock();
}

