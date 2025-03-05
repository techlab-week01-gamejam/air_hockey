#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include <windows.h>
#include <ctime>

// D3D 사용에 필요한 라이브러리들을 링크합니다.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D 사용에 필요한 헤더파일들을 포함합니다.
#include <d3d11.h>
#include <d3dcompiler.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"


#include "UI/UIManager.h"
#include "GameManager.h"
bool bUseGravity = false; // 중력 적용 여부 (기본 OFF)
float Gravity = -0.001f; // 중력 가속도 값 (음수 값: 아래 방향)
float leftHole = 0.2f;
float rightHole = 0.2f;
int scoreA = 0;
int scoreB = 0;
char isGoal;

// 1. Define the triangle vertices
struct FVertexSimple
{
    float x, y, z;    // Position
    float r, g, b, a; // Color
    float u,v;
};

// 벽에 사용할 정육면체
FVertexSimple cube_vertices[] =
{
    // Front face (Z+)
    { -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f},
    {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 8.0f },
                                                  
    // Back face (Z-)                             
    { -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    {  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    {  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 8.0f },
                                                  
    // Left face (X-)                             
    { -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    { -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    { -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 8.0f },
                                                  
    // Right face (X+)                            
    {  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 8.0f },
                                                  
    // Top face (Y+)                              
    { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 8.0f },
                                                  
    // Bottom face (Y-)                           
    { -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    { -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
    {  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    { -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 8.0f },
    {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 0.0f },
    {  0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 8.0f },
};


// Structure for a 3D vector
struct FVector3
{
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    // 벡터 덧셈 연산자
    FVector3 operator+(const FVector3& Other) const
    {
        return FVector3(x + Other.x, y + Other.y, z + Other.z);
    }

    // 벡터 뺄셈 연산자
    FVector3 operator-(const FVector3& Other) const
    {
        return FVector3(x - Other.x, y - Other.y, z - Other.z);
    }

    // 벡터 스칼라 곱 연산자 (오른쪽 스칼라 곱)
    FVector3 operator*(float Scalar) const
    {
        return FVector3(x * Scalar, y * Scalar, z * Scalar);
    }

    // 벡터 스칼라 나누기 연산자
    FVector3 operator/(float Scalar) const
    {
        return FVector3(x / Scalar, y / Scalar, z / Scalar);
    }

    // 벡터 덧셈 후 대입 연산자
    FVector3& operator+=(const FVector3& Other)
    {
        x += Other.x;
        y += Other.y;
        z += Other.z;
        return *this;
    }

    // 벡터 뺄셈 후 대입 연산자
    FVector3& operator-=(const FVector3& Other)
    {
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;
        return *this;
    }

    // 벡터 스칼라 곱 후 대입 연산자
    FVector3& operator*=(float Scalar)
    {
        x *= Scalar;
        y *= Scalar;
        z *= Scalar;
        return *this;
    }

    // 벡터 스칼라 나누기 후 대입 연산자
    FVector3& operator/=(float Scalar)
    {
        x /= Scalar;
        y /= Scalar;
        z /= Scalar;
        return *this;
    }

    float Length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }
    float LengthSquared() const
    {
        return (x * x + y * y + z * z);
    }
    FVector3 Normalized() const
    {
        float length = Length();
        if (length == 0)
        {
            return FVector3(0, 0, 0);
        }
        return FVector3(x / length, y / length, z / length);
    }
    float Dot(const FVector3& Other) const
    {
        return x * Other.x + y * Other.y + z * Other.z;
    }
};

#include "Sphere.h"

class URenderer
{
public:
    // Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
    ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
    ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
    IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인

    // 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
    ID3D11Texture2D* FrameBuffer = nullptr; // 화면 출력용 텍스처
    ID3D11RenderTargetView* FrameBufferRTV = nullptr; // 텍스처를 렌더 타겟으로 사용하는 뷰
    ID3D11RasterizerState* RasterizerState = nullptr; // 래스터라이저 상태(컬링, 채우기 모드 등 정의)
    ID3D11Buffer* ConstantBuffer = nullptr; // 쉐이더에 데이터를 전달하기 위한 상수 버퍼

    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상 (RGBA)
    D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

    ID3D11VertexShader* SimpleVertexShader;
    ID3D11PixelShader* SimplePixelShader;
    ID3D11InputLayout* SimpleInputLayout;
    unsigned int Stride;

    ID3D11SamplerState* SimpleSamplerState = nullptr;
    
    ID3D11ShaderResourceView* PrimaryTexture = nullptr;

    ID3D11ShaderResourceView* CorkTexture = nullptr;
    ID3D11ShaderResourceView* BallTexture = nullptr;
    ID3D11ShaderResourceView* WallTexture = nullptr;
    ID3D11ShaderResourceView* HoleTexture = nullptr;

public:
    // 렌더러 초기화 함수
    void Create(HWND hWindow)
    {
        // Direct3D 장치 및 스왑 체인 생성
        CreateDeviceAndSwapChain(hWindow);

        // 프레임 버퍼 생성
        CreateFrameBuffer();

        // 래스터라이저 상태 생성
        CreateRasterizerState();

        // 깊이 스텐실 버퍼 및 블렌드 상태는 이 코드에서는 다루지 않음

        // 샘플러 생성
        CreateSamplerState();
        
        TextureLoader::Get().g_pd3dDevice = Device;

        // Texture Preload
        TextureLoader::Get().LoadTextureFromFile("./textures/sample.jpg", &PrimaryTexture, "sample"); // 기본
        TextureLoader::Get().LoadTextureFromFile("./textures/cork.jpg", &CorkTexture, "cork"); // 콕
        TextureLoader::Get().LoadTextureFromFile("./textures/ball.jpg", &BallTexture, "ball"); // 볼
        TextureLoader::Get().LoadTextureFromFile("./textures/wall.jpg", &WallTexture, "wall"); // 벽
        TextureLoader::Get().LoadTextureFromFile("./textures/hole.jpg", &HoleTexture, "hole"); // 홀
    }

    void CreateSamplerState()
    {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        Device->CreateSamplerState(&samplerDesc, &SimpleSamplerState);
    }

    // Direct3D 장치 및 스왑 체인을 생성하는 함수
    void CreateDeviceAndSwapChain(HWND hWindow)
    {
        // 지원하는 Direct3D 기능 레벨을 정의
        D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

        // 스왑 체인 설정 구조체 초기화
        DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
        swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
        swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
        swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
        swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
        swapchaindesc.BufferCount = 2; // 더블 버퍼링
        swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
        swapchaindesc.Windowed = TRUE; // 창 모드
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 방식

        // Direct3D 장치와 스왑 체인을 생성
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
            featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
            &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

        // 생성된 스왑 체인의 정보 가져오기
        SwapChain->GetDesc(&swapchaindesc);

        // 뷰포트 정보 설정
        ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
    }

    // Direct3D 장치 및 스왑 체인을 해제하는 함수
    void ReleaseDeviceAndSwapChain()
    {
        if (DeviceContext)
        {
            DeviceContext->Flush(); // 남아있는 GPU 명령 실행
        }

        if (SwapChain)
        {
            SwapChain->Release();
            SwapChain = nullptr;
        }

        if (Device)
        {
            Device->Release();
            Device = nullptr;
        }

        if (DeviceContext)
        {
            DeviceContext->Release();
            DeviceContext = nullptr;
        }
    }

    // 프레임 버퍼를 생성하는 함수
    void CreateFrameBuffer()
    {
        // 스왑 체인으로부터 백 버퍼 텍스처 가져오기
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

        // 렌더 타겟 뷰 생성
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

        Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
    }

    // 프레임 버퍼를 해제하는 함수
    void ReleaseFrameBuffer()
    {
        if (FrameBuffer)
        {
            FrameBuffer->Release();
            FrameBuffer = nullptr;
        }

        if (FrameBufferRTV)
        {
            FrameBufferRTV->Release();
            FrameBufferRTV = nullptr;
        }
    }

    // 래스터라이저 상태를 생성하는 함수
    void CreateRasterizerState()
    {
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
        rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

        Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
    }

    // 래스터라이저 상태를 해제하는 함수
    void ReleaseRasterizerState()
    {
        if (RasterizerState)
        {
            RasterizerState->Release();
            RasterizerState = nullptr;
        }
    }

    // 렌더러에 사용된 모든 리소스를 해제하는 함수
    void Release()
    {
        RasterizerState->Release();

        // 렌더 타겟을 초기화
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        ReleaseFrameBuffer();
        ReleaseDeviceAndSwapChain();
    }

    // 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
    void SwapBuffer()
    {
        SwapChain->Present(1, 0); // 1: VSync 활성화
    }

    void CreateShader()
    {
        ID3DBlob* vertexshaderCSO;
        ID3DBlob* pixelshaderCSO;

        D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

        Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

        D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

        Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

        Stride = sizeof(FVertexSimple);

        vertexshaderCSO->Release();
        pixelshaderCSO->Release();
    }

    void ReleaseShader()
    {
        if (SimpleInputLayout)
        {
            SimpleInputLayout->Release();
            SimpleInputLayout = nullptr;
        }

        if (SimplePixelShader)
        {
            SimplePixelShader->Release();
            SimplePixelShader = nullptr;
        }

        if (SimpleVertexShader)
        {
            SimpleVertexShader->Release();
            SimpleVertexShader = nullptr;
        }
    }

    void Prepare()
    {
        DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

        DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        DeviceContext->RSSetViewports(1, &ViewportInfo);
        DeviceContext->RSSetState(RasterizerState);

        DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
        DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    }

    void PrepareShader()
    {
        DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
        DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);

        // 버텍스 쉐이더에 상수 버퍼를 설정합니다.
        if (ConstantBuffer)
        {
            DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
        }

        // 텍스처와 샘플러를 픽셀 쉐이더에 바인딩 (register t0, s0에 대응)
        DeviceContext->PSSetShaderResources(0, 1, &PrimaryTexture);
        DeviceContext->PSSetSamplers(0, 1, &SimpleSamplerState);
    }

    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
    {
        UINT offset = 0;
        DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

        DeviceContext->Draw(numVertices, 0);
    }

    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth)
    {
        // 2. Create a vertex buffer
        D3D11_BUFFER_DESC vertexbufferdesc = {};
        vertexbufferdesc.ByteWidth = byteWidth;
        vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated 
        vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

        ID3D11Buffer* vertexBuffer;

        Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

        return vertexBuffer;
    }

    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer)
    {
        vertexBuffer->Release();
    }

    // 상수 버퍼 생성, 소멸 함수 추가
    struct FConstants
    {
        FVector3 Offset;
        float Scale; // 공의 크기(반지름)
    };

    void CreateConstantBuffer()
    {
        D3D11_BUFFER_DESC constantbufferdesc = {};
        constantbufferdesc.ByteWidth = (sizeof(FConstants) + 0xf) & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
    }

    void ReleaseConstantBuffer()
    {
        if (ConstantBuffer)
        {
            ConstantBuffer->Release();
            ConstantBuffer = nullptr;
        }
    }

    // 상수 버퍼를 갱신하는 함수
    void UpdateConstant(FVector3 Offset, float Scale)
    {
        if (ConstantBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

            DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
            FConstants* constants = (FConstants*)constantbufferMSR.pData;
            {
                constants->Offset = Offset;
                constants->Scale = Scale; // 공의 크기 전달
            }
            DeviceContext->Unmap(ConstantBuffer, 0);
        }
    }
};

// 랜덤 실수 생성
float RandomFloat(float Min, float Max)
{
    return Min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (Max - Min);
}

class UBall
{
public:
    FVector3 Location;  // 공의 위치
    FVector3 Velocity;  // 공의 속도
    float Radius;       // 공의 반지름
    float Mass;         // 공의 질량
    int PlayerFlag;     // 마지막으로 공격한 플레이어

    UBall(FVector3 Location, FVector3 Velocity, float Radius) : Location(Location), Velocity(Velocity), Radius(Radius)
    {
        Mass = Radius * 10.0f; // 크기에 비례하는 질량
    }

    void SetPlayerFlag(int playerFlag)
    {
        PlayerFlag = playerFlag;
    }

    // 공을 이동시키는 함수
    void Move()
    {
        Location.x += Velocity.x;
        Location.y += Velocity.y;
        if (Velocity.x > 0.001f)
            Velocity.x -= 0.0004f;
        else if (Velocity.x < -0.001f)
            Velocity.x += 0.0001f;
        if (Velocity.y > 0.001f)
            Velocity.y -= 0.0004f;
        else if (Velocity.y < -0.001f)
            Velocity.y += 0.0001f;
    }

    // 벽과의 충돌 처리
    char CheckWallCollision()
    {
        const float leftBorder = -0.95f + Radius;
        const float rightBorder = 0.95f - Radius;
        const float topBorder = 0.475f - Radius;
        const float bottomBorder = -0.475f + Radius;

        if (Location.x < leftBorder)
        {
            if (leftHole > Location.y && Location.y > -(leftHole))
            {
                Velocity = (0.0f);
                scoreB++;
                GameManager::Get().AddScore(EPlayer::Player2);
                return 'B';
            }
            Location.x = leftBorder;  // 위치 보정
            Velocity.x *= -0.95f;      // 반사
        }
        else if (Location.x > rightBorder)
        {
            if (rightHole > Location.y && Location.y > -(rightHole))
            {
                Velocity = (0.0f);
                scoreA++;
                GameManager::Get().AddScore(EPlayer::Player1);
                return 'A';
            }
            Location.x = rightBorder;
            Velocity.x *= -0.95f;
        }

        if (Location.y > topBorder)
        {
            Location.y = topBorder;
            Velocity.y *= -0.95f;
        }
        else if (Location.y < bottomBorder)
        {
            Location.y = bottomBorder;
            Velocity.y *= -0.95f; // 중력이 없을 때는 완전 반사
        }
    }

    // 두 공 사이의 충돌 처리
    void ResolveCollision(UBall& Other)
    {
        FVector3 Diff = Location - Other.Location;
        float Distance = sqrt(Diff.x * Diff.x + Diff.y * Diff.y);
        float MinDist = Radius + Other.Radius;

        if (Distance < MinDist) // 충돌 발생
        {
            // 충돌 후 속도 교환 (탄성 충돌 공식 적용)
            FVector3 Normal = Diff / Distance;
            FVector3 RelativeVelocity = Velocity - Other.Velocity;
            float Speed = (RelativeVelocity.x * Normal.x + RelativeVelocity.y * Normal.y);

            if (Speed > 0) return; // 이미 멀어지고 있다면 처리하지 않음

            float Impulse = 2.0f * Speed / (Mass + Other.Mass);
            Velocity -= Normal * Impulse * Other.Mass;
            Other.Velocity += Normal * Impulse * Mass;

            // 위치 보정: 충돌한 공을 밀어서 겹치지 않도록 함
            float Overlap = MinDist - Distance;
            FVector3 Correction = Normal * (Overlap / 2.0f);

            Location += Correction;
            Other.Location -= Correction;
        }
    }

    void Render(URenderer* renderer, ID3D11Buffer* vertexBuffer)
    {
        if (nullptr == vertexBuffer)
            return;

        renderer->DeviceContext->PSSetShaderResources(0, 1, &renderer->BallTexture);
        renderer->UpdateConstant(Location, Radius);
        renderer->RenderPrimitive(vertexBuffer, sizeof(sphere_vertices) / sizeof(FVertexSimple));
    }
};

class UCork
{
public:
    FVector3 Location;  // 콕의 위치
    FVector3 Velocity;  // 콕의 속도
    float Radius;       // 콕의 반지름
    float Mass;         // 콕의 질량
    int PlayerFlag;     // 콕이 어떤 플레이어인지 나타내는 플래그
    ID3D11Buffer* VertexBuffer;  // 버텍스 버퍼
    URenderer* Renderer;
    ManagedTexture Texture;

    UCork(FVector3 Location, float Radius, float Mass, int PlayerFlag, URenderer* renderer) : Location(Location), Radius(Radius), Mass(Mass), PlayerFlag(PlayerFlag)
    {
        Renderer = renderer;
        VertexBuffer = Renderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
        Texture = TextureLoader::Get().GetTexture("sample2");
    }

    void SetLocationY(float deltaY)
    {
        Location.y += deltaY;
    }

    void SetVelocityY(float deltaY)
    {
        Velocity.y = deltaY;
    }

    void SetLocationX(float deltaX)
    {
        Location.x += deltaX;
    }

    void SetVelocityX(float deltaX)
    {
        Velocity.x = deltaX;
    }

    void SetInit(FVector3 location)
    {
        Location = location;
        Velocity = (0.0f);
    }

    void Render()
    {
        if (nullptr == VertexBuffer)
            return;

        Renderer->DeviceContext->PSSetShaderResources(0, 1, &Renderer->CorkTexture);
        Renderer->UpdateConstant(Location, Radius);
        Renderer->RenderPrimitive(VertexBuffer, sizeof(sphere_vertices) / sizeof(FVertexSimple));
    }

    // Cork와 Ball 사이의 충돌 처리
    void ResolveCollision(UBall& Other)
    {
        FVector3 Diff = Location - Other.Location;
        float Distance = sqrt(Diff.x * Diff.x + Diff.y * Diff.y);
        float MinDist = Radius + Other.Radius;

        if (Distance < MinDist) // 충돌 발생
        {
            FVector3 Normal = (Distance > 1e-6f) ? (Diff / Distance) : FVector3(0, 0, 0);
            FVector3 RelativeVelocity = Velocity - Other.Velocity;
            float Speed = (RelativeVelocity.x * Normal.x + RelativeVelocity.y * Normal.y);

            // 겹쳐 있을 때도 충돌 처리를 강제 실행
            if (Speed > 0 && Distance >= MinDist) return;

            float Impulse = 2.0f * Speed / (Mass + Other.Mass);
            Other.Velocity += Normal * Impulse * Mass;

            // 위치 보정 추가 (공이 너무 겹치는 문제 방지)
            float Overlap = MinDist - Distance;
            FVector3 Correction = Normal * (Overlap / 2.0f);

            Other.Location -= Correction;

            // 공에게 마지막으로 공격한 플레이어가 누구인지 전달
            Other.SetPlayerFlag(PlayerFlag);

        }
    }
};

enum class EItem
{
    TwoBalls,
    Slow,
    MultipleGoals,
    Stop,
    TotalItemCount
};

EItem GetRandomItem()
{
    return static_cast<EItem>(rand() % static_cast<int>(EItem::TotalItemCount));
}

class UItem
{
public:
    FVector3 Location;
    EItem ItemType;

    UItem(FVector3 Location, EItem ItemType) : Location(Location), ItemType(ItemType) {}

    void Render(URenderer* renderer, ID3D11Buffer* vertexBuffer)
    {
        if (nullptr == vertexBuffer)
            return;

        renderer->UpdateConstant(Location, 0.04f);
        renderer->RenderPrimitive(vertexBuffer, sizeof(sphere_vertices) / sizeof(FVertexSimple));
    }

    // Item와 Ball 사이의 충돌 여부
    bool IsCollisionWithItem(UBall& Other)
    {
        FVector3 Diff = Location - Other.Location;
        float Distance = sqrt(Diff.x * Diff.x + Diff.y * Diff.y);
        float MinDist = 0.05f + Other.Radius;

        if (Distance < MinDist) // 아이템과 공이 충돌하면
        {
            return true;
        }

        return false;
    }
};

class UItemManager
{
public:
    UItem** ItemList;
    int ItemCount;
    int Capacity;      // 배열의 최대 크기 (미리 할당된 공간)
    URenderer* Renderer; // 렌더러 참조
    ID3D11Buffer* VertexBuffer;  // 버텍스 버퍼
    bool bMultipleBalls; // 공이 두개 있는지 여부

    UItemManager(URenderer* renderer)
    {
        ItemList = nullptr;
        ItemCount = 0;
        Capacity = 2;
        Renderer = renderer;
        VertexBuffer = nullptr;

        ItemList = new UItem * [Capacity];

        InitializeVertexBuffer();
    }

    void setbMultipleBalls(bool bMultiple)
    {
        bMultipleBalls = bMultiple;
    };

    // 새로운 게임 시작시 호출
    void SetUpNewGame()
    {
        // 기존 아이템들을 모두 삭제
        for (int i = 0; i < ItemCount; i++)
        {
            delete ItemList[i];
        }
        // 기존 배열 메모리 해제
        delete[] ItemList;

        // 아이템 개수를 초기화
        ItemCount = 0;

        // Capacity에 맞춰 새로운 아이템 배열 생성
        ItemList = new UItem * [Capacity];
    }

    void AddItem()
    {
        if (ItemCount <= Capacity)
        {
            // 아이템 랜덤 뽑기
            EItem newItem = GetRandomItem();
            // 공이 두개이면 더 이상 TwoBalls 아이템이 나오지 않음
            while (bMultipleBalls && newItem == EItem::TwoBalls)
            {
                newItem = GetRandomItem();
            }

            // 일시적
            if (bMultipleBalls)
            {
                return;
            }

            ItemList[ItemCount] = new UItem(FVector3(RandomFloat(-0.9f, 0.9f), RandomFloat(-0.4f, 0.4f), 0.0f), EItem::TwoBalls);
            ItemCount++;
        }
    }

    void RemoveItem(UItem* Item)
    {
        if (ItemCount == 0) return;

        for (int i = 0; i < ItemCount; i++)
        {
            if (ItemList[i] == Item)
            {
                delete ItemList[i];
                ItemList[i] = ItemList[ItemCount - 1];
                ItemCount--;
                return;
            }
        }

    }

    // 모든 아이템 렌더링
    void RenderItems()
    {
        for (int i = 0; i < ItemCount; i++)
        {
            ItemList[i]->Render(Renderer, VertexBuffer);
        }
    }

    // VertexBuffer 초기화
    void InitializeVertexBuffer()
    {
        if (nullptr == VertexBuffer)
        {
            VertexBuffer = Renderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
        }
    }

    // Vertex Buffer 해제 (프로그램 종료 시 실행)
    void ReleaseVertexBuffer()
    {
        if (VertexBuffer)
        {
            Renderer->ReleaseVertexBuffer(VertexBuffer);
            VertexBuffer = nullptr;
        }
    }

    // 소멸자 (모든 공 삭제)
    ~UItemManager()
    {
        for (int i = 0; i < ItemCount; i++)
        {
            delete ItemList[i];
        }
        delete[] ItemList;
        ReleaseVertexBuffer();
    }
};

class UBallManager
{
public:
    UBall** BallList; // 이중 포인터를 이용한 공 리스트
    int BallCount;    // 현재 공의 개수
    int Capacity;      // 배열의 최대 크기 (미리 할당된 공간)
    URenderer* Renderer; // 렌더러 참조
    ID3D11Buffer* VertexBuffer;  // 버텍스 버퍼
    int MaxSpeed = 0.05f;
    UItemManager* ItemManager;

    // 생성자
    UBallManager(URenderer* renderer, UItemManager* itemManager)
    {
        BallList = nullptr;
        BallCount = 0;
        Capacity = 15;
        Renderer = renderer;
        ItemManager = itemManager;
        VertexBuffer = nullptr;

        BallList = new UBall * [Capacity];


        // Vertex Buffer 생성
        InitializeVertexBuffer();
    }

    // 새로운 게임 시작 시 호출
    void SetupNewGame()
    {
        while (BallCount > 0)
        {
            RemoveBall();
        }
        
        AddBall(FVector3(0.0f, 0.0f, 0.0f));
    }

    // 공 추가 함수
    void AddBall(FVector3 Location)
    {
        if (BallCount >= Capacity)
        {
            // 배열 크기를 2배로 확장
            Capacity *= 2;
            UBall** NewList = new UBall * [Capacity];

            // 기존 데이터 복사
            memcpy(NewList, BallList, BallCount * sizeof(UBall*));

            delete[] BallList;
            BallList = NewList;
        }

        // 새 공 추가
        BallList[BallCount] = new UBall(Location, FVector3(RandomFloat(-0.03f, 0.03f), RandomFloat(-0.02f, 0.02f), 0.0f), 0.05f);
        BallCount++;
    }

    // 공 제거 (삭제할 요소를 마지막 요소와 교체)
    void RemoveBall()
    {
        if (BallCount == 0) return;

        int RemoveIndex = rand() % BallCount;

        // 마지막 공과 자리 바꿈
        delete BallList[RemoveIndex];
        BallList[RemoveIndex] = BallList[BallCount - 1];
        BallCount--;

        // 배열이 너무 크면 줄이기
        if (BallCount < Capacity / 4 && Capacity > 15)
        {
            Capacity /= 2;
            UBall** NewList = new UBall * [Capacity];

            memcpy(NewList, BallList, BallCount * sizeof(UBall*));

            delete[] BallList;
            BallList = NewList;
        }
    }

    // 모든 공 이동
    void UpdateBalls(UCork* CorkA, UCork* CorkB)
    {
        if (bUseGravity)
        {
            for (int i = 0; i < BallCount; i++)
            {
                float acceleration = Gravity / BallList[i]->Mass; // 중력 가속도 = Gravity / Mass
                BallList[i]->Velocity.y += acceleration; // 질량에 비례한 중력 가속도 적용
            }
        }

        for (int i = 0; i < BallCount; i++)
        {
            BallList[i]->Move();
            isGoal = BallList[i]->CheckWallCollision();
            if (isGoal == 'A' || isGoal == 'B')
            {
                while (BallCount > 0)
                    RemoveBall();
                AddBall(FVector3(0.0f));
                CorkA->SetInit(FVector3(-0.875f, 0.0f, 0.0f));
                CorkB->SetInit(FVector3(0.875f, 0.0f, 0.0f));
                break;
            }
            CorkA->ResolveCollision(*BallList[i]);
            CorkB->ResolveCollision(*BallList[i]);

            for (int j = 0; j < ItemManager->ItemCount; j++)
            {
                if (ItemManager->ItemList[j]->IsCollisionWithItem(*BallList[i]))
                {
                    if (ItemManager->ItemList[j]->ItemType == EItem::TwoBalls)
                    {
                        AddBall(ItemManager->ItemList[j]->Location);
                    }
                    ItemManager->RemoveItem(ItemManager->ItemList[j]);
                }
            }
        }

        // 공이 1개 이하라면 충돌 연산 불필요
        if (BallCount < 2)
        {
            ItemManager->setbMultipleBalls(false);
            return;
        }
        ItemManager->setbMultipleBalls(true);

        // 충돌 감지 및 처리 (모든 공 쌍 확인)
        for (int i = 0; i < BallCount; i++)
        {
            for (int j = i + 1; j < BallCount; j++)
            {
                BallList[i]->ResolveCollision(*BallList[j]);
            }
        }

    }

    // 모든 공 렌더링
    void RenderBalls()
    {
        for (int i = 0; i < BallCount; i++)
        {
            BallList[i]->Render(Renderer, VertexBuffer);
        }
    }

    // VertexBuffer 초기화
    void InitializeVertexBuffer()
    {
        if (nullptr == VertexBuffer)
        {
            VertexBuffer = Renderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
        }
    }

    // Vertex Buffer 해제 (프로그램 종료 시 실행)
    void ReleaseVertexBuffer()
    {
        if (VertexBuffer)
        {
            Renderer->ReleaseVertexBuffer(VertexBuffer);
            VertexBuffer = nullptr;
        }
    }

    // 소멸자 (모든 공 삭제)
    ~UBallManager()
    {
        for (int i = 0; i < BallCount; i++)
        {
            delete BallList[i];
        }
        delete[] BallList;
        ReleaseVertexBuffer();
    }
};



extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

DWORD WINAPI TimerThread(LPVOID lpParam)
{
    UItemManager* ItemManager = static_cast<UItemManager*>(lpParam);
    while (true)
    {
        if (ItemManager->ItemCount == 2)
        {
            Sleep(10000);
        }
        else
        {
            ItemManager->AddItem();
            Sleep(10000);
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    srand(static_cast<unsigned int>(time(nullptr))); // 현재 시간을 기반으로 시드 설정

    // 윈도우 클래스 이름
    WCHAR WindowClass[] = L"JungleWindowClass";

    // 윈도우 타이틀바에 표시될 이름
    WCHAR Title[] = L"Game Tech Lab";

    // 각종 메시지를 처리할 함수인 WndProc의 함수 포인터를 WindowClass 구조체에 넣는다.
    WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

    // 윈도우 클래스 등록
    RegisterClassW(&wndclass);

    // 1024 x 1024 크기에 윈도우 생성
    HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
        nullptr, nullptr, hInstance, nullptr);

    // Renderer Class를 생성합니다.
    URenderer	renderer;

    // D3D11 생성하는 함수를 호출합니다.
    renderer.Create(hWnd);
    // 렌더러 생성 직후에 쉐이더를 생성하는 함수를 호출합니다.
    renderer.CreateShader();
    renderer.CreateConstantBuffer();


    UItemManager ItemManager(&renderer);
    HANDLE hThread = CreateThread(NULL, 0, TimerThread, &ItemManager, 0, NULL);


    UBallManager BallManager(&renderer, &ItemManager);



    // 가로선 세로선 생성
    UINT numVerticeCube = sizeof(cube_vertices) / sizeof(FVertexSimple);
    FVertexSimple horizontal[36];
    FVertexSimple vertical[36];

    for (UINT i = 0; i < numVerticeCube; i++)
    {
        //가로선 스케일링
        horizontal[i] = cube_vertices[i];
        horizontal[i].x *= 2.0f;
        horizontal[i].y *= 0.05f;

        //세로선 스케일링
        vertical[i] = cube_vertices[i];
        vertical[i].x *= 0.1f;
        vertical[i].y *= 1.0f;
    }
    ID3D11Buffer* vertexBufferHorizontal = renderer.CreateVertexBuffer(horizontal, sizeof(horizontal));
    ID3D11Buffer* vertexBufferVertical = renderer.CreateVertexBuffer(vertical, sizeof(vertical));

    //가로선, 세로선 offset
    FVector3 offsetHorizontal(0.0f, 0.5f, 0.0f);
    FVector3 offsetVertical(-1.0f, 0.0f, 0.0f);

    // 골대 A, B
    FVertexSimple holeA[36];
    FVertexSimple holeB[36];

    for (UINT i = 0; i < numVerticeCube; i++)
    {
        //골대 스케일링
        holeA[i] = cube_vertices[i];
        holeA[i].x *= 0.1f;
        holeA[i].y *= leftHole * 2;
        holeA[i].r = 0.025f; holeA[i].g = 0.025f; holeA[i].b = 0.025f; holeA[i].a = 1.0f;

        holeB[i] = cube_vertices[i];
        holeB[i].x *= 0.1f;
        holeB[i].y *= rightHole * 2;
        holeB[i].r = 0.025f; holeB[i].g = 0.025f; holeB[i].b = 0.025f; holeB[i].a = 1.0f;
    }
    ID3D11Buffer* vertexBufferHoleA = renderer.CreateVertexBuffer(holeA, sizeof(holeA));
    ID3D11Buffer* vertexBufferHoleB = renderer.CreateVertexBuffer(holeB, sizeof(holeB));

    FVector3 offsetHoleA(-1.0f, 0.0f, 0.0f);
    FVector3 offsetHoleB(1.0f, 0.0f, 0.0f);

    bool bIsExit = false;

    // UI Manager
    UIManager* HUD = new UIManager();
    HUD->Initialize(renderer.Device, renderer.DeviceContext, hWnd);
    HUD->ReplaceUI(EUIState::MAIN);
    static bool bEscapeReady = true;

    // FPS 제한을 위한 설정
    const int targetFPS = 60;
    const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    // 고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;

    //플레이어A, B 이동속도
    float moveA = 0.03f;
    float moveB = 0.03f;

    // 플레이어 Cork
    UCork* CorkA = new UCork(FVector3(-0.875f, 0.0f, 0.0f), 0.07f, 35.0f, 1, &renderer);
    UCork* CorkB = new UCork(FVector3(0.875f, 0.0f, 0.0f), 0.07f, 35.0f, 2, &renderer);

    // A의 초기 위치 및 목표 위치 설정
    float initialXA = CorkA->Location.x; // A의 원래 위치
    float targetXA = CorkA->Location.x + 0.6f; // A가 이동할 목표 위치

    bool isMovingRightA = false;  // A가 오른쪽으로 이동 중인지
    bool isReturningA = false;    // A가 원래 위치로 돌아오는 중인지

    // B의 초기 위치 및 목표 위치 설정
    float initialXB = CorkB->Location.x; // B의 원래 위치
    float targetXB = CorkB->Location.x - 0.6f; // B가 이동할 목표 위치

    bool isMovingLeftB = false;  // B가 왼쪽으로 이동 중인지
    bool isReturningB = false;   // B가 원래 위치로 돌아오는 중인지

    // Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
    while (bIsExit == false)
    {
        // 루프 시작 시간 기록
        QueryPerformanceCounter(&startTime);

        MSG msg;

        // 처리할 메시지가 더 이상 없을때 까지 수행
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&msg);

            // 메시지를 적절한 윈도우 프로시저에 전달, 메시지가 위에서 등록한 WndProc 으로 전달됨
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        // Escape Key Press : InGame
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            if (bEscapeReady)
            {
                // Play Stop
                GameManager::Get().TogglePlaying();

                HUD->TogglePause();
                bEscapeReady = false;
            }
        }
        else
        {
            bEscapeReady = true;
        }

        #pragma region RenderSetup
        renderer.Prepare();
        renderer.PrepareShader();
        #pragma endregion

        #pragma region GameSetup
        if (GameManager::Get().ShouldStartNewGame())
        {
            // Global variables Setup
            GameManager::Get().SetUpNewGame();

            // Player A Setup
            CorkA->SetInit(FVector3(-0.875f, 0.0f, 0.0f));

            // Player B Setup
            CorkB->SetInit(FVector3(0.875f, 0.0f, 0.0f));

            // BallManager Setup
            BallManager.SetupNewGame();

            // ItemManager Setup
            ItemManager.SetUpNewGame();

            // HUD Setup
            HUD->ReplaceUI(EUIState::GAME);

            // Ready !
            GameManager::Get().ReadyForNewGame();
        }
        #pragma endregion

        #pragma region Logic
        if (GameManager::Get().IsPlaying() && HUD->GetCurrentState() == EUIState::GAME)
        {
            if (GetAsyncKeyState(0x57) & 0x8000 && CorkA->Location.y + moveA < 0.405f)
            {
                CorkA->SetLocationY(moveA);
                CorkA->SetVelocityY(moveA);
            }
            if (GetAsyncKeyState(0x53) & 0x8000 && CorkA->Location.y - moveA > -0.405f)
            {
                CorkA->SetLocationY(-moveA);
                CorkA->SetVelocityY(-moveA);
            }
            if (GetAsyncKeyState(VK_UP) & 0x8000 && CorkB->Location.y + moveB < 0.405f)
            {
                CorkB->SetLocationY(moveB);
                CorkB->SetVelocityY(moveB);
            }
            if (GetAsyncKeyState(VK_DOWN) & 0x8000 && CorkB->Location.y - moveB > -0.405f)
            {
                CorkB->SetLocationY(-moveB);
                CorkB->SetVelocityY(-moveB);
            }
            //왼쪽 컨트롤키
            if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 && !isMovingRightA && !isReturningA)
            {
                isMovingRightA = true;
            }
            //엔터키
            if (GetAsyncKeyState(VK_RETURN) & 0x8000 && !isMovingLeftB && !isReturningB)
            {
                isMovingLeftB = true;
            }

            // A가 오른쪽으로 이동 중이라면
            if (isMovingRightA)
            {
                if (CorkA->Location.x < targetXA)
                {
                    CorkA->SetLocationX(0.06f);
                    CorkA->SetVelocityX(0.06f);
                }
                else
                {
                    isMovingRightA = false;
                    isReturningA = true; // 원래 위치로 돌아가기 시작
                }
            }

            // A가 원래 위치로 돌아오는 중이라면
            if (isReturningA)
            {
                if (CorkA->Location.x > initialXA)
                {
                    CorkA->SetLocationX(-0.06f);
                    CorkA->SetVelocityX(-0.06f);
                }
                else
                {
                    isReturningA = false; // 복귀 완료
                }
            }

            // B가 왼쪽으로 이동 중이라면
            if (isMovingLeftB)
            {
                if (CorkB->Location.x > targetXB)
                {
                    CorkB->SetLocationX(-0.06f);
                    CorkB->SetVelocityX(-0.06f);
                }
                else
                {
                    isMovingLeftB = false;
                    isReturningB = true; // 원래 위치로 돌아가기 시작
                }
            }

            // B가 원래 위치로 돌아오는 중이라면
            if (isReturningB)
            {
                if (CorkB->Location.x < initialXB)
                {
                    CorkB->SetLocationX(0.06f);
                    CorkB->SetVelocityX(0.06f);
                }
                else
                {
                    isReturningB = false; // 복귀 완료
                }
            }

            BallManager.UpdateBalls(CorkA, CorkB);

            CorkA->SetVelocityY(0.0f);
            CorkB->SetVelocityY(0.0f);

            if (!isMovingRightA && !isReturningA)
                CorkA->SetVelocityX(0.0f);
            if (!isMovingLeftB && !isReturningB)
                CorkB->SetVelocityX(0.0f);
        }
        #pragma endregion

        #pragma region Rendering
        if (HUD->GetCurrentState() != EUIState::MAIN)
        {
            BallManager.RenderBalls();
            ItemManager.RenderItems();

            //가로벽 렌더링
            renderer.DeviceContext->PSSetShaderResources(0, 1, &renderer.WallTexture);
            renderer.UpdateConstant(offsetHorizontal, 1.0f);
            renderer.RenderPrimitive(vertexBufferHorizontal, numVerticeCube);
            offsetHorizontal.y *= -1.0f; //offset 변경
            renderer.UpdateConstant(offsetHorizontal, 1.0f);
            renderer.RenderPrimitive(vertexBufferHorizontal, numVerticeCube);

            //세로벽 렌더링
            renderer.UpdateConstant(offsetVertical, 1.0f);
            renderer.RenderPrimitive(vertexBufferVertical, numVerticeCube);
            offsetVertical.x *= -1.0f;  //offset 변경
            renderer.UpdateConstant(offsetVertical, 1.0f);
            renderer.RenderPrimitive(vertexBufferVertical, numVerticeCube);

            //홀 렌더링
            renderer.DeviceContext->PSSetShaderResources(0, 1, &renderer.HoleTexture);
            renderer.UpdateConstant(offsetHoleA, 1.0f);
            renderer.RenderPrimitive(vertexBufferHoleA, numVerticeCube);
            renderer.UpdateConstant(offsetHoleB, 1.0f);
            renderer.RenderPrimitive(vertexBufferHoleB, numVerticeCube);

            //플레이어A 렌더링
            CorkA->Render();

            //플레이어B 렌더링
            CorkB->Render();

            
        }
        #pragma endregion

        // 최종 UI를 업데이트 합니다.
        HUD->Update();

        // 현재 화면에 보여지는 버퍼와 그리기 작업을 위한 버퍼를 서로 교환합니다.
        renderer.SwapBuffer();

        do
        {
            Sleep(0);

            // 루프 종료 시간 기록
            QueryPerformanceCounter(&endTime);

            // 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

        } while (elapsedTime < targetFrameTime);
    }

    // ReleaseShader() 직전에 소멸 함수를 추가합니다.
    renderer.ReleaseConstantBuffer();
    renderer.ReleaseVertexBuffer(vertexBufferHorizontal);
    renderer.ReleaseVertexBuffer(vertexBufferVertical);
    renderer.ReleaseVertexBuffer(vertexBufferHoleA);
    renderer.ReleaseVertexBuffer(vertexBufferHoleB);
    renderer.ReleaseShader();
    renderer.Release();

    return 0;
}