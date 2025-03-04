// ShaderW0.hlsl
cbuffer constants : register(b0)
{
    float3 Offset;
    float Scale;
}

struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // Pass the position directly to the pixel shader (no transformation)
    //output.position = input.position;
    
    // ������۸� ���� �Ѱ� ���� Offset�� ���ؼ� ���ؽ��� �̵� ���� �ȼ����̴��� �ѱ�
    output.position = float4(Offset + input.position.xyz * Scale , 1.0f);
    
    // Pass the color to the pixel shader
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}
