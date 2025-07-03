//--------------------------------------------
//	ランバート拡散反射計算関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 CalcLambert(float3 N, float3 L, float3 C, float3 K)
{
    float power = saturate(dot(N, L));
    return C * power * K;
}

//--------------------------------------------
//	フォンの鏡面反射計算関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// E:視線ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 CalcPhongSpecular(float3 N, float3 L, float3 V, float3 C, float3 K)
{
    float3 R = reflect(L, N);
    float power = max(dot(V, R), 0);
    power = pow(power, 128);
    return C * power * K;
}

//--------------------------------------------
//	ハーフランバート拡散反射計算関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 ClacHalfLambert(float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, L) * 0.5f + 0.5f);
    return C * D * K;
}

//--------------------------------------------
// リムライト
//--------------------------------------------
// N:法線(正規化済み)
// E:視点方向ベクトル(正規化済み)
// L:入射ベクトル(正規化済み)
// C :ライト色
// RimPower : リムライトの強さ(初期値はテキトーなので自分で設定するが吉)
float3 CalcRimLight(float3 N, float3 V, float3 L, float3 C, float RimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(N, V));
    return C * pow(rim, RimPower) * saturate(dot(L, V));
}

//--------------------------------------------
// ランプシェーディング
//--------------------------------------------
// tex:ランプシェーディング用テクスチャ
// samp:ランプシェーディング用サンプラステート
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 CalcRampShading(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 C, float3 K)
{
    float D = saturate(dot(N, L) * 0.5f + 0.5f);
    float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
    return C * Ramp * K.rgb;
}

//--------------------------------------------
// 半球ライティング
//--------------------------------------------
// normal:法線(正規化済み)
// up:上方向（片方）
// sky_color:空(上)色
// ground_color:地面(下)色
// hemisphere_weight:重み
float3 CalcHemiSphereLight(float3 normal, float3 up, float3 sky_color, float3 ground_color, float4 hemisphere_weight)
{
    float factor = dot(normal, up) * 0.5f + 0.5f;
    return lerp(ground_color, sky_color, factor) * hemisphere_weight.x;
}

//--------------------------------------------
//	フォグ
//--------------------------------------------
//color:現在のピクセル色
//fog_color:フォグの色
//fog_range:フォグの範囲情報
//eye_length:視点からの距離
float4 CalcFog(in float4 color, float4 fogColor, float2 fogRange, float eyeLength)
{
    float fogAlpha = saturate((eyeLength - fogRange.x) / (fogRange.y - fogRange.x));
    return lerp(color, fogColor, fogAlpha);
}

//--------------------------------------------
//	フレネル項
//--------------------------------------------
//F0	: 垂直入射時の反射率
//VdotH	: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1.0f - F0) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f);
}

static const float PI = 3.14159265359f;
//--------------------------------------------
//	拡散反射BRDF(正規化ランバートの拡散反射)
//--------------------------------------------
//VdotH		: 視線へのベクトルとハーフベクトルとの内積
//fresnelF0	: 垂直入射時のフレネル反射色
//diffuseReflectance	: 入射光のうち拡散反射になる割合
float3 DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuseReflectance)
{
    return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuseReflectance / PI);
}

//--------------------------------------------
//	法線分布関数
//--------------------------------------------
//NdotH		: 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
//roughness : 粗さ
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float b = (NdotH * NdotH) * (a - 1.0f) + 1.0f;
    return a / (PI * b * b);
}

//--------------------------------------------
//	幾何減衰項の算出
//--------------------------------------------
//NdotL		: 法線ベクトルと光源へのベクトルとの内積
//NdotV		: 法線ベクトルと視線へのベクトルとの内積
//roughness : 粗さ
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = NdotL / (NdotL * (1.0 - r) + r);
    float masking = NdotV / (NdotV * (1.0 - r) + r);
    return shadowing * masking;
}

//--------------------------------------------
//	鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
//--------------------------------------------
//NdotV		: 法線ベクトルと視線へのベクトルとの内積
//NdotL		: 法線ベクトルと光源へのベクトルとの内積
//NdotH		: 法線ベクトルとハーフベクトルとの内積
//VdotH		: 視線へのベクトルとハーフベクトルとの内積
//fresnelF0	: 垂直入射時のフレネル反射色
//roughness	: 粗さ
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	//	D項(法線分布)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	//	G項(幾何減衰項)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	//	F項(フレネル反射)
    float3 F = (CalcFresnel(fresnelF0, VdotH));

    return (D * F * G) / (4.0 * NdotV * NdotL + 1e-5);
}

//--------------------------------------------
//	直接光の物理ベースライティング
//--------------------------------------------
//diffuseReflectance	: 入射光のうち拡散反射になる割合
//F0					: 垂直入射時のフレネル反射色
//normal				: 法線ベクトル(正規化済み)
//eyeVector				: 視点に向かうベクトル(正規化済み)
//lightVector			: 光源に向かうベクトル(正規化済み)
//lightColor			: ライトカラー
//roughness				: 粗さ
void DirectBDRF(float3 diffuseReflectance,
				float3 F0,
				float3 N,
				float3 V,
				float3 L,
                float3 H,
				float3 lightColor,
				float roughness,
				out float3 outDiffuse,
				out float3 outSpecular)
{
    //float3 N = normal;
    //float3 L = -lightVector;
    //float3 V = -eyeVector;
    //float3 H = normalize(L + V);

    float NdotV = max(0.0001f, dot(N, V));
    float NdotL = max(0.0001f, dot(N, L));
    float NdotH = max(0.0001f, dot(N, H));
    float VdotH = max(0.0001f, dot(V, H));

    float3 irradiance = lightColor * NdotL;

    //	拡散反射BRDF
    outDiffuse = DiffuseBRDF(VdotH, F0, diffuseReflectance) * irradiance;

	//	鏡面反射BRDF
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}


float3 ClosestPointOnLine(float3 p, float3 start, float3 end)
{
    float3 lineDir = end - start;
    float lineLengthSq = dot(lineDir, lineDir);
    float t = dot(p -start, lineDir) / lineLengthSq;
    t = saturate(t);
    return start + t * lineDir;
}
