//=====================================
//
//テンプレート処理[template.cpp]
//Author:GP11A341 21 立花雄太
//
//=====================================
#include "instancing.h"

/**************************************
マクロ定義
***************************************/

/**************************************
構造体定義
***************************************/
struct VTX
{
	float x, y, z;
	float u, v;
};

struct UV
{
	float u, v;
};

struct WORLDPOS
{
	float x, y;
};

struct COLOR
{
	float r, g, b, a;
};
/**************************************
グローバル変数
***************************************/
const float screenW = 640.0f;
const float screenH = 480.0f;
const int texPx = 64;
const int tipPx = 64;
const float u = (float)tipPx / texPx;
const int timNumInTex = texPx / tipPx;
const int W = 640 / tipPx;
const int H = 480 / tipPx;
const int tipNum = W * H;

//単位板ポリバッファ
VTX vtx[4] =
{
	{0.0f, 0.0f, 0.0f, 0.0f },
	{tipPx, 0.0f, u, 0.0f },
	{ 0.0f, tipPx, 0.0f, u },
	{tipPx, tipPx, u, u}
};

UV uv[tipNum];
WORLDPOS worldPos[tipNum];
COLOR color[tipNum];
LPDIRECT3DVERTEXBUFFER9 vtxBuff = NULL, uvBuff = NULL, worldPosBuff = NULL, colorBuff = NULL;
LPDIRECT3DVERTEXDECLARATION9 decl;
LPDIRECT3DTEXTURE9 texture;
LPD3DXEFFECT effect;
LPDIRECT3DINDEXBUFFER9 indexBuff;

/**************************************
プロトタイプ宣言
***************************************/
void CopyBuff(unsigned sz, void *src, LPDIRECT3DVERTEXBUFFER9 buff);

/**************************************
初期化処理
***************************************/
void InitInstancing(int num)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->CreateVertexBuffer(sizeof(vtx), 0, 0, D3DPOOL_MANAGED, &vtxBuff, 0);
	pDevice->CreateVertexBuffer(sizeof(WORLDPOS) * tipNum, 0, 0, D3DPOOL_MANAGED, &worldPosBuff, 0);
	pDevice->CreateVertexBuffer(sizeof(UV) * tipNum, 0, 0, D3DPOOL_MANAGED, &uvBuff, 0);
	pDevice->CreateVertexBuffer(sizeof(COLOR) * tipNum, 0, 0, D3DPOOL_MANAGED, &colorBuff, 0);

	for (int i = 0; i < tipNum; i++)
	{
		uv[i].u = 0;
		uv[i].v = 0;

		color[i].r = color[i].g = color[i].b  = 1.0f - (float)i / tipNum;;
		color[i].a = 1.0f;
	}

	for (int w = 0; w < W; w++)
	{
		for (int h = 0; h < H; h++)
		{
			int e = h * W + w;
			worldPos[e].x = tipPx * w;
			worldPos[e].y = tipPx * h;
		}
	}

	CopyBuff(sizeof(vtx), vtx, vtxBuff);
	CopyBuff(sizeof(WORLDPOS) * tipNum, worldPos, worldPosBuff);
	CopyBuff(sizeof(UV) * tipNum, uv, uvBuff);
	CopyBuff(sizeof(COLOR) * tipNum, color, colorBuff);

	D3DVERTEXELEMENT9 declElems[] = {
		{0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
		{3, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
	pDevice->CreateVertexDeclaration(declElems, &decl);

	WORD index[6] = { 0, 1, 2, 2, 1, 3 };
	pDevice->CreateIndexBuffer(sizeof(index), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuff, 0);
	void *p = 0;
	indexBuff->Lock(0, 0, &p, 0);
	memcpy(p, index, sizeof(index));
	indexBuff->Unlock();

	ID3DXBuffer *errBuffer = 0;
	D3DXCreateEffectFromFileA(pDevice, "tip.fx", 0, 0, 0, 0, &effect, &errBuffer);
	if (errBuffer || effect == NULL)
	{
		const char *errMsg = (const char*)errBuffer->GetBufferPointer();
		OutputDebugStringA(errMsg);
		return;
	}

	texture = CreateTextureFromFile((LPSTR)"explosionFlare.png", pDevice);

}

/**************************************
終了処理
***************************************/
void UninitInstancing(void)
{
	SAFE_RELEASE(vtxBuff);
	SAFE_RELEASE(uvBuff);
	SAFE_RELEASE(worldPosBuff);
	SAFE_RELEASE(decl);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(effect);
	SAFE_RELEASE(colorBuff);
}

/**************************************
更新処理
***************************************/
void UpdateInstancing(void)
{

}

/**************************************
描画処理
***************************************/
void DrawInstancing(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | (W * H));
	pDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);
	pDevice->SetStreamSourceFreq(2, D3DSTREAMSOURCE_INSTANCEDATA | 1);
	pDevice->SetStreamSourceFreq(3, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	pDevice->SetVertexDeclaration(decl);
	pDevice->SetStreamSource(0, vtxBuff, 0, sizeof(VTX));
	pDevice->SetStreamSource(1, worldPosBuff, 0, sizeof(WORLDPOS));
	pDevice->SetStreamSource(2, uvBuff, 0, sizeof(UV));
	pDevice->SetStreamSource(3, colorBuff, 0, sizeof(COLOR));
	pDevice->SetIndices(indexBuff);

	effect->SetTechnique("tech");
	UINT passNum = 0;
	effect->Begin(&passNum, 0);
	effect->BeginPass(0);

	effect->SetTexture("tex", texture);
	effect->SetFloat("screenW", screenW / 2);
	effect->SetFloat("screenH", screenH / 2);

	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	effect->EndPass();
	effect->End();

	pDevice->SetStreamSourceFreq(0, 1);
	pDevice->SetStreamSourceFreq(1, 1);
	pDevice->SetStreamSourceFreq(2, 1);
}

/**************************************
メモリコピー
***************************************/
void CopyBuff(unsigned sz, void *src, LPDIRECT3DVERTEXBUFFER9 buff)
{
	void *p = 0;
	buff->Lock(0, 0, &p, 0);
	memcpy(p, src, sz);
	buff->Unlock();
}