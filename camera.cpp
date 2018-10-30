//=============================================================================
//
// �J�������� [camera.cpp]
// Author : GP11A341 21 ���ԗY��
//
//=============================================================================
#include "camera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VIEW_ANGLE			(D3DXToRadian(45.0f))	// ����p
#define	VIEW_ASPECT			((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��
#define	VIEW_NEAR_Z			(00.0f)					// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z			(20000.0f)				// �r���[���ʂ�FarZ�l
#define	VALUE_MOVE_CAMERA	(0.0f)					// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(D3DX_PI * 0.01f)		// �J�����̉�]��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
D3DXVECTOR3		cameraPos;				// �J�����̎��_
D3DXVECTOR3		cameraAt;				// �J�����̒����_
D3DXVECTOR3		g_posCameraU;				// �J�����̏����
D3DXVECTOR3		g_posCameraPDest;			// �J�����̎��_�̖ړI�ʒu
D3DXVECTOR3		g_posCameraRDest;			// �J�����̒����_�̖ړI�ʒu
D3DXVECTOR3		g_rotCamera;				// �J�����̉�]
float			g_fLengthIntervalCamera;	// �J�����̎��_�ƒ����_�̋���
D3DXMATRIX		g_mtxView;					// �r���[�}�g���b�N�X
D3DXMATRIX		g_mtxProjection;			// �v���W�F�N�V�����}�g���b�N�X

//=============================================================================
// �J�����̏�����
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
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}

//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{

}
//=============================================================================
// �J�����̐ݒ菈��
//=============================================================================
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxView);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&g_mtxView, 
						&cameraPos,		// �J�����̎��_
						&cameraAt,		// �J�����̒����_
						&g_posCameraU);		// �J�����̏����

	// �r���[�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &g_mtxView);


	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&g_mtxProjection,
								VIEW_ANGLE,			// ����p
								VIEW_ASPECT,		// �A�X�y�N�g��
								VIEW_NEAR_Z,		// �r���[���ʂ�NearZ�l
								VIEW_FAR_Z);		// �r���[���ʂ�FarZ�l

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &g_mtxProjection);
}

//=============================================================================
// �J�����̌����̎擾
//=============================================================================
D3DXVECTOR3 GetRotCamera(void)
{
	return g_rotCamera;
}

//=============================================================================
// �r���[�}�g���b�N�X�̎擾
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
// �J������������]�s��̎擾
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
// �J�������W�̎擾
//=============================================================================
D3DXVECTOR3 GetCameraPos(void)
{
	return cameraPos;
}