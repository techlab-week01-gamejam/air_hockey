#include <windows.h>

// D3D ��뿡 �ʿ��� ���̺귯������ ��ũ�մϴ�.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D ��뿡 �ʿ��� ������ϵ��� �����մϴ�.
#include <d3d11.h>
#include <d3dcompiler.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

#include "SoundManager.h"
#include "TextureManager.h"
#include "SpriteAnimationManager.h" 
#include "vector3.h"

#include "DebugLog.h"

bool bUseGravity = false; // �߷� ���� ���� (�⺻ OFF)
float Gravity = -0.001f; // �߷� ���ӵ� �� (���� ��: �Ʒ� ����)
int colliisionCount = 0; // �浹 Ƚ��

// 1. Define the triangle vertices
struct FVertexSimple {
    float x, y, z;    // Position
    float r, g, b, a; // Color
};

#include "Sphere.h"

class URenderer {
public:
    // Direct3D 11 ��ġ(Device)�� ��ġ ���ؽ�Ʈ(Device Context) �� ���� ü��(Swap Chain)�� �����ϱ� ���� �����͵�
    ID3D11Device* Device = nullptr; // GPU�� ����ϱ� ���� Direct3D ��ġ
    ID3D11DeviceContext* DeviceContext = nullptr; // GPU ��� ������ ����ϴ� ���ؽ�Ʈ
    IDXGISwapChain* SwapChain = nullptr; // ������ ���۸� ��ü�ϴ� �� ���Ǵ� ���� ü��

    // �������� �ʿ��� ���ҽ� �� ���¸� �����ϱ� ���� ������
    ID3D11Texture2D* FrameBuffer = nullptr; // ȭ�� ��¿� �ؽ�ó
    ID3D11RenderTargetView* FrameBufferRTV = nullptr; // �ؽ�ó�� ���� Ÿ������ ����ϴ� ��
    ID3D11RasterizerState* RasterizerState = nullptr; // �����Ͷ����� ����(�ø�, ä��� ��� �� ����)
    ID3D11Buffer* ConstantBuffer = nullptr; // ���̴��� �����͸� �����ϱ� ���� ��� ����
	ID3D11BlendState* AlphaBlendState = nullptr; // ���� ���� ����

    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // ȭ���� �ʱ�ȭ(clear)�� �� ����� ���� (RGBA)
    D3D11_VIEWPORT ViewportInfo; // ������ ������ �����ϴ� ����Ʈ ����

    ID3D11VertexShader* SimpleVertexShader;
    ID3D11PixelShader* SimplePixelShader;
    ID3D11InputLayout* SimpleInputLayout;
    unsigned int Stride;


public:
    // ������ �ʱ�ȭ �Լ�
    void Create(HWND hWindow) {
        // Direct3D ��ġ �� ���� ü�� ����
        CreateDeviceAndSwapChain(hWindow);

        // ������ ���� ����
        CreateFrameBuffer();

        // �����Ͷ����� ���� ����
        CreateRasterizerState();

        // ���� ���� ������Ʈ ����
        CreateBlendState();
    }

    // Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
    void CreateDeviceAndSwapChain(HWND hWindow) {
        // �����ϴ� Direct3D ��� ������ ����
        D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

        // ���� ü�� ���� ����ü �ʱ�ȭ
        DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
        swapchaindesc.BufferDesc.Width = 0; // â ũ�⿡ �°� �ڵ����� ����
        swapchaindesc.BufferDesc.Height = 0; // â ũ�⿡ �°� �ڵ����� ����
        swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // ���� ����
        swapchaindesc.SampleDesc.Count = 1; // ��Ƽ ���ø� ��Ȱ��ȭ
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ���� Ÿ������ ���
        swapchaindesc.BufferCount = 2; // ���� ���۸�
        swapchaindesc.OutputWindow = hWindow; // �������� â �ڵ�
        swapchaindesc.Windowed = TRUE; // â ���
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ���� ���

        // Direct3D ��ġ�� ���� ü���� ����
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
            featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
            &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

        // ������ ���� ü���� ���� ��������
        SwapChain->GetDesc(&swapchaindesc);

        // ����Ʈ ���� ����
        ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
    }

    // Direct3D ��ġ �� ���� ü���� �����ϴ� �Լ�
    void ReleaseDeviceAndSwapChain() {
        if (DeviceContext) {
            DeviceContext->Flush(); // �����ִ� GPU ��� ����
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

    // ������ ���۸� �����ϴ� �Լ�
    void CreateFrameBuffer() {
        // ���� ü�����κ��� �� ���� �ؽ�ó ��������
        SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

        // ���� Ÿ�� �� ����
        D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
        framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // ���� ����
        framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D �ؽ�ó

        Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
    }

    // ������ ���۸� �����ϴ� �Լ�
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

    // �����Ͷ����� ���¸� �����ϴ� �Լ�
    void CreateRasterizerState() {
        D3D11_RASTERIZER_DESC rasterizerdesc = {};
        rasterizerdesc.FillMode = D3D11_FILL_SOLID; // ä��� ���
        rasterizerdesc.CullMode = D3D11_CULL_BACK; // �� ���̽� �ø�

        Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
    }

    // �����Ͷ����� ���¸� �����ϴ� �Լ�
    void ReleaseRasterizerState() {
        if (RasterizerState) {
            RasterizerState->Release();
            RasterizerState = nullptr;
        }
    }

    // �������� ���� ��� ���ҽ��� �����ϴ� �Լ�
    void Release() {
        RasterizerState->Release();

        // ���� Ÿ���� �ʱ�ȭ
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

        ReleaseFrameBuffer();
        ReleaseDeviceAndSwapChain();

		if (AlphaBlendState) {
			AlphaBlendState->Release();
			AlphaBlendState = nullptr;
		}
    }

    // ���� ü���� �� ���ۿ� ����Ʈ ���۸� ��ü�Ͽ� ȭ�鿡 ���
    void SwapBuffer() {
        SwapChain->Present(1, 0); // 1: VSync Ȱ��ȭ
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
        DeviceContext->OMSetBlendState(AlphaBlendState, nullptr, 0xffffffff);
    }

    void PrepareShader() {
        DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
        DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
        DeviceContext->IASetInputLayout(SimpleInputLayout);

        // ���ؽ� ���̴��� ��� ���۸� �����մϴ�.
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

    // ��� ���� ����, �Ҹ� �Լ� �߰�
    struct FConstants {
        FVector3 Offset;
        float Scale; // ���� ũ��(������)
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

    // ��� ���۸� �����ϴ� �Լ�
    void UpdateConstant(FVector3 Offset, float Scale) {
        if (ConstantBuffer) {
            D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

            DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
            FConstants* constants = (FConstants*)constantbufferMSR.pData;
            {
                constants->Offset = Offset;
                constants->Scale = Scale; // ���� ũ�� ����
            }
            DeviceContext->Unmap(ConstantBuffer, 0);
        }
    }

    void CreateBlendState() {
        D3D11_BLEND_DESC blendDesc = {};
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

        // ��� ���� Ÿ�꿡 ������ ���� �ɼ��� ����
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = TRUE;

        // �ҽ� ���Ŀ� ���ҽ� ���ĸ� �̿��� ������ ���
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        // ���� ä�ο� ���ؼ��� ������ ���� ����
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

        // RGBA ��� ����
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        HRESULT hr = Device->CreateBlendState(&blendDesc, &AlphaBlendState);
    }
};

class UBall {
public:
    FVector3 Location;  // ���� ��ġ
    FVector3 Velocity;  // ���� �ӵ�
    float Radius;       // ���� ������
    float Mass;         // ���� ����

    // ������: ������ ũ��, ��ġ, �ӵ��� ���� �� ����
    UBall() {
        Radius = RandomFloat(0.05f, 0.15f); // 0.05~0.15 ������ ���� ũ��
        Mass = Radius * 10.0f; // ũ�⿡ ����ϴ� ����
        Location = FVector3(RandomFloat(-0.8f, 0.8f), RandomFloat(-0.8f, 0.8f), 0.0f);
        Velocity = FVector3(RandomFloat(-0.02f, 0.02f), RandomFloat(-0.02f, 0.02f), 0.0f);
    }

    // ���� �̵���Ű�� �Լ�
    void Move() {
        Location.x += Velocity.x;
        Location.y += Velocity.y;
    }

    // ������ �浹 ó��
    void CheckWallCollision() {
        const float leftBorder = -1.0f + Radius;
        const float rightBorder = 1.0f - Radius;
        const float topBorder = 1.0f - Radius;
        const float bottomBorder = -1.0f + Radius;
        SoundManager* SoundMgr = SoundManager::GetInstance();

        if (Location.x < leftBorder) {
            Location.x = leftBorder;  // ��ġ ����
            Velocity.x *= -1.0f;      // �ݻ�

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
                Velocity.y *= -0.8f; // �ٴ� �浹 �� ź�� �ս�
                if (fabs(Velocity.y) < 0.01f) { // �ſ� ���� ���̸� ����
                    Velocity.y = 0.0f;
                }
            } else {
                Velocity.y *= -1.0f; // �߷��� ���� ���� ���� �ݻ�
            }
        }
    }

    // �� �� ������ �浹 ó��
    void ResolveCollision(UBall& Other) {
        FVector3 Diff = Location - Other.Location;
        float Distance = sqrt(Diff.x * Diff.x + Diff.y * Diff.y);
        float MinDist = Radius + Other.Radius;

        if (Distance < MinDist) // �浹 �߻�
        {
            SoundManager* SoundMgr = SoundManager::GetInstance();
            // �浹 �� �ӵ� ��ȯ (ź�� �浹 ���� ����)
            FVector3 Normal = Diff / Distance;
            FVector3 RelativeVelocity = Velocity - Other.Velocity;
            float Speed = (RelativeVelocity.x * Normal.x + RelativeVelocity.y * Normal.y);

            SoundMgr->PlaySFX("Hit");

            AddDebugLog("Collision Detected: " + std::to_string(++colliisionCount));

            if (Speed > 0) return; // �̹� �־����� �ִٸ� ó������ ����

            float Impulse = 2.0f * Speed / (Mass + Other.Mass);
            Velocity -= Normal * Impulse * Other.Mass;
            Other.Velocity += Normal * Impulse * Mass;

            // ��ġ ����: �浹�� ���� �о ��ġ�� �ʵ��� ��
            float Overlap = MinDist - Distance;
            FVector3 Correction = Normal * (Overlap / 2.0f);

            Location += Correction;
            Other.Location -= Correction;

            // �ִϸ��̼� ���� (�浹 ��������)
            FVector3 CollisionPoint = (Location + Other.Location) * 0.5f;
            SpriteAnimationManager::GetInstance()->PlayAnimation("Hit1", CollisionPoint, 0.2f);
        }
    }

    void Render(URenderer* renderer, ID3D11Buffer* vertexBuffer) {
        if (nullptr ==  vertexBuffer) 
            return;

        renderer->UpdateConstant(Location, Radius);
        renderer->RenderPrimitive(vertexBuffer, sizeof(sphere_vertices) / sizeof(FVertexSimple));
    }

    // ���� �Ǽ� ����
    float RandomFloat(float Min, float Max) {
        return Min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (Max - Min));
    }
};

class UBallManager {
public:
    UBall** BallList; // ���� �����͸� �̿��� �� ����Ʈ
    int BallCount;    // ���� ���� ����
    int Capacity;      // �迭�� �ִ� ũ�� (�̸� �Ҵ�� ����)
    URenderer* Renderer; // ������ ����
    ID3D11Buffer* VertexBuffer;  // ���ؽ� ����

    // ������
    UBallManager(URenderer* renderer) {
        BallList = nullptr;
        BallCount = 0;
        Capacity = 15;
        Renderer = renderer;
        VertexBuffer = nullptr;

        BallList = new UBall * [Capacity];

        // Vertex Buffer ����
        InitializeVertexBuffer();
    }

    // �� �߰� �Լ�
    void AddBall() {
        if (BallCount >= Capacity) {
            // �迭 ũ�⸦ 2��� Ȯ��
            Capacity *= 2;
            UBall** NewList = new UBall * [Capacity];

            // ���� ������ ����
            memcpy(NewList, BallList, BallCount * sizeof(UBall*));

            delete[] BallList;
            BallList = NewList;
        }

        // �� �� �߰�
        BallList[BallCount] = new UBall();
        BallCount++;
    }

    // �� ���� (������ ��Ҹ� ������ ��ҿ� ��ü)
    void RemoveBall() {
        if (BallCount == 0) return;

        int RemoveIndex = rand() % BallCount;

        // ������ ���� �ڸ� �ٲ�
        delete BallList[RemoveIndex];
        BallList[RemoveIndex] = BallList[BallCount - 1];
        BallCount--;

        // �迭�� �ʹ� ũ�� ���̱�
        if (BallCount < Capacity / 4 && Capacity > 15) {
            Capacity /= 2;
            UBall** NewList = new UBall * [Capacity];

            memcpy(NewList, BallList, BallCount * sizeof(UBall*));

            delete[] BallList;
            BallList = NewList;
        }
    }

    // ��� �� �̵�
    void UpdateBalls() {
        if (bUseGravity) {

            for (int i = 0; i < BallCount; i++) {
                float acceleration = Gravity / BallList[i]->Mass; // �߷� ���ӵ� = Gravity / Mass
                BallList[i]->Velocity.y += acceleration; // ������ ����� �߷� ���ӵ� ����
            }
        }

        for (int i = 0; i < BallCount; i++) {
            BallList[i]->Move();
            BallList[i]->CheckWallCollision();
        }

        // ���� 1�� ���϶�� �浹 ���� ���ʿ�
        if (BallCount < 2) return;

        // �浹 ���� �� ó�� (��� �� �� Ȯ��)
        for (int i = 0; i < BallCount; i++) {
            for (int j = i + 1; j < BallCount; j++) {
                BallList[i]->ResolveCollision(*BallList[j]);
            }
        }
    }

    // ��� �� ������
    void RenderBalls() {
        for (int i = 0; i < BallCount; i++) {
            BallList[i]->Render(Renderer, VertexBuffer);
        }
    }

    // VertexBuffer �ʱ�ȭ
    void InitializeVertexBuffer() {
        if (nullptr == VertexBuffer) {
            VertexBuffer = Renderer->CreateVertexBuffer(sphere_vertices, sizeof(sphere_vertices));
        }
    }

    // Vertex Buffer ���� (���α׷� ���� �� ����)
    void ReleaseVertexBuffer() {
        if (VertexBuffer) {
            Renderer->ReleaseVertexBuffer(VertexBuffer);
            VertexBuffer = nullptr;
        }
    }

    // �Ҹ��� (��� �� ����)
    ~UBallManager() {
        for (int i = 0; i < BallCount; i++) {
            delete BallList[i];
        }
        delete[] BallList;
        ReleaseVertexBuffer();
    }
};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ���� �޽����� ó���� �Լ�
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
    // ������ Ŭ���� �̸�
    WCHAR WindowClass[] = L"JungleWindowClass";

    // ������ Ÿ��Ʋ�ٿ� ǥ�õ� �̸�
    WCHAR Title[] = L"Game Tech Lab";

    // ���� �޽����� ó���� �Լ��� WndProc�� �Լ� �����͸� WindowClass ����ü�� �ִ´�.
    WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

    // ������ Ŭ���� ���
    RegisterClassW(&wndclass);

    // 1024 x 1024 ũ�⿡ ������ ����
    HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
        nullptr, nullptr, hInstance, nullptr);

    FMOD::System* system;
    FMOD::System_Create(&system);

    // Renderer Class�� �����մϴ�.
    URenderer	renderer;

    // D3D11 �����ϴ� �Լ��� ȣ���մϴ�.
    renderer.Create(hWnd);
    // ������ ���� ���Ŀ� ���̴��� �����ϴ� �Լ��� ȣ���մϴ�.
    renderer.CreateShader();
    renderer.CreateConstantBuffer();

    UBallManager BallManager(&renderer);
    BallManager.AddBall();

    bool bIsExit = false;

    // ImGui�� �����մϴ�.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(renderer.Device, renderer.DeviceContext);

    // FPS ������ ���� ����
    const int targetFPS = 60;
    const double targetFrameTime = 1000.0 / targetFPS; // �� �������� ��ǥ �ð� (�и��� ����)

    // ���� Ÿ�̸� �ʱ�ȭ
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;
    double deltaTime = 0.0f;

    /* SoundManager Init */
    SoundManager* SoundMgr = SoundManager::GetInstance();

    if (!SoundMgr->Initialize()) {
        return -1;
    }

    SoundMgr->LoadSound("Hit", "hit.mp3");

    /* TextureManager �ʱ�ȭ*/
    TextureManager::GetInstance()->Initiallize(renderer.Device, renderer.DeviceContext);

	/* SpriteAnimationManager �ʱ�ȭ */ 
    SpriteAnimationManager::GetInstance()->Initialize(renderer.Device);

    // �ִϸ��̼� ���
	SpriteAnimationManager::GetInstance()->RegisterAnimation("Hit1", "hit_1.png", 1, 5, 0.05f, renderer.Device);
    SpriteAnimationManager::GetInstance()->RegisterAnimation("Hit2", "hit_2.png", 1, 4, 0.05f, renderer.Device);

    // Main Loop (Quit Message�� ������ ������ �Ʒ� Loop�� ������ �����ϰ� ��)
    while (bIsExit == false) {
        // ���� ���� �ð� ���
        QueryPerformanceCounter(&startTime);

        MSG msg;

        // ó���� �޽����� �� �̻� ������ ���� ����
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            // Ű �Է� �޽����� ����
            TranslateMessage(&msg);

            // �޽����� ������ ������ ���ν����� ����, �޽����� ������ ����� WndProc ���� ���޵�
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                bIsExit = true;
                break;
            }
        }

        // �����̽� �ٸ� ������ �� ȿ���� ���
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            SoundMgr->PlaySFX("Hit");
            Sleep(100);  // �ߺ� �Է� ���� (0.1�� ������)
        }

        BallManager.UpdateBalls();
        SpriteAnimationManager::GetInstance()->Update(deltaTime);
		SoundManager::GetInstance()->Update();


        // �غ� �۾�
        renderer.Prepare();
        renderer.PrepareShader();

        BallManager.RenderBalls();
        SpriteAnimationManager::GetInstance()->Render(renderer.DeviceContext);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ���� ImGui UI ��Ʈ�� �߰��� ImGui::NewFrame()�� ImGui::Render() ������ ���⿡ ��ġ�մϴ�.
        ImGui::Begin("Jungle Property Window");

        ImGui::Text("Hello Jungle World!");

        // �߷� üũ�ڽ� �߰�
        ImGui::Checkbox("Use Gravity", &bUseGravity);

        static int ballCountInput = BallManager.BallCount;  // ���� �� ���� ����

        // �� ���� ǥ�� �� ���� �Է� ����
        if (ImGui::InputInt("Number Of Balls", &ballCountInput)) {
            // �� ������ �Է��� ���� �°� ����
            if (ballCountInput < 0) ballCountInput = 0; // ���� ����

            if (ballCountInput > BallManager.BallCount) {
                // ���� �������� ������ �߰�
                while (BallManager.BallCount < ballCountInput) {
                    BallManager.AddBall();
                }
            } else if (ballCountInput < BallManager.BallCount) {
                // ���� �������� ������ ����
                while (BallManager.BallCount > ballCountInput) {
                    BallManager.RemoveBall();
                }
            }
        }

        ImGui::End();

        ShowDebugWindow();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // ���� ȭ�鿡 �������� ���ۿ� �׸��� �۾��� ���� ���۸� ���� ��ȯ�մϴ�.
        renderer.SwapBuffer();

        do {
            Sleep(0);

            // ���� ���� �ð� ���
            QueryPerformanceCounter(&endTime);

            // �� �������� �ҿ�� �ð� ��� (�и��� ������ ��ȯ)
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
			deltaTime = elapsedTime / 1000.0; // �� ������ ��ȯ

        } while (elapsedTime < targetFrameTime);
    }

    // ImGui �Ҹ�
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // ReleaseShader() ������ �Ҹ� �Լ��� �߰��մϴ�.
    renderer.ReleaseConstantBuffer();
    renderer.ReleaseShader();
    renderer.Release();

    return 0;
}