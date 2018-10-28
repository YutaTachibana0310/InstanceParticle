//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "meshfield.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	TEXTURE_FILENAME	"data/TEXTURE/field004.jpg"		// 読み込むテクスチャファイル名
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
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pD3DTextureField;			// テクスチャ読み込み場所
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffField;		// 頂点バッファインターフェースへのポインタ
LPDIRECT3DINDEXBUFFER9 g_pD3DIdxBuffField;		// インデックスバッファインターフェースへのポインタ

D3DXMATRIX g_mtxWorldField;						// ワールドマトリックス
D3DXVECTOR3 g_posField;							// ポリゴン表示位置の中心座標
D3DXVECTOR3 g_rotField;							// ポリゴンの回転角

int numBlockX, numBlockZ;		// ブロック数
int g_nNumVertexField;							// 総頂点数	
int g_nNumVertexIndexField;						// 総インデックス数
int g_nNumPolygonField;							// 総ポリゴン数
float sizeBlockX, sizeBlockZ;	// ブロックサイズ

static int cntFrame = 0;

static int cntProcess = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ポリゴン表示位置の中心座標を設定
	g_posField = pos;

	g_rotField = rot;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
		TEXTURE_FILENAME,		// ファイルの名前
		&g_pD3DTextureField);	// 読み込むメモリー

// ブロック数の設定
	numBlockX = nNumBlockX;
	numBlockZ = nNumBlockZ;

	// 頂点数の設定
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	sizeBlockX = fBlockSizeX;
	sizeBlockZ = fBlockSizeZ;

	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * g_nNumVertexField,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,					// 頂点バッファの使用法　
		FVF_VERTEX_3D,						// 使用する頂点フォーマット
		D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
		&g_pD3DVtxBuffField,				// 頂点バッファインターフェースへのポインタ
		NULL)))								// NULLに設定
	{
		return E_FAIL;
	}

	// オブジェクトのインデックスバッファを生成
	if (FAILED(pDevice->CreateIndexBuffer(sizeof(DWORD) * g_nNumVertexIndexField,		// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,					// 頂点バッファの使用法　
		D3DFMT_INDEX32,						// 使用するインデックスフォーマット
		D3DPOOL_MANAGED,					// リソースのバッファを保持するメモリクラスを指定
		&g_pD3DIdxBuffField,				// インデックスバッファインターフェースへのポインタ
		NULL)))								// NULLに設定
	{
		return E_FAIL;
	}

	SetVertexBuffer(0);
	SetIndexBuffer();

	return S_OK;
}

void SetVertexBuffer(int cnt)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;
#if 0
		const float texSizeX = 1.0f / g_nNumBlockX;
		const float texSizeZ = 1.0f / g_nNumBlockZ;
#else
		const float texSizeX = 1.0f;
		const float texSizeZ = 1.0f;
#endif

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtxZ = 0; nCntVtxZ < (numBlockZ + 1); nCntVtxZ++)
		{
			for (int nCntVtxX = 0; nCntVtxX < (numBlockX + 1); nCntVtxX++)
			{
				// 頂点座標の設定
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].vtx.x = -(numBlockX / 2.0f) * sizeBlockX + nCntVtxX * sizeBlockX;

				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].vtx.y = 0.0f;		//ここをランダムにすればボコボコになる
				//pVtx[nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX].vtx.y = sinf(nCntVtxX * 50.0f + cnt * 0.1f) * nCntVtxX * 1.0f;
				//pVtx[nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX].vtx.y = heightMap[nCntVtxZ][nCntVtxX];
				//pVtx[nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX].vtx.y = map1[nCntVtxX];

				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].vtx.z = (numBlockZ / 2.0f) * sizeBlockZ - nCntVtxZ * sizeBlockZ;

				// 反射光の設定
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
				pVtx[nCntVtxZ * (numBlockX + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxZ;
			}
		}

		for (int nZ = 0; nZ < (numBlockZ + 1); nZ++)
		{
			for (int nX = 0; nX < (numBlockX + 1); nX++)
			{
				// 法線の設定
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

		// 頂点データをアンロックする
		g_pD3DVtxBuffField->Unlock();
	}
}

void SetIndexBuffer(void)
{
	{//インデックスバッファの中身を埋める
		DWORD *pIdx;

		// インデックスデータの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DIdxBuffField->Lock(0, 0, (void**)&pIdx, 0);

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < numBlockZ; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
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
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (numBlockX + 1) + numBlockX;
				nCntIdx++;
			}
		}

		// インデックスデータをアンロックする
		g_pD3DIdxBuffField->Unlock();
	}
}
//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if (g_pD3DVtxBuffField)
	{// 頂点バッファの開放
		g_pD3DVtxBuffField->Release();
		g_pD3DVtxBuffField = NULL;
	}

	if (g_pD3DIdxBuffField)
	{// インデックスバッファの開放
		g_pD3DIdxBuffField->Release();
		g_pD3DIdxBuffField = NULL;
	}

	if (g_pD3DTextureField)
	{// テクスチャの開放
		g_pD3DTextureField->Release();
		g_pD3DTextureField = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
#if 0
	cntFrame++;
	SetVertexBuffer(cntFrame);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{return;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTranslate;

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldField);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField.y, g_rotField.x, g_rotField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posField.x, g_posField.y, g_posField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxTranslate);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField);

	// 頂点バッファをレンダリングパイプラインに設定
	pDevice->SetStreamSource(0, g_pD3DVtxBuffField, 0, sizeof(VERTEX_3D));

	// インデックスバッファをレンダリングパイプラインに設定
	pDevice->SetIndices(g_pD3DIdxBuffField);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pD3DTextureField);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, g_nNumVertexField, 0, g_nNumPolygonField);
}

#if 0
bool CheckHitMeshField(Vector3 start, Vector3 end, Vector3 *out)
{
	VERTEX_3D *pVtx;
	PLANE p;
	static int cntFrame = 0, loopx1 = 0, loopx2 = 0, loopz1 = 0, loopz2 = 0;

	//始点と終点を調整
	float sx = start.x + (numBlockX) * sizeBlockX / 2;
	float sz = -start.z + (numBlockZ) * sizeBlockZ;
	float ex = end.x + (numBlockX) * sizeBlockX / 2;
	float ez = -end.z + (numBlockZ) * sizeBlockZ;

	//各座標をいい感じに
	int x1 = (int)(sx * 8 / (sizeBlockX * (numBlockX)));
	int z1 = (int)(sz * 8 / (sizeBlockZ * (numBlockZ)));
	int x2 = (int)(ex * 8 / (sizeBlockX * (numBlockX)));
	int z2 = (int)(ez * 8 / (sizeBlockZ * (numBlockZ)));

	/*
	まずは4分木探索で検出する空間を絞り込む
	*/
	DWORD mortonS = GetMortonNum((WORD)x1, (WORD)z1);	//始点のモートン番号を計算
	DWORD mortonE = GetMortonNum((WORD)x2, (WORD)z2);	//終点のモートン番号を計算
	SpaceLevel spaceLevel = CalcSpaceLevel(mortonS ^ mortonE);	//空間分割レベルを計算

	/*
	絞りん混んだらループの始点と終点を計算
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
	PrintDebugProc("XLOOP : %d ～ %d\n", loopx1, loopx2);
	PrintDebugProc("ZLOOP : %d ～ %d\n", loopz1, loopz2);

#if 0
	zLoopStart = 0;
	zLoopEnd = numBlockZ;
	xLoopStart = 0;
	xLoopEnd = numBlockX;
#endif

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
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

	//フィールドの各頂点(反時計回りの順番で設定しないとバグる)


	//線分の始点、終点とフィールドの法線ベクトルとの内積を計算
	Vector3 v1 = start - plane.topL;
	Vector3 v2 = goal - plane.topL;

	float dotV1 = D3DXVec3Dot(&plane.nor, &v1);
	float dotV2 = D3DXVec3Dot(&plane.nor, &v2);

	//平面と線分が交差していない(内積同士の積の符号が正である)場合、falseをリターン
	if (dotV1 * dotV2 > 0)
	{
		return false;
	}

	//線分と平面の交点を計算
	Vector3 line = goal - start;	//線分を表すベクトル

	float ratio = fabsf(dotV1) / (fabsf(dotV1) + fabsf(dotV2));

	float x = start.x + (line.x * ratio);	//x座標
	float y = start.y + (line.y * ratio);	//y座標
	float z = start.z + (line.z * ratio);	//z座標
	Vector3 p = Vector3(x, y, z);			//交点

	//交点がポリゴン領域内かチェック
	Vector3 n1, n2, n3, n4;

	D3DXVec3Cross(&n1, &(plane.bottomL - plane.topL), &(p - plane.bottomL));	//フィールドの辺と、頂点→交点ベクトルの法線ベクトルを計算
	D3DXVec3Cross(&n2, &(plane.bottomR - plane.bottomL), &(p - plane.bottomR));
	D3DXVec3Cross(&n3, &(plane.topR - plane.bottomR), &(p - plane.topR));
	D3DXVec3Cross(&n4, &(plane.topL - plane.topR), &(p - plane.topL));

	float dot_n12 = D3DXVec3Dot(&n1, &n2);
	float dot_n23 = D3DXVec3Dot(&n2, &n3);
	float dot_n34 = D3DXVec3Dot(&n3, &n4);

	//全ての法線ベクトルが同じ向き（内積が負にならない）であれば交点はフィールド内にある
	if (dot_n12 >= 0 && dot_n23 >= 0 && dot_n34 >= 0)
	{
		*out = p;
		return true;
	}

	return false;
}

bool CheckHitTriangleAndLine(Vector3 start, Vector3 end, TRIANGLE tri, Vector3 *out)
{
	//線分の始点、終点とフィールドの法線ベクトルとの内積を計算
	Vector3 v1 = start - tri.a;
	Vector3 v2 = end - tri.a;

	float dotV1 = D3DXVec3Dot(&tri.nor, &v1);
	float dotV2 = D3DXVec3Dot(&tri.nor, &v2);

	//平面と線分が交差していない(内積同士の積の符号が正である)場合、falseをリターン
	if (dotV1 * dotV2 > 0)
	{
		return false;
	}

	//線分と平面の交点を計算
	Vector3 line = end - start;	//線分を表すベクトル

	float ratio = fabsf(dotV1) / (fabsf(dotV1) + fabsf(dotV2));

	float x = start.x + (line.x * ratio);	//x座標
	float y = start.y + (line.y * ratio);	//y座標
	float z = start.z + (line.z * ratio);	//z座標
	Vector3 p = Vector3(x, y, z);			//交点

											//交点がポリゴン領域内かチェック
	Vector3 n1, n2, n3;

	D3DXVec3Cross(&n1, &(tri.b - tri.a), &(p - tri.a));	//フィールドの辺と、頂点→交点ベクトルの法線ベクトルを計算
	D3DXVec3Cross(&n2, &(tri.c - tri.b), &(p - tri.b));
	D3DXVec3Cross(&n3, &(tri.a - tri.c), &(p - tri.c));

	float dot_n12 = D3DXVec3Dot(&n1, &n2);
	float dot_n23 = D3DXVec3Dot(&n2, &n3);

	//全ての法線ベクトルが同じ向き（内積が負にならない）であれば交点はフィールド内にある
	if (dot_n12 >= 0 && dot_n23 >= 0)
	{
		*out = p;
		return true;
	}

	return false;
}

#endif

