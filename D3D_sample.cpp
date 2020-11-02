// D3D_sample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <windows.h>
#include <initguid.h>
#include <atlbase.h>

//D3D9
#include <d3d9.h>

//D3D11
#include <d3d11.h>
#include <dxgi1_2.h>

//DXVA
#include <dxva2api.h>
#include <dxva.h>

#define D3DFMT_NV12 (D3DFORMAT) MAKEFOURCC('N', 'V', '1', '2')

int memcpy_CPU2GPU_D3D9_ARGB()
{
    IDirect3D9Ex* d3d;
    IDirect3DDevice9Ex* dev;
    IDirect3DSurface9* surface;
    void* buffer;
    int width = 1920;
    int height = 1080;
    int copies = 1000;
    int bufferLen = width * height * 4;

    int hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);
    if (FAILED(hr)) {
        printf("Failed to Create Direct3D9Ex: %d", hr);
        return 1;
    }

    D3DPRESENT_PARAMETERS p;
    p.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    p.BackBufferCount = 2;
    p.BackBufferFormat = D3DFMT_A8R8G8B8;
    p.BackBufferWidth = width;
    p.BackBufferHeight = height;
    p.EnableAutoDepthStencil = FALSE;
    p.Flags = 0;
    p.hDeviceWindow = 0;
    p.MultiSampleQuality = 0;
    p.MultiSampleType = D3DMULTISAMPLE_NONE;
    p.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    p.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p.Windowed = TRUE;
    p.FullScreen_RefreshRateInHz = 0;

    hr = d3d->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &p, NULL, &dev);
    if (FAILED(hr)) {
        printf("Failed to Create Device: %d", hr);
        return 1;
    }

    hr = dev->CreateOffscreenPlainSurfaceEx(width, height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surface, NULL, 0);
    if (FAILED(hr)) {
        printf("Failed to Create Surface: %d", hr);
        return 1;
    }

    buffer = malloc(bufferLen);

    LARGE_INTEGER freq;
    LARGE_INTEGER t1, t2;
    LONGLONG milliseconds;
    LONGLONG bandwidth = 0;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    D3DLOCKED_RECT r;

    memset(buffer, 255, bufferLen);

    printf("%s: Copying %d %dx%d surfaces...\n", __FUNCTION__, copies, width, height);

    for (int i = 0; i < copies; i++)
    {
        surface->LockRect(&r, NULL, 0);
        memcpy(r.pBits, buffer, bufferLen);
        bandwidth += bufferLen;
        surface->UnlockRect();
    }

    QueryPerformanceCounter(&t2);
    milliseconds = (t2.QuadPart - t1.QuadPart) * 1000 / freq.QuadPart;
    bandwidth = ((bandwidth * 1000) / milliseconds) / 1024 / 1024;
    printf("Transfer took %I64d ms ( %I64d MB/sec )\n", milliseconds, bandwidth);
}

int memcpy_CPU2GPU_D3D9_NV12()
{
    IDirect3D9Ex* d3d;
    IDirect3DDevice9Ex* dev;
    IDirect3DSurface9* surface;
    void* buffer;
    int width = 1920;
    int height = 1080;
    int copies = 1000;
    int bufferLen = width * height * 3 / 2;

    int hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);
    if (FAILED(hr)) {
        printf("Failed to Create Direct3D9Ex: %d", hr);
        return 1;
    }

    D3DPRESENT_PARAMETERS p;
    p.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    p.BackBufferCount = 2;
    p.BackBufferFormat = D3DFMT_A8R8G8B8;
    p.BackBufferWidth = width;
    p.BackBufferHeight = height;
    p.EnableAutoDepthStencil = FALSE;
    p.Flags = 0;
    p.hDeviceWindow = 0;
    p.MultiSampleQuality = 0;
    p.MultiSampleType = D3DMULTISAMPLE_NONE;
    p.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    p.SwapEffect = D3DSWAPEFFECT_DISCARD;
    p.Windowed = TRUE;
    p.FullScreen_RefreshRateInHz = 0;

    hr = d3d->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &p, NULL, &dev);
    if (FAILED(hr)) {
        printf("Failed to Create Device: %d", hr);
        return 1;
    }

    hr = dev->CreateOffscreenPlainSurfaceEx(width, height, D3DFMT_NV12, D3DPOOL_DEFAULT, &surface, NULL, 0);
    if (FAILED(hr)) {
        printf("Failed to Create Surface: %d", hr);
        return 1;
    }

    buffer = malloc(bufferLen);

    LARGE_INTEGER freq;
    LARGE_INTEGER t1, t2;
    LONGLONG milliseconds;
    LONGLONG bandwidth = 0;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    D3DLOCKED_RECT r;

    memset(buffer, 255, bufferLen);

    printf("%s: Copying %d %dx%d surfaces...\n", __FUNCTION__, copies, width, height);

    for (int i = 0; i < copies; i++)
    {
        surface->LockRect(&r, NULL, 0);
        memcpy(r.pBits, buffer, bufferLen);
        bandwidth += bufferLen;
        surface->UnlockRect();
    }

    QueryPerformanceCounter(&t2);
    milliseconds = (t2.QuadPart - t1.QuadPart) * 1000 / freq.QuadPart;
    bandwidth = ((bandwidth * 1000) / milliseconds) / 1024 / 1024;
    printf("Transfer took %I64d ms ( %I64d MB/sec )\n", milliseconds, bandwidth);
}

int memcpy_CPU2GPU_D3D11_ARGB()
{
    ID3D11Device* g_pD3D11Device;
    ID3D11DeviceContext* g_pD3D11Ctx;
    IDXGIFactory2* g_pDXGIFactory;
    IDXGIAdapter* g_pAdapter;

    HRESULT hres = CreateDXGIFactory1(__uuidof(IDXGIFactory2),
        (void**)(&g_pDXGIFactory));
    if (FAILED(hres))
    {
        printf("Failed to CreateDXGIFactory: %d", hres);
        return 1;
    }

    IDXGIAdapter* adapter;
    hres = g_pDXGIFactory->EnumAdapters(0, &g_pAdapter);
    if (FAILED(hres))
    {
        printf("Failed to EnumAdapters: %d", hres);
        return 1;
    }

    static D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_1,
                        D3D_FEATURE_LEVEL_11_0,
                        D3D_FEATURE_LEVEL_10_1,
                        D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL pFeatureLevelsOut;
    UINT dxFlags = 0;
    //UINT dxFlags = D3D11_CREATE_DEVICE_DEBUG;

    hres = D3D11CreateDevice(
        g_pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, dxFlags,
        FeatureLevels,
        (sizeof(FeatureLevels) / sizeof(FeatureLevels[0])),
        D3D11_SDK_VERSION, &g_pD3D11Device, &pFeatureLevelsOut,
        &g_pD3D11Ctx);
    if (FAILED(hres))
    {
        printf("Failed to D3D11CreateDevice: %d", hres);
        return 1;
    }

    // turn on multithreading for the DX11 context
    CComQIPtr<ID3D10Multithread> p_mt(g_pD3D11Ctx);
    if (p_mt)
        p_mt->SetMultithreadProtected(true);

    // create default surface
    D3D11_TEXTURE2D_DESC desc = { 0 };
    desc.Width = 1920;
    desc.Height = 1080;
    desc.MipLevels = 1;
    desc.ArraySize = 1; // number of subresources is 1 in this case
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = 0;
    ID3D11Texture2D* pSurf;
    hres = g_pD3D11Device->CreateTexture2D(&desc, NULL, &pSurf);
    if (FAILED(hres))
    {
        printf("Failed to CreateTexture2D (default): %d", hres);
        return 1;
    }

    // Create staging surface
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    ID3D11Texture2D* pStagingSurf;
    hres = g_pD3D11Device->CreateTexture2D(&desc, NULL, &pStagingSurf);
    if (FAILED(hres))
    {
        printf("Failed to CreateTexture2D (staging): %d", hres);
        return 1;
    }

    // copy
    void* buffer;
    int width = 1920;
    int height = 1080;
    int copies = 1000;
    int bufferLen = width * height * 4;
    buffer = malloc(bufferLen);

    LARGE_INTEGER freq;
    LARGE_INTEGER t1, t2;
    LONGLONG milliseconds;
    LONGLONG bandwidth = 0;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    memset(buffer, 255, bufferLen);

    printf("%s: Copying %d %dx%d surfaces...\n", __FUNCTION__, copies, width, height);

    for (int i = 0; i < copies; i++)
    {
        D3D11_MAPPED_SUBRESOURCE lockedRect = { 0 };
        do {
            hres = g_pD3D11Ctx->Map(pStagingSurf, 0, D3D11_MAP_READ_WRITE,
                D3D11_MAP_FLAG_DO_NOT_WAIT, &lockedRect);
            if (S_OK != hres && DXGI_ERROR_WAS_STILL_DRAWING != hres)
            {
                printf("Failed to Map: %d", hres);
                return 1;
            }
        } while (DXGI_ERROR_WAS_STILL_DRAWING == hres);
        memcpy(lockedRect.pData, buffer, bufferLen);
        bandwidth += bufferLen;
        g_pD3D11Ctx->Unmap(pStagingSurf, 0);

        // If want update default surrafe, call either one below.
        //g_pD3D11Ctx->CopySubresourceRegion(pSurf, 0, 0, 0, 0, pStagingSurf, 0, NULL);
        //g_pD3D11Ctx->CopyResource(pSurf, pStagingSurf);
    }

    QueryPerformanceCounter(&t2);
    milliseconds = (t2.QuadPart - t1.QuadPart) * 1000 / freq.QuadPart;
    bandwidth = ((bandwidth * 1000) / milliseconds) / 1024 / 1024;
    printf("Transfer took %I64d ms ( %I64d MB/sec )\n", milliseconds, bandwidth);

    // exit
    if (pSurf)
    {
        pSurf->Release();
        pSurf = NULL;
    }
    if (pStagingSurf)
    {
        pStagingSurf->Release();
        pStagingSurf = NULL;
    }
    if (g_pAdapter) {
        g_pAdapter->Release();
        g_pAdapter = NULL;
    }
    if (g_pD3D11Device) {
        g_pD3D11Device->Release();
        g_pD3D11Device = NULL;
    }
    if (g_pD3D11Ctx) {
        g_pD3D11Ctx->Release();
        g_pD3D11Ctx = NULL;
    }
    if (g_pDXGIFactory) {
        g_pDXGIFactory->Release();
        g_pDXGIFactory = NULL;
    }
}

int memcpy_CPU2GPU_D3D11_NV12()
{
    ID3D11Device* g_pD3D11Device;
    ID3D11DeviceContext* g_pD3D11Ctx;
    IDXGIFactory2* g_pDXGIFactory;
    IDXGIAdapter* g_pAdapter;

    HRESULT hres = CreateDXGIFactory1(__uuidof(IDXGIFactory2),
        (void**)(&g_pDXGIFactory));
    if (FAILED(hres))
    {
        printf("Failed to CreateDXGIFactory: %d", hres);
        return 1;
    }

    IDXGIAdapter* adapter;
    hres = g_pDXGIFactory->EnumAdapters(0, &g_pAdapter);
    if (FAILED(hres))
    {
        printf("Failed to EnumAdapters: %d", hres);
        return 1;
    }

    static D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_1,
                        D3D_FEATURE_LEVEL_11_0,
                        D3D_FEATURE_LEVEL_10_1,
                        D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL pFeatureLevelsOut;
    UINT dxFlags = 0;
    //UINT dxFlags = D3D11_CREATE_DEVICE_DEBUG;

    hres = D3D11CreateDevice(
        g_pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, dxFlags,
        FeatureLevels,
        (sizeof(FeatureLevels) / sizeof(FeatureLevels[0])),
        D3D11_SDK_VERSION, &g_pD3D11Device, &pFeatureLevelsOut,
        &g_pD3D11Ctx);
    if (FAILED(hres))
    {
        printf("Failed to D3D11CreateDevice: %d", hres);
        return 1;
    }

    // turn on multithreading for the DX11 context
    CComQIPtr<ID3D10Multithread> p_mt(g_pD3D11Ctx);
    if (p_mt)
        p_mt->SetMultithreadProtected(true);

    // create default surface
    D3D11_TEXTURE2D_DESC desc = { 0 };
    desc.Width = 1920;
    desc.Height = 1080;
    desc.MipLevels = 1;
    desc.ArraySize = 1; // number of subresources is 1 in this case
    desc.Format = DXGI_FORMAT_NV12;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = 0;
    ID3D11Texture2D* pSurf;
    hres = g_pD3D11Device->CreateTexture2D(&desc, NULL, &pSurf);
    if (FAILED(hres))
    {
        printf("Failed to CreateTexture2D (default): %d", hres);
        return 1;
    }

    // Create staging surface
    desc.ArraySize = 1;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    ID3D11Texture2D* pStagingSurf;
    hres = g_pD3D11Device->CreateTexture2D(&desc, NULL, &pStagingSurf);
    if (FAILED(hres))
    {
        printf("Failed to CreateTexture2D (staging): %d", hres);
        return 1;
    }

    // copy
    void* buffer;
    int width = 1920;
    int height = 1080;
    int copies = 1000;
    int bufferLen = width * height * 3 / 2;
    buffer = malloc(bufferLen);

    LARGE_INTEGER freq;
    LARGE_INTEGER t1, t2;
    LONGLONG milliseconds;
    LONGLONG bandwidth = 0;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);

    memset(buffer, 255, bufferLen);

    printf("%s: Copying %d %dx%d surfaces...\n", __FUNCTION__, copies, width, height);

    for (int i = 0; i < copies; i++)
    {
        D3D11_MAPPED_SUBRESOURCE lockedRect = { 0 };
        do {
            hres = g_pD3D11Ctx->Map(pStagingSurf, 0, D3D11_MAP_READ_WRITE,
                D3D11_MAP_FLAG_DO_NOT_WAIT, &lockedRect);
            if (S_OK != hres && DXGI_ERROR_WAS_STILL_DRAWING != hres)
            {
                printf("Failed to Map: %d", hres);
                return 1;
            }
        } while (DXGI_ERROR_WAS_STILL_DRAWING == hres);
        memcpy(lockedRect.pData, buffer, bufferLen);
        bandwidth += bufferLen;
        g_pD3D11Ctx->Unmap(pStagingSurf, 0);

        // If want update default surrafe, call either one below.
        //g_pD3D11Ctx->CopySubresourceRegion(pSurf, 0, 0, 0, 0, pStagingSurf, 0, NULL);
        //g_pD3D11Ctx->CopyResource(pSurf, pStagingSurf);
    }

    QueryPerformanceCounter(&t2);
    milliseconds = (t2.QuadPart - t1.QuadPart) * 1000 / freq.QuadPart;
    bandwidth = ((bandwidth * 1000) / milliseconds) / 1024 / 1024;
    printf("Transfer took %I64d ms ( %I64d MB/sec )\n", milliseconds, bandwidth);

    // exit
    if (pSurf)
    {
        pSurf->Release();
        pSurf = NULL;
    }
    if (pStagingSurf)
    {
        pStagingSurf->Release();
        pStagingSurf = NULL;
    }
    if (g_pAdapter) {
        g_pAdapter->Release();
        g_pAdapter = NULL;
    }
    if (g_pD3D11Device) {
        g_pD3D11Device->Release();
        g_pD3D11Device = NULL;
    }
    if (g_pD3D11Ctx) {
        g_pD3D11Ctx->Release();
        g_pD3D11Ctx = NULL;
    }
    if (g_pDXGIFactory) {
        g_pDXGIFactory->Release();
        g_pDXGIFactory = NULL;
    }
}

int main()
{
    //std::cout << "Hello World!\n";

    memcpy_CPU2GPU_D3D9_ARGB();
    memcpy_CPU2GPU_D3D9_NV12(); // Why this is much faster than ARGB? Seems wrong.

    memcpy_CPU2GPU_D3D11_ARGB();
    memcpy_CPU2GPU_D3D11_NV12();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
