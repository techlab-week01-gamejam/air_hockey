Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

cbuffer SpriteData : register(b0)
{
    float2 UVOffset; // UV 시작위치
    float2 UVSize; // UV 크기
    float2 Position; // 스프라이트 위치 정보
    float Scale; // 스프라이트 크기
    float padding; // 패딩
}

struct VS_INPUT
{
    float3 position : POSITION; // 정점 위치
    float2 uv : TEXCOORD0; // UV 좌표
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = float4(input.position * Scale, 1.0f);
    // 위치 정보 적용
    output.position.xy += Position;
    
    output.uv = input.uv * UVSize + UVOffset; // UV좌표 변경
    
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return tex.Sample(samLinear, input.uv);
}