cbuffer constants : register(b0)
{
    float3 Offset;
    float Scale;
};

struct VS_INPUT
{
    float4 position : POSITION; // 버텍스 위치
    float4 color : COLOR; // 버텍스 색상
    float2 texcoord : TEXCOORD0; // 버텍스 텍스처 좌표
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 클립 공간 위치
    float4 color : COLOR; // 색상
    float2 texcoord : TEXCOORD0; // 텍스처 좌표
};

Texture2D myTexture : register(t0); // 텍스처 리소스
SamplerState mySampler : register(s0); // 텍스처 샘플러

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    // 상수 버퍼를 통해 받은 Offset과 Scale을 적용
    output.position = float4(Offset + input.position.xyz * Scale, 1.0f);
    output.color = input.color;
    output.texcoord = input.texcoord;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // 텍스처 샘플링
    float4 texColor = myTexture.Sample(mySampler, input.texcoord);
    // 버텍스 색상과 텍스처 색상을 곱하여 최종 색상 산출
    return texColor;
}