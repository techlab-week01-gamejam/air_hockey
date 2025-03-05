cbuffer constants : register(b0)
{
    float3 Offset;
    float Scale;
};

struct VS_INPUT
{
    float4 position : POSITION; // ���ؽ� ��ġ
    float4 color : COLOR; // ���ؽ� ����
    float2 texcoord : TEXCOORD0; // ���ؽ� �ؽ�ó ��ǥ
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Ŭ�� ���� ��ġ
    float4 color : COLOR; // ����
    float2 texcoord : TEXCOORD0; // �ؽ�ó ��ǥ
};

Texture2D myTexture : register(t0); // �ؽ�ó ���ҽ�
SamplerState mySampler : register(s0); // �ؽ�ó ���÷�

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    // ��� ���۸� ���� ���� Offset�� Scale�� ����
    output.position = float4(Offset + input.position.xyz * Scale, 1.0f);
    output.color = input.color;
    output.texcoord = input.texcoord;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // �ؽ�ó ���ø�
    float4 texColor = myTexture.Sample(mySampler, input.texcoord);
    // ���ؽ� ����� �ؽ�ó ������ ���Ͽ� ���� ���� ����
    return texColor;
}