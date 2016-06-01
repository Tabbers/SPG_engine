#include "d3dc.h"
#include "globaldefinitions.h"



D3Dc::D3Dc()
{
	m_device					= nullptr;
	m_devCon					= nullptr;
	m_swapChain					= nullptr;
	m_targetView				= nullptr;
	m_depthBuffer				= nullptr;
	m_depthStencilState			= nullptr;
	m_depthStencilView			= nullptr;
	m_rasterStateBackCulling	= nullptr;
	m_rasterStateFrontCulling	= nullptr;
	m_rasterStateWireFrame		= nullptr;
}

D3Dc::D3Dc(const D3Dc &other)
{
}


D3Dc::~D3Dc()
{
	if (m_rasterStateWireFrame)
		m_rasterStateWireFrame->Release();
	if (m_rasterStateBackCulling)
		m_rasterStateBackCulling->Release();
	if (m_rasterStateFrontCulling)
		m_rasterStateFrontCulling->Release();
	if (m_depthStencilView)
		m_depthStencilView->Release();
	if (m_depthStencilState)
		m_depthStencilState->Release();
	if (m_depthBuffer)
		m_depthBuffer->Release();
	if (m_targetView)
		m_targetView->Release();
	if (m_devCon)
		m_devCon->Release();
	if (m_device)
		m_device->Release();
	if (m_swapChain)
		m_swapChain->Release();
}

bool D3Dc::Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen)
{
	// Win Result code
	HRESULT result;
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* usedAdapter = nullptr;
	IDXGIOutput* usedAdapterOut = nullptr;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList = nullptr;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	UINT qualitylevel = 0u;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the vsync setting.
	m_vsync_on = vsync;
	// graphics facotry interface
	adapters = EnumerateAdapters();
	//Primary monitor enumeration

	int id=0;
	int adapternumerator = 0;
	for each (IDXGIAdapter* adapter in adapters)
	{


		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result)) return false;
		if (adapterDesc.VendorId == NVIDIA)
		{
			id = adapternumerator;
		}
		adapternumerator++;
	}

	usedAdapter = adapters[id];
	usedAdapter->GetDesc(&adapterDesc);
	// get video Card name
	result = usedAdapter->GetDesc(&adapterDesc);
	if (FAILED(result)) return false;

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	// convert Name to char array of length 128
	error = wcstombs_s(&stringLength, m_vCardDesc, 128, adapterDesc.Description, 128);
	if (error != 0) return false;


	// Set the direct X version to be used
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &m_device, NULL, &m_devCon);
	if (FAILED(result)) return false;
	
	for (int sampleCount = 1; sampleCount < 9; sampleCount++)
	{
		switch (adapterDesc.VendorId)
		{
		case NVIDIA:
			this->m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &qualitylevel);
			if (qualitylevel != 0)
			{
				if (qualitylevel == 17 && sampleCount == 4)
				{ 
					m_msaa.QualityLevel.push_back(1);
					m_msaa.SampleCounts.push_back(sampleCount);

					m_msaa.QualityLevel.push_back(qualitylevel - 9);
					m_msaa.SampleCounts.push_back(sampleCount);

					m_msaa.QualityLevel.push_back(qualitylevel - 1);
					m_msaa.SampleCounts.push_back(sampleCount);
				}
				else if (qualitylevel == 33 && sampleCount == 8)
				{
					m_msaa.QualityLevel.push_back(1);
					m_msaa.SampleCounts.push_back(sampleCount);

					m_msaa.QualityLevel.push_back(qualitylevel - 17);
					m_msaa.SampleCounts.push_back(sampleCount);

					m_msaa.QualityLevel.push_back(qualitylevel - 1);
					m_msaa.SampleCounts.push_back(sampleCount);

				}
				else
				{
					if (sampleCount != 1)
					{
						m_msaa.QualityLevel.push_back(1);
						m_msaa.SampleCounts.push_back(sampleCount);
					}

					m_msaa.QualityLevel.push_back(qualitylevel - 1);
					m_msaa.SampleCounts.push_back(sampleCount);
				}
				
			}
		
			break;
		case AMD: case INTEL:
			if (qualitylevel != 0)
			{
				m_msaa.QualityLevel.push_back(qualitylevel-1);
				m_msaa.SampleCounts.push_back(sampleCount);
			}
			break;
		default:
			break;
		}
	}
	if (m_msaa.QualityLevel.size() == 0 && m_msaa.SampleCounts.size() == 0)
	{
		m_msaa.QualityLevel.push_back(0);
		m_msaa.SampleCounts.push_back(1);
	}


	// release unneeded values
	if (displayModeList != nullptr)
	{
		delete[] displayModeList;
	}
	for each (IDXGIAdapter* adapter in adapters)
	{
		adapter->Release();
	}

	// Initialize the swapchain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	// set the format to 32 bit surface
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//vsync settings
	if (m_vsync_on)
	{
		//Adapt render speed to the refresh of the Monitor
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
	{
		// Render as fast as possible
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	// Set the Backbuffer as the standard output for rendering
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Set the window for rendering
	swapChainDesc.OutputWindow = hwnd;

	m_msaaIndex = 0;

	// Disable Mulitsampling
	swapChainDesc.SampleDesc.Count = m_msaa.SampleCounts[m_msaaIndex];
	swapChainDesc.SampleDesc.Quality = m_msaa.QualityLevel[m_msaaIndex];

	// Senable windowd mode
	swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// Set the direct X version to be used
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//create the device and swapchain 
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_devCon);
	if (FAILED(result)) return false;
	
	// Get a Back buffer pointer
	result = m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D),(LPVOID*)&m_backBufferPtr);
	if (FAILED(result)) return false;
	
	// create rendertargetView
	result = m_device->CreateRenderTargetView(m_backBufferPtr, NULL, &m_targetView);
	if (FAILED(result)) return false;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc = swapChainDesc.SampleDesc;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthBuffer);
	if (FAILED(result)) return false;

	/*
		Stencilbuffer used to mask out rendered content and or implement shadows
	*/

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result)) return false;

	// Set the stencils state to be active
	m_devCon->OMSetDepthStencilState(m_depthStencilState, 1);

	//Create the depth stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	//Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilParticleState);

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthBuffer, NULL, &m_depthStencilView);
	if (FAILED(result)) return false;

	// Bind the render target and stencil buffer to the render pipeline
	m_devCon->OMSetRenderTargets(1, &m_targetView, m_depthStencilView);

	// Set up rasterization
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create rasterized state
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateBackCulling);
	if (FAILED(result)) return false;

	rasterDesc.CullMode = D3D11_CULL_FRONT;
	
	// Create rasterized state
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateFrontCulling);
	if (FAILED(result)) return false;

	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;

	// Create rasterized state
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateWireFrame);
	if (FAILED(result)) return false;

	m_devCon->RSSetState(m_rasterStateBackCulling);

	D3D11_BLEND_DESC blendDesc = {};

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	if (m_device->CreateBlendState(&blendDesc, &m_blendstateAlphaEnabled) != S_OK)


	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	if (m_device->CreateBlendState(&blendDesc, &m_blendstateAlphaDisabled) != S_OK)

	DisableAlphaBlending();
	//Initialize the World Matrix
	m_worldMatrix = XMMatrixIdentity();
	return true;
}

void D3Dc::BeginScene(XMFLOAT4 bgcolor)
{
	float color[4] { bgcolor.x,bgcolor.y,bgcolor.z,bgcolor.w };
	m_devCon->ClearRenderTargetView(m_targetView, color);
	m_devCon->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3Dc::EndScene()
{
	if (m_vsync_on) m_swapChain->Present(1, 0);
	else m_swapChain->Present(0, 0);
	return;
}

std::vector <IDXGIAdapter*> D3Dc::EnumerateAdapters(void)
{
	IDXGIAdapter * pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;
	IDXGIFactory* pFactory = NULL;


	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
	{
		return vAdapters;
	}


	for (UINT i = 0;
	pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;++i)
	{
		vAdapters.push_back(pAdapter);
	}


	if (pFactory)
	{
		pFactory->Release();
	}

	return vAdapters;

}
