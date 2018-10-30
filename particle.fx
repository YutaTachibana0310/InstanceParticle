texture tex;			//テクスチャ
float4x4 mtxView;		//ビュー行列
float4x4 mtxProj;		//プロジェクション行列

//チップサンプラー（特に気にしなくてよい）
sampler tipSampler = sampler_state {
	texture = <tex>;
	MipFilter = LINEAR;
	MinFilter = POINT;
	MagFilter = POINT;
};

//頂点シェーダ出力構造体
struct VS_OUT {
	float4 pos : POSITION;		//座標
	float2 uv  : TEXCOORD0;		//テクスチャ座標
	float4 color : COLOR0;		//ディフューズ
};

//パーティクルシェーダ出力構造体
struct PS_OUTPUT {
	float4 RGB : COLOR0;		//表示される色
};

//頂点シェーダ（Vertex Shader)
VS_OUT vsMain(
	float3 pos : POSITION,			//引数1：頂点座標
	float2 localUV : TEXCOORD0,		//引数2：頂点のローカルUV
	float4 mtxWorld1 : TEXCOORD1,	//引数3：ワールド変換行列1行目
	float4 mtxWorld2 : TEXCOORD2,	//引数4：ワールド変換行列2行目
	float4 mtxWorld3 : TEXCOORD3,	//引数5：ワールド変換行列3行目
	float4 mtxWorld4 : TEXCOORD4,	//引数6：ワールド変換行列4行目
	float2 tipUV : TEXCOORD5,		//引数7：設定したいテクスチャ座標
	float4 col : COLOR0				//引数8：頂点のディフューズ
) {
	VS_OUT Out;	//出力構造体

	float4x4 mtxWorld = {	//ワールド変換行列を作成
		mtxWorld1,
		mtxWorld2,
		mtxWorld3,
		mtxWorld4,
	};

	//ビューイングパイプラインを行う
	Out.pos = float4(pos, 1.0f);
	Out.pos = mul(Out.pos, mtxWorld);
	Out.pos = mul(Out.pos, mtxView);
	Out.pos = mul(Out.pos, mtxProj);

	//UV座標を設定
	Out.uv = tipUV + localUV;
	
	//ディフューズを設定
	Out.color = col;

	return Out;
}

//ピクセルシェーダ
float4 psMain( VS_OUT In ) : COLOR0 {

	//テクスチャの色とディフューズを乗算して出力
	return tex2D(tipSampler, In.uv) * In.color;
}

//ここをプログラム内でセットして使う
technique tech {
	pass p0 {
		VertexShader = compile vs_2_0 vsMain();
		PixelShader  = compile ps_2_0 psMain();
	}
}
