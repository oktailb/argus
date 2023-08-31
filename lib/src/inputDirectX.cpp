#include <input.hpp>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <chrono>

#ifdef WIN32
#include <windows.h>
#include <windows.h>
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <comdef.h>

using namespace std;

bool input::initDirectX(HWND hWndToCapture)
{
    // Initialisation de DirectX et création du device et du deviceContext
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_9_1 };
    D3D_FEATURE_LEVEL selectedFeatureLevel;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &device,
        &selectedFeatureLevel,
        &deviceContext
        );

    if (FAILED(hr)) {
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        cerr << "Erreur lors de la création du device : " << errMsg << std::endl;
        exit(0);
    }

    // Initialisation de DXGI Factory
    dxgiFactory = nullptr;
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

    if (FAILED(hr)) {
        // Gérer l'erreur
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        cerr << "Erreur lors de la création de la factory : " << errMsg << std::endl;
        deviceContext->Release();
        device->Release();
        exit(0);
    }

    // Création du swap chain
    swapChain = nullptr;
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    // Initialisez cette structure avec les paramètres appropriés
    swapChainDesc.BufferDesc.Width = width; // Largeur de la fenêtre
    swapChainDesc.BufferDesc.Height = height; // Hauteur de la fenêtre
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Format des pixels (par exemple, RGBA8 non signé)
    swapChainDesc.BufferDesc.RefreshRate.Numerator = fps; // Taux de rafraîchissement en images par seconde (numérateur)
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // Taux de rafraîchissement en images par seconde (dénominateur)
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Utilisation du swap chain
    swapChainDesc.BufferCount = 1; // Nombre de tampons dans le swap chain
    swapChainDesc.OutputWindow = hWnd; // Fenêtre cible
    swapChainDesc.SampleDesc.Count = 1; // Nombre d'échantillons par pixel
    swapChainDesc.SampleDesc.Quality = 0; // Qualité d'échantillonnage
    swapChainDesc.Windowed = TRUE; // Mode fenêtré (TRUE) ou plein écran (FALSE)
    hr = dxgiFactory->CreateSwapChain(device, &swapChainDesc, &swapChain);

    if (FAILED(hr)) {
        // Gérer l'erreur
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        cerr << "Erreur lors de la création de la swap chain : " << errMsg << std::endl;

        dxgiFactory->Release();
        deviceContext->Release();
        device->Release();
        exit(0);
    }

    // ...

    return true;
}

bool input::captureDirectX(char * buffer)
{
    // Lorsque vous souhaitez capturer les pixels
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    ID3DXBuffer* pBuffer = nullptr;
    // Maintenant, vous pouvez utiliser pBackBuffer et deviceContext pour la capture des pixels
    D3D11_TEXTURE2D_DESC desc;
    pBackBuffer->GetDesc(&desc);

    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;

    ID3D11Texture2D* pStagingTexture;
    device->CreateTexture2D(&desc, nullptr, &pStagingTexture);

    deviceContext->CopyResource(pStagingTexture, pBackBuffer);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    deviceContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);


//    // Maintenant, vous pouvez accéder aux pixels via mappedResource.pData
    memcpy(buffer, mappedResource.pData, width * height * 4);

    deviceContext->Unmap(pStagingTexture, 0);

    return true;
}

void input::cleanupDirectX()
{
    // Nettoyage
    pBackBuffer->Release();
    swapChain->Release();
    deviceContext->Release();
    device->Release();
}

#endif
