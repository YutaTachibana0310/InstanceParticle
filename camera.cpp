//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : GP11A341 21 立花雄太
//
//=============================================================================
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VIEW_ANGLE			(D3DXToRadian(45.0f))	// 視野角
#define	VIEW_ASPECT			((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比
#define	VIEW_NEAR_Z			(00.0f)					// ビュー平面のNearZ値
#define	VIEW_FAR_Z			(20000.0f)				// ビュー平面のFarZ値
#define	VALUE_MOVE_CAMERA	(0.0f)					// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(D3DX_PI * 0.01f)		// カメラの回転量

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DXVECTOR3		cameraPos;				// カメラの視点
D3DXVECTOR3		cameraAt;				// カメラの注視点
D3DXVECTOR3		g_posCameraU;				// カメラの上方向
D3DXVECTOR3		g_posCameraPDest;			// カメラの視点の目的位置
D3DXVECTOR3		g_posCameraRDest;			// カメラの注視点の目的位置
D3DXVECTOR3		g_rotCamera;				// カメラの回転
float			g_fLengthIntervalCamera;	// カメラの視点と注視点の距離
D3DXMATRIX		g_mtxView;					// ビューマトリックス
D3DXMATRIX		g_mtxProjection;			// プロジェクションマトリックス

//=============================================================================
// カメラの初期化
//=============================================================================
HRESULT InitCamera(void)
{
	cameraPos = D3DXVECTOR3(0.0f, 100.0f, -100.0f);
	cameraAt = D3DXVECTOR3(0.0f, 90.0f, 200.0f);
	g_posCameraU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	g_posCameraPDest = D3DXVECTOR3(0.0f, 200.0f, -200.0f);
	g_posCameraRDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotCamera = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	float vx,vz;
	vx = cameraPos.x - cameraAt.x;
	vz = cameraPos.z - cameraAt.z;
	g_fLengthIntervalCamera = sqrtf(vx * vx + vz * vz);

	return S_OK;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{

}
//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_mtxView, 
						&cameraPos,		// カメラの視点
						&cameraAt,		// カメラの注視点
						&g_posCameraU);		// カメラの上方向

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_mtxView);


	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_mtxProjection,
								VIEW_ANGLE,			// 視野角
								VIEW_ASPECT,		// アスペクト比
								VIEW_NEAR_Z,		// ビュー平面のNearZ値
								VIEW_FAR_Z);		// ビュー平面のFarZ値

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_mtxProjection);
}

//=============================================================================
// カメラの向きの取得
//=============================================================================
D3DXVECTOR3 GetRotCamera(void)
{
	return g_rotCamera;
}

//=============================================================================
// ビューマトリックスの取得
//=============================================================================
D3DXMATRIX GetMtxView(void)
{
	return g_mtxView;
}

D3DXMATRIX GetMtxProjection(void)
{
	return g_mtxProjection;
}

//=============================================================================
// カメラを向く回転行列の取得
//=============================================================================
void GetInvCameraRotMtx(D3DXMATRIX *mtx)
{
	/*
	D3DXMATRIX inv;
	D3DXMatrixIdentity(&inv);
	D3DXMatrixLookAtLH(&inv, &cameraPos, objPos, &g_posCameraU);
	D3DXMatrixInverse(&inv, NULL, &inv);
	inv._41 = 0.0f;
	inv._42 = 0.0f;
	inv._43 = 0.0f;
	*/

	mtx->_11 = g_mtxView._11;
	mtx->_12 = g_mtxView._21;
	mtx->_13 = g_mtxView._31;

	mtx->_21 = g_mtxView._12;
	mtx->_22 = g_mtxView._22;
	mtx->_23 = g_mtxView._32;

	mtx->_31 = g_mtxView._13;
	mtx->_32 = g_mtxView._23;
	mtx->_33 = g_mtxView._33;

	mtx->_41 = 0.0f;
	mtx->_42 = 0.0f;
	mtx->_43 = 0.0f;
}

//=============================================================================
// カメラ座標の取得
//=============================================================================
D3DXVECTOR3 GetCameraPos(void)
{
	return cameraPos;
}