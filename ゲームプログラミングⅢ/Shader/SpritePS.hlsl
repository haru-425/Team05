#include "sprite.hlsli"

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

#define TEXTURE_WIDTH 240
#define TEXTURE_HEIGHT 200
//#define PARAMETER_MAX 360;
float rand(float2 co)
{
    // 疑似ランダム（パターンノイズ用）
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}

// ピクセルシェーダーエントリポイント
float4 main(VS_OUT pin) : SV_TARGET
{

   float len;
   if (flag == 1)
   {
        float2 center_1 = float2((TEXTURE_WIDTH / 2.0f) * screen_size.x, ((720 - TEXTURE_HEIGHT) + TEXTURE_HEIGHT / 2.0f) * screen_size.x);
       float distance = length(center_1 - pin.position.xy);
       if (distance < radius -20)
       {
           return spriteTexture.Sample(spriteSampler, pin.texcoord) * pin.color;
       }
       else if (distance < radius-10)
       {
           float4 originalColor = spriteTexture.Sample(spriteSampler, pin.texcoord) * pin.color;
           return lerp(originalColor, float4(0, 0, 0, 0.5f), 0.8f); // 80%黒寄り
       }
       else if (distance < radius)
       {
           float2 mater_vec = { pin.position.xy - center_1 };
           float angle = atan2(-mater_vec.x,-mater_vec.y);
           if (angle < 0)
               angle += 2.0f * 3.14159265f;
         
           float standard_angle = parametar * (3.14159265f / 180.0f);
         
           float4 metar_color = { 1, 1, 0, 1 };
           if (standard_angle >= angle)
           {
               return metar_color;
           }
           return float4(metar_color.rgb, 0.0f);
       }


       //float standard_angle = parametar * (3.14159265f / 180.0f);

       //float4 metar_color = { 0.84567625, 0.84567625, 0.84567625, 1 };
       //if (standard_angle >= angle)
       //{
       //    return metar_color;
       //}
       //return float4(metar_color.rgb, 0.0f);
   }
   else
   {
       float4 color = { 0, 0, 0, 0 };
       return color;
   }
    
    return spriteTexture.Sample(spriteSampler, pin.texcoord) * pin.color;

}
