//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : 
//
//=============================================================================
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

#include "main.h"

typedef D3DXVECTOR3 Vector3;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshField(D3DXVECTOR3 pos, D3DXVECTOR3 rot,
						int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);
bool CheckHitMeshField(Vector3 start, Vector3 goal, Vector3 *out);

#endif
