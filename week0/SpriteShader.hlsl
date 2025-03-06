Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

cbuffer SpriteData : register(b0)
{
    float2 UVOffset; // UV ������ġ
    float2 UVSize; // UV ũ��
    float2 Position; // ��������Ʈ ��ġ ����
    float Scale; // ��������Ʈ ũ��
    float padding; // �е�
}

struct VS_INPUT
{
    float3 position : POSITION; // ���� ��ġ
    float2 uv : TEXCOORD0; // UV ��ǥ
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
    // ��ġ ���� ����
    output.position.xy += Position;
    
    output.uv = input.uv * UVSize + UVOffset; // UV��ǥ ����
    
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return tex.Sample(samLinear, input.uv);
}