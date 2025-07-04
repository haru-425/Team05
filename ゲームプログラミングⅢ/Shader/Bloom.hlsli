// BLOOM
cbuffer BLOOM_CONSTANT_BUFFER : register(b12)
{
    float bloom_extraction_threshold;
    float bloom_intensity;
    float2 something;
};