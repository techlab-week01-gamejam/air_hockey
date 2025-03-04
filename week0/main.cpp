#include <windows.h>

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

bool bUseGravity = false; // 중력 적용 여부 (기본 OFF)
float Gravity = -0.001f; // 중력 가속도 값 (음수 값: 아래 방향)

// 1. Define the triangle vertices
struct FVertexSimple {
    float x, y, z;    // Position
    float r, g, b, a; // Color
};

// Structure for a 3D vector
struct FVector3 {
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    // 벡터 덧셈 연산자
    FVector3 operator+(const FVector3& Other) const {
        return FVector3(x + Other.x, y + Other.y, z + Other.z);
    }

    // 벡터 뺄셈 연산자
    FVector3 operator-(const FVector3& Other) const {
        return FVector3(x - Other.x, y - Other.y, z - Other.z);
    }

    // 벡터 스칼라 곱 연산자 (오른쪽 스칼라 곱)
    FVector3 operator*(float Scalar) const {
        return FVector3(x * Scalar, y * Scalar, z * Scalar);
    }

    // 벡터 스칼라 나누기 연산자
    FVector3 operator/(float Scalar) const {
        return FVector3(x / Scalar, y / Scalar, z / Scalar);
    }

    // 벡터 덧셈 후 대입 연산자
    FVector3& operator+=(const FVector3& Other) {
        x += Other.x;
        y += Other.y;
        z += Other.z;
        return *this;
    }

    // 벡터 뺄셈 후 대입 연산자
    FVector3& operator-=(const FVector3& Other) {
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;
        return *this;
    }

    // 벡터 스칼라 곱 후 대입 연산자
    FVector3& operator*=(float Scalar) {
        x *= Scalar;
        y *= Scalar;
        z *= Scalar;
        return *this;
    }

    // 벡터 스칼라 나누기 후 대입 연산자
    FVector3& operator/=(float Scalar) {
        x /= Scalar;
        y /= Scalar;
        z /= Scalar;
        return *this;
    }
};

#include "Sphere.h"

class URenderer {
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


public:
    // 렌더러 초기화 함수
    void Create(HWND hWindow) {
        // Direct3D 장치 및 스왑 체인 생성
        CreateDeviceAndSwapChain(hWindow);

        // 프레임 버퍼 생성
        CreateFrameBuffer();

        // 래스터라이저 상태 생성
        CreateRasterizerState();

        // 깊이 스텐실 버퍼 및 블렌드 상태는 이 코드에서는 다루지 않음
    }

    // Direct3D 장치 및 스왑 체인을 생성하는 함수
    void CreateDeviceAndSwapChain(HWND hWindow) {
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
    void ReleaseDeviceAndSwapChain() {
        if (DeviceContext) {
            DeviceContext->Flush(); // 남아있는 GPU 명령 실행
        }

        if (SwapChain) {
            SwapChain->Release();
            SwapChain = nullptr;
        }

        if (Device) {
            Device->Release();
            Device = nullptr;
        }

        if (DeviceContext) {
            DeviceContext->Release();
            DeviceContext = nullptr;
        }
    }

    // 프레임 버퍼를 생성하는 함수
    void CreateFrameBuffer() {
        // 스왑 체인으로부터 백 버퍼 텍스처 가져오기
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

        // 렌더 타겟 뷰 생성
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // 색상 포맷
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

        Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
    }

    // 프레임 버퍼를 해제하는 함수
    void ReleaseFrameBuffer() {
        if (FrameBuffer) {
            FrameBuffer->Release();
            FrameBuffer = nullptr;
        }

        if (FrameBufferRTV) {
            FrameBufferRTV->Release();
            FrameBufferRTV = nullptr;
        }
    }

    // 래스터라이저 상태를 생성하는 함수
    void CreateRasterizerState() {
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
        rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

        Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
    }

    // 래스터라이저 상태를 해제하는 함수
    void ReleaseRasterizerState() {
        if (RasterizerState) {
            RasterizerState->Release();
            RasterizerState = nullptr;
        }
    }

    // 렌더러에 사용된 모든 리소스를 해제하는 함수
    void Release() {
        RasterizerState->Release();

        // 렌더 타겟을 초기화
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        ReleaseFrameBuffer();
        ReleaseDeviceAndSwapChain();
    }

    // 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
    void SwapBuffer() {
        SwapChain->Present(1, 0); // 1: VSync 활성화
    }

    void CreateShader() {
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
        };

        Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

        Stride = sizeof(FVertexSimple);

        vertexshaderCSO->Release();
        pixelshaderCSO->Release();
    }

    void ReleaseShader() {
        if (SimpleInputLayout) {
            SimpleInputLayout->Release();
            SimpleInputLayout = nullptr;
        }

        if (SimplePixelShader) {
            SimplePixelShader->Release();
            SimplePixelShader = nullptr;
        }

        if (SimpleVertexShader) {
            SimpleVertexShader->Release();
            SimpleVertexShader = nullptr;
        }
    }

    void Prepare() {
        DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

        DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        DeviceContext->RSSetViewports(1, &ViewportInfo);
        DeviceContext->RSSetState(RasterizerState);

        DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, nullptr);
        DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    }

    void PrepareShader() {
        DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
        DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);

        // 버텍스 쉐이더에 상수 버퍼를 설정합니다.
        if (ConstantBuffer) {
            DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
        }
    }

    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices) {
        UINT offset = 0;
        DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

        DeviceContext->Draw(numVertices, 0);
    }

    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth) {
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

    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer) {
        vertexBuffer->Release();
    }

    // 상수 버퍼 생성, 소멸 함수 추가
    struct FConstants {
        FVector3 Offset;
        float Scale; // 공의 크기(반지름)
    };

    void CreateConstantBuffer() {
        D3D11_BUFFER_DESC constantbufferdesc = {};
        constantbufferdesc.ByteWidth = (sizeof(FConstants) + 0xf) & 0xfffffff0; // ensure constant buffer size is multiple of 16 bytes
        constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
        constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
    }

    void ReleaseConstantBuffer() {
        if (ConstantBuffer) {
            ConstantBuffer->Release();
            ConstantBuffer = nullptr;
        }
    }

    // 상수 버퍼를 갱신하는 함수
    void UpdateConstant(FVector3 Offset, float Scale) {
        if (ConstantBuffer) {
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

class UBall {
public:
    FVector3 Location;  // 공의 위치
    FVector3 Velocity;  // 공의 속도
    float Radius;       // 공의 반지름
    float Mass;         // 공의 질량

    // 생성자: 임의의 크기, 위치, 속도를 가진 공 생성
    UBall() {
        Radius = RandomFloat(0.05f, 0.15f); // 0.05~0.15 사이의 임의 크기
        Mass = Radius * 10.0f; // 크기에 비례하는 질량
        Location = FVector3(RandomFloat(-0.8f, 0.8f), RandomFloat(-0.8f, 0.8f), 0.0f);
        Velocity = FVector3(RandomFloat(-0.02f, 0.02f), RandomFloat(-0.02f, 0.02f), 0.0f);
    }

    // 공을 이동시키는 함수
    void Move() {
        Location.x += Velocity.x;
        Location.y += Velocity.y;
    }

    // 벽과의 충돌 처리
    void CheckWallCollision() {
        const float leftBorder = -1.0f + Radius;
        const float rightBorder = 1.0f - Radius;
        const float topBorder = 1.0f - Radius;
        const float bottomBorder = -1.0f + Radius;

        if (Location.x < leftBorder) {
            Location.x = leftBorder;  // 위치 보정
            Velocity.x *= -1.0f;      // 반사
        } else if (Location.x > rightBorder) {
            Location.x = rightBorder;
            Velocity.x *= -1.0f;
        }

        if (Location.y > topBorder) { 
            Location.y = topBorder;
            Velocity.y *= -1.0f;
        } else if (Location.y < bottomBorder) { 
            Location.y = bottomBorder;

            if (bUseGravity) {
                Velocity.y *= -0.8f; // 바닥 충돌 시 탄성 손실
                if (fabs(Velocity.y) < 0.01f) { // 매우 작은 값이면 멈춤
                    Velocity.y = 0.0f;
                }
            } else {
                Velocity.y *= -1.0f; // 중력이 없을 때는 완전 반사
            }
        }
    }

    // 두 공 사이의 충돌 처리
    void ResolveCollision(UBall& Other) {
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

    void Render(URenderer* renderer, ID3D11Buffer* vertexBuffer) {
        if (nullptr ==  vertexBuffer) 
            return;

        renderer->UpdateConstant(Location, Radius);
        renderer->RenderPrimitive(vertexBuffer, sizeof(sphere_vertices) / sizeof(FVertexSimple));
    }

    // 랜덤 실수 생성
    float RandomFloat(float Min, float Max) {
        return Min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (Max - Min));
    }
};

class UBallManager {
public:
    UBall** BallList; // 이중 포인터를 이용한 공 리스트
    int BallCount;    // 현재 공의 개수
    int Capacity;      // 배열의 최대 크기 (미리 할당된 공간)
    URenderer* Renderer; // 렌더러 참조
    ID3D11Buffer* VertexBuffer;  // 버텍스 버퍼

    // 생성자
    UBallManager(URenderer* renderer) {
        BallList = nullptr;
        BallCount = 0;
        Capacity = 15;
        Renderer = renderer;
        VertexBuffer = nullptr;

        BallList = new UBall * [Capacity];

        // Vertex Buffer 생성
        InitializeVertexBuffer();
    }

    // 공 추가 함수
    void AddBall() {
        if (BallCount >= Capacity) {
            // 배열 크기를 2배로 확장
            Capacity *= 2;
            UBall** NewList = new UBall * [Capacity];

            // 기존 데이터 복사
            memcpy(NewList, BallList, BallCount * sizeof(UBall*));

            delete[] BallList;
            BallList = NewList;
        }

        // 새 공 추가
        BallList[BallCount] = new UBall();
        BallCount++;
    }

    // 공 제거 (삭제할 요소를 마지막 요소와 교체)
    void RemoveBall() {
        if (BallCount == 0) return;

        int RemoveIndex = rand() % BallCount;

        // 마지막 공과 자리 바꿈
        delete BallList[RemoveIndex];
        BallList[RemoveIndex] = BallList[BallCount - 1];
        BallCount--;

        // 배열이 너무 크면 줄이기
        if (BallCount < Capacity / 4 && Capacity > 15) {
            Capacity /= 2;
            UBall** NewList = new UBall * [Capacity];

            memcpy(NewList, BallList, BallCount * sizeof(UBall*));

            delete[] BallList;
            BallList = NewList;
        }
    }

    // 모든 공 이동
    void UpdateBalls() {
        if (bUseGravity) {

            for (int i = 0; i < BallCount; i++) {
                float acceleration = Gravity / BallList[i]->Mass; // 중력 가속도 = Gravity / Mass
                BallList[i]->Velocity.y += acceleration; // 질량에 비례한 중력 가속도 적용
            }
        }

        for (int i = 0; i < BallCount; i++) {
            BallList[i]->Move();
            BallList[i]->CheckWallCollision();
        }

        // 공이 1개 이하라면 충돌 연산 불필요
        if (BallCount < 2) return;

        // 충돌 감지 및 처리 (모든 공 쌍 확인)
        for (int i = 0; i < BallCount; i++) {
            for (int j = i + 1; j < BallCount; j++) {
                BallList[i]->ResolveCollision(*BallList[j]);
            }
        }
    }

    // 모든 공 렌더링
    void RenderBalls() {
        for (int i = 0; i < BallCount; i++) {
            BallList[i]->Render(Renderer, VertexBuffer);
        }
    }

    // VertexBuffer 초기화
    void InitializeVertexBuffer() {
        if (nullptr == VertexBuffer) {
            VertexBuffer = Renderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
        }
    }

    // Vertex Buffer 해제 (프로그램 종료 시 실행)
    void ReleaseVertexBuffer() {
        if (VertexBuffer) {
            Renderer->ReleaseVertexBuffer(VertexBuffer);
            VertexBuffer = nullptr;
        }
    }

    // 소멸자 (모든 공 삭제)
    ~UBallManager() {
        for (int i = 0; i < BallCount; i++) {
            delete BallList[i];
        }
        delete[] BallList;
        ReleaseVertexBuffer();
    }
};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
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

    UBallManager BallManager(&renderer);
    BallManager.AddBall();

    bool bIsExit = false;

    // ImGui를 생성합니다.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

    // FPS 제한을 위한 설정
    const int targetFPS = 60;
    const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    // 고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;

    // Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
    while (bIsExit == false) {
        // 루프 시작 시간 기록
        QueryPerformanceCounter(&startTime);

        MSG msg;

        // 처리할 메시지가 더 이상 없을때 까지 수행
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            // 키 입력 메시지를 번역
            TranslateMessage(&msg);

            // 메시지를 적절한 윈도우 프로시저에 전달, 메시지가 위에서 등록한 WndProc 으로 전달됨
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                bIsExit = true;
                break;
            }
        }

            BallManager.UpdateBalls();


            // 준비 작업
            renderer.Prepare();
            renderer.PrepareShader();

            BallManager.RenderBalls();

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // 이후 ImGui UI 컨트롤 추가는 ImGui::NewFrame()과 ImGui::Render() 사이인 여기에 위치합니다.
            ImGui::Begin("Jungle Property Window");

            ImGui::Text("Hello Jungle World!");

            // 중력 체크박스 추가
            ImGui::Checkbox("Use Gravity", &bUseGravity);

            static int ballCountInput = BallManager.BallCount;  // 현재 공 개수 저장

            // 공 개수 표시 및 수동 입력 가능
            if (ImGui::InputInt("Number Of Balls", &ballCountInput)) {
                // 공 개수를 입력한 값에 맞게 조정
                if (ballCountInput < 0) ballCountInput = 0; // 음수 방지

                if (ballCountInput > BallManager.BallCount) {
                    // 현재 개수보다 많으면 추가
                    while (BallManager.BallCount < ballCountInput) {
                        BallManager.AddBall();
                    }
                } else if (ballCountInput < BallManager.BallCount) {
                    // 현재 개수보다 적으면 삭제
                    while (BallManager.BallCount > ballCountInput) {
                        BallManager.RemoveBall();
                    }
                }
            }

            ImGui::End();

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // 현재 화면에 보여지는 버퍼와 그리기 작업을 위한 버퍼를 서로 교환합니다.
            renderer.SwapBuffer();

            do {
                Sleep(0);

                // 루프 종료 시간 기록
                QueryPerformanceCounter(&endTime);

                // 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
                elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

            } while (elapsedTime < targetFrameTime);
        }

        // ImGui 소멸
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        // ReleaseShader() 직전에 소멸 함수를 추가합니다.
        renderer.ReleaseConstantBuffer();
        renderer.ReleaseShader();
        renderer.Release();

        return 0;
}