texture tex;			//�e�N�X�`��
float4x4 mtxView;		//�r���[�s��
float4x4 mtxProj;		//�v���W�F�N�V�����s��

//�`�b�v�T���v���[�i���ɋC�ɂ��Ȃ��Ă悢�j
sampler tipSampler = sampler_state {
	texture = <tex>;
	MipFilter = LINEAR;
	MinFilter = POINT;
	MagFilter = POINT;
};

//���_�V�F�[�_�o�͍\����
struct VS_OUT {
	float4 pos : POSITION;		//���W
	float2 uv  : TEXCOORD0;		//�e�N�X�`�����W
	float4 color : COLOR0;		//�f�B�t���[�Y
};

//�p�[�e�B�N���V�F�[�_�o�͍\����
struct PS_OUTPUT {
	float4 RGB : COLOR0;		//�\�������F
};

//���_�V�F�[�_�iVertex Shader)
VS_OUT vsMain(
	float3 pos : POSITION,			//����1�F���_���W
	float2 localUV : TEXCOORD0,		//����2�F���_�̃��[�J��UV
	float4 mtxWorld1 : TEXCOORD1,	//����3�F���[���h�ϊ��s��1�s��
	float4 mtxWorld2 : TEXCOORD2,	//����4�F���[���h�ϊ��s��2�s��
	float4 mtxWorld3 : TEXCOORD3,	//����5�F���[���h�ϊ��s��3�s��
	float4 mtxWorld4 : TEXCOORD4,	//����6�F���[���h�ϊ��s��4�s��
	float2 tipUV : TEXCOORD5,		//����7�F�ݒ肵�����e�N�X�`�����W
	float4 col : COLOR0				//����8�F���_�̃f�B�t���[�Y
) {
	VS_OUT Out;	//�o�͍\����

	float4x4 mtxWorld = {	//���[���h�ϊ��s����쐬
		mtxWorld1,
		mtxWorld2,
		mtxWorld3,
		mtxWorld4,
	};

	//�r���[�C���O�p�C�v���C�����s��
	Out.pos = float4(pos, 1.0f);
	Out.pos = mul(Out.pos, mtxWorld);
	Out.pos = mul(Out.pos, mtxView);
	Out.pos = mul(Out.pos, mtxProj);

	//UV���W��ݒ�
	Out.uv = tipUV + localUV;
	
	//�f�B�t���[�Y��ݒ�
	Out.color = col;

	return Out;
}

//�s�N�Z���V�F�[�_
float4 psMain( VS_OUT In ) : COLOR0 {

	//�e�N�X�`���̐F�ƃf�B�t���[�Y����Z���ďo��
	return tex2D(tipSampler, In.uv) * In.color;
}

//�������v���O�������ŃZ�b�g���Ďg��
technique tech {
	pass p0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader  = compile ps_2_0 psMain();
	}
}
