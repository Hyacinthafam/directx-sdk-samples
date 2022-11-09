//--------------------------------------------------------------------------------------
// File: Tutorial04.cpp
//
// This application displays a 3D cube using Direct3D 11
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729721.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
/*#include <assimp/Importer.hpp>       
#include <assimp/scene.h>            
#include <assimp/postprocess.h>*/   
//Assimp::Importer::ReadFile()

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};


struct ConstantBuffer
{
    XMMATRIX mWorld;
    XMMATRIX mView;
    XMMATRIX mProjection;
};


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = nullptr;
HWND                    g_hWnd = nullptr;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11Device1* g_pd3dDevice1 = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
IDXGISwapChain1* g_pSwapChain1 = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;
ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pIndexBuffer = nullptr;
ID3D11Buffer* g_pConstantBuffer = nullptr;
XMMATRIX                g_World;
XMMATRIX                g_View;
XMMATRIX                g_Projection;


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (FAILED(InitWindow(hInstance, nCmdShow)))
        return 0;

    if (FAILED(InitDevice()))
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 4: 3D Spaces",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);
    if (!g_hWnd)
        return E_FAIL;

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
    if (dxgiFactory2)
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
        if (SUCCEEDED(hr))
        {
            (void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = g_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"Tutorial04.fxh", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader);
    if (FAILED(hr))
    {
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(), &g_pVertexLayout);
    pVSBlob->Release();
    if (FAILED(hr))
        return hr;

    // Set the input layout
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"Tutorial04.fxh", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }
    ID3D11RasterizerState* m_rasterState = 0;

    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterDesc.ScissorEnable = false;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    hr = g_pd3dDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);

    g_pImmediateContext->RSSetState(m_rasterState);
    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader);
    pPSBlob->Release();
    if (FAILED(hr))
        return hr;

    /*const auto radius = 1.0f;
    const auto pi = 3.14;*/

    const int m = 7;
   const  int n = 8;
    float w = -1.0f;
    float d = 1.0f;

    float halfWidth = 0.5f * w;

    float halfDepth = -1.5f * d;

    float dx = w / (n - 1);

    float dz = d / (m - 1);

    constexpr  auto nVertices = m * n;

    constexpr  auto nStrips = (m - 1) * (n - 1) * 2;

    SimpleVertex gridVertices[100] = {};

    for (int i = 0; i < m; ++i) {

        float z = halfDepth - i * dz;

        for (int j = 0; j < n; ++j) {

            float x = -halfWidth + j * dx;

            gridVertices[i * n + j].Pos = XMFLOAT3(x, 0.0f, z);


            gridVertices[i * n + j].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);

        }

    };



    /* Create vertex buffer

    const auto radius = 1.0f;
    const auto pi = 3.14;

   SimpleVertex vertices[] =


    {


       

        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(radius*XMScalarCos(XM_PI/3), 1.0f, radius*XMScalarSin(3.14/3)), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI *2 / 3), 1.0f, radius * XMScalarSin(XM_PI*2 / 3)), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI *3/ 3), 1.0f, radius * XMScalarSin(XM_PI *3 / 3)), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI *4/ 3), 1.0f, radius * XMScalarSin(XM_PI *4 / 3)), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI *5/ 3), 1.0f, radius * XMScalarSin(XM_PI *5 / 3)), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI *6/ 3), 1.0f, radius * XMScalarSin(XM_PI *6 / 3)), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },


        { XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI / 3), -1.0f, radius * XMScalarSin(3.14 / 3)), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI * 2 / 3), -1.0f, radius * XMScalarSin(XM_PI * 2 / 3)), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI * 3 / 3), -1.0f, radius * XMScalarSin(XM_PI * 3 / 3)), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI * 4 / 3), -1.0f, radius * XMScalarSin(XM_PI * 4 / 3)), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI * 5 / 3), -1.0f, radius * XMScalarSin(XM_PI * 5 / 3)), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT3(radius * XMScalarCos(XM_PI * 6 / 3), -1.0f, radius * XMScalarSin(XM_PI * 6 / 3)), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },*/


   ////     
   ////    
   ////    /* {XMFLOAT3(0.0f, -2.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
   ////     { XMFLOAT3(0.5f, -2.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
   ////     { XMFLOAT3(1.0f, -2.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
   ////     { XMFLOAT3(0.5f, -2.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
   ////     { XMFLOAT3(-0.5f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
   ////     { XMFLOAT3(-1.0f, -2.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
   ////     { XMFLOAT3(-0.5f, -2.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
   ////     { XMFLOAT3(1.0f, -2.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },*/
   //      };
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * nStrips * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = gridVertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

    // Create index buffer
    //WORD indices[] =
    //{
    //   6,0,1,
    //   1,0,2,
    //   2,0,3,
    //   3,0,4,
    //   4,0,5,
    //   5,0,6,
    //    //6,0,1,
    //    /*-2,0,3,
    //     3,4,5,
    //    -5,3,4,


    //     0,5,4,
    //     1,5,0,

    //     3,4,7,
    //     0,4,3,

    //     1,6,5,
    //     2,6,1,

    //     2,7,6,
    //     // 3,1,2,
    //      6,4,5,
    //      6,4,5,
    //      7,4,6,*/

    //   7,13,8,
    //   8,7,9,
    //   9,7,10,
    //   10,7,11,
    //   11,7,12,
    //   12,7,13,
    //   13,7,8,


    // 5,6,13,
    //  5,12,13,
    //  6,13,1,
    //  1,13,8,
    //  1,8,9,
    //  1,9,2,
    //  2,9,3,
    //  3,10,4,
    //  4,10,11,
    //  4,11,12,
    //  12,4,5,


    //};
   // const auto nStrips = 0;
    WORD gridIndices[nStrips * 3] = {};

    int k = 0;



    for (int i = 0; i < m - 1; ++i) {

        for (int j = 0; j < n - 1; ++j) {



            gridIndices[k] = i * n + j;

            gridIndices[k + 1] = i * n + (j + 1);

            gridIndices[k + 2] = (i + 1) * n + j;

            gridIndices[k + 3] = (i + 1) * n + j;

            gridIndices[k + 4] = i * n + (j + 1);

            gridIndices[k + 5] = (i + 1) * n + (j + 1);



            k += 6;

        }

    };




    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * nStrips * 3;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = gridIndices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
    if (FAILED(hr))
        return hr;

    // Set index buffer
    g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
    if (FAILED(hr))
        return hr;

    // Initialize the world matrix
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet(0.0f, 4.0f,-5.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(5.0f, 2.0f, 0.0f, 0.0f);
    g_View = XMMatrixLookAtLH(Eye, At, Up);

    // Initialize the projection matrix
    g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

    if (g_pConstantBuffer) g_pConstantBuffer->Release();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pIndexBuffer) g_pIndexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain1) g_pSwapChain1->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext1) g_pImmediateContext1->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice1) g_pd3dDevice1->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Update our time
    static float t = 0.0f;
    if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float)XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if (timeStart == 0)
            timeStart = timeCur;
        t = (timeCur - timeStart) / 1000.0f;
    }

    //
    // Animate the cube
    //
   // g_World = XMMatrixRotationY(t);

    //
    // Clear the back buffer
    //
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);

    //
    // Update variables
    //
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(g_World);
    cb.mView = XMMatrixTranspose(g_View);
    cb.mProjection = XMMatrixTranspose(g_Projection);
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //
    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
    g_pImmediateContext->DrawIndexed(420, 0, 0);        // 36 vertices needed for 12 triangles in a triangle list

    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present(0, 0);
}

