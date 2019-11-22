#include "Graphics.h"

Graphics::~Graphics()
{
}

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	InitGui(hwnd);

	return true;
}
void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.05f, 0.05f, 0.1f, 1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	RenderVisualisation();
	RendeGui();
	updateFPSCounter();

	this->swapchain->Present(0, NULL);
}

void Graphics::updateFPSCounter() {
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringConverter::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();
}

void Graphics::InitGui(HWND hwnd) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();
}

void Graphics::RendeGui() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RenderMainPanel();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Graphics::RenderMainPanel() {
	ImGui::SetNextWindowSize(ImVec2(400, 950), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Once);
	if (!ImGui::Begin("Main Panel"))
	{
		ImGui::End();
		return;
	}

	if (simulation->paused) {
		if (ImGui::Button("Start"))
			simulation->paused = false;
	}
	else {
		if (ImGui::Button("Pause"))
			simulation->paused = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		simulation->Reset();
	}

	ImGui::SliderFloat("delta time", &simulation->delta_time, 0.01f, 0.2f);

	ImGui::Checkbox("show cube", &guiData->showCube);
	ImGui::Checkbox("show gravity", &guiData->showGravity);
	ImGui::Checkbox("show probes", &guiData->showProbes);
	ImGui::Checkbox("show diagonal", &guiData->showDiagonal);
	ImGui::Checkbox("center of mass", &guiData->showMassCenter);

	ImGui::Separator();
	bool update = false;
	if (ImGui::SliderFloat("cube size", &simulation->cubeSize, 0.1, 10)) update = true;
	if (ImGui::SliderFloat("density", &simulation->density, 0.1, 10)) update = true;
	if (ImGui::SliderInt("initial angle", &simulation->initialAngle, -180, 180)) update = true;
	if (ImGui::SliderFloat("initial angular velocity", &simulation->initialVelocity, 0, 5)) update = true;
	if (update) simulation->Reset();
	ImGui::Separator();

	ImGui::Checkbox("gravity", &simulation->gravityOn);
	ImGui::SliderFloat("simulation speed", &simulation->simulationSpeed, 0.1, 10);
	ImGui::SliderInt("probes count", &simulation->maxProbes, 50, 500);

	ImGui::End();
}

void Graphics::RenderVisualisation()
{
	this->deviceContext->IASetInputLayout(this->vertexshader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	UINT offset = 0;

	this->deviceContext->VSSetConstantBuffers(0, 1, this->cbColoredObject.GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cbColoredObject.GetAddressOf());

	if (guiData->showCube)
	{
		cbColoredObject.data.worldMatrix = simulation->GetModelMatrix();
		cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
		cbColoredObject.data.color = { 0.8f, 0.4f, 0.0f, 1.0f };

		if (!cbColoredObject.ApplyChanges()) return;
		this->deviceContext->IASetVertexBuffers(0, 1, vbCube.GetAddressOf(), vbCube.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(ibCube.Get(), DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(ibCube.BufferSize(), 0, 0);
	}

	if (guiData->showMassCenter) {
		cbColoredObject.data.worldMatrix = Matrix::CreateTranslation(-0.5, -0.5, -0.5) * Matrix::CreateScale(0.1f, 0.1f, 0.1f)
			* Matrix::CreateTranslation(0.5f, 0.5f, 0.5f) * simulation->GetModelMatrix();
		cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
		cbColoredObject.data.color = { 0.2f,0.8f,0.2f,1.0f };

		if (!cbColoredObject.ApplyChanges()) return;
		this->deviceContext->IASetVertexBuffers(0, 1, vbCube.GetAddressOf(), vbCube.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(ibCube.Get(), DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(ibCube.BufferSize(), 0, 0);

	}

	if (guiData->showDiagonal)
	{
		cbColoredObject.data.worldMatrix = Matrix::CreateTranslation(-0.5, -0.5, 0) * Matrix::CreateScale(0.03f, 0.03f, sqrtf(3))
			* XMMatrixRotationY(XMScalarACos(sqrtf(3) / 3)) * XMMatrixRotationZ(XM_PI / 4) * simulation->GetModelMatrix();
		cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
		cbColoredObject.data.color = { 1.0f,0.2f,0.2f,1.0f };

		if (!cbColoredObject.ApplyChanges()) return;
		this->deviceContext->IASetVertexBuffers(0, 1, vbCube.GetAddressOf(), vbCube.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(ibCube.Get(), DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(ibCube.BufferSize(), 0, 0);
	}

	if (guiData->showProbes)
	{
		this->deviceContext->PSSetShader(diagonalPixelshader.GetShader(), NULL, 0);

		cbColoredObject.data.worldMatrix = Matrix::Identity;
		cbColoredObject.data.wvpMatrix = cbColoredObject.data.worldMatrix * camera.GetViewMatrix() * camera.GetProjectionMatrix();
		cbColoredObject.data.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (!cbColoredObject.ApplyChanges()) return;
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);

		D3D11_MAPPED_SUBRESOURCE resource;
		this->deviceContext->Map(vbProbes.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, simulation->probes.data(), simulation->probes.size() * sizeof(VertexPN));
		this->deviceContext->Unmap(vbProbes.Get(), 0);

		this->deviceContext->IASetVertexBuffers(0, 1, vbProbes.GetAddressOf(), vbProbes.StridePtr(), &offset);
		this->deviceContext->Draw(simulation->probes.size(), 0);
	}
}


bool Graphics::InitializeDirectX(HWND hwnd)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No IDXGI Adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = this->windowWidth;
	scd.BufferDesc.Height = this->windowHeight;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, //FOR SOFTWARE DRIVER TYPE
		NULL, //FLAGS FOR RUNTIME LAYERS
		NULL, //FEATURE LEVELS ARRAY
		0, //# OF FEATURE LEVELS IN ARRAY
		D3D11_SDK_VERSION,
		&scd, //Swapchain description
		this->swapchain.GetAddressOf(), //Swapchain Address
		this->device.GetAddressOf(), //Device Address
		NULL, //Supported feature level
		this->deviceContext.GetAddressOf()); //Device Context Address

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain.");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = this->windowWidth;
	depthStencilDesc.Height = this->windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr)) //If error occurred
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view.");
		return false;
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state.");
		return false;
	}

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = this->windowWidth;
	viewport.Height = this->windowHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//Create Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");


	return true;
}

bool Graphics::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexshader.Initialize(this->device, L"my_vs.cso", layout, numElements))
		return false;

	if (!pixelshader.Initialize(this->device, L"my_ps.cso"))
		return false;

	if (!diagonalPixelshader.Initialize(this->device, L"diagonal_ps.cso"))
		return false;

	return true;
}

bool Graphics::InitializeScene()
{
	VertexPN v[] = {
		VertexPN(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		VertexPN(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		VertexPN(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		VertexPN(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f),

		VertexPN(1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexPN(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexPN(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		VertexPN(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f),

		VertexPN(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f),
		VertexPN(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f),

		VertexPN(0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f),
		VertexPN(0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VertexPN(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		VertexPN(0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f),

		VertexPN(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		VertexPN(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),
		VertexPN(0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),

		VertexPN(0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f),
		VertexPN(1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f),
		VertexPN(1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f),
		VertexPN(0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f)
	};

	int indices[] =
	{
		0, 2, 3, 0,1, 2,
		4, 6, 7, 4,5, 6,
		8, 10, 11, 8, 9, 10,
		12, 14, 15, 12, 13, 14,
		16, 18, 19, 16, 17, 18,
		20, 22, 23, 20, 21, 22,

		//0,3,2,0,2,1,
		//4,7,6,4,6,5,
		//8,11,10,8,10,9,
		//12,15,14,12,14,13,
		//16,19,18,16,18,17,
		//20,23,22,20,22,21
	};

	/*XMFLOAT3 p[] = {
		{ 0.000000 ,0.000000, 0.000000 },
		{ 0.000000 ,1.154701, 1.632993 },
		{ -1.414214 ,1.154701, -0.816497 },
		{ -1.414214 ,2.309401, 0.816497 },
		{ 1.414214 ,1.154701, -0.816497 },
		{ 1.414214 ,2.309401, 0.816497 },
		{ 0.000000 ,2.309401, -1.632993 },
		{ 0.000000 ,3.464102, 0.000000 },
	};

	XMFLOAT3 n[] = {
		{ -0.7071, -0.5774, 0.4082 },
		{ -0.7071, 0.5774 , -0.4082},
		{ 0.7071 , 0.5774 , -0.4082},
		{ 0.7071 , -0.5774, 0.4082 },
		{ -0.0000, -0.5774, -0.8165},
		{ 0.0000 , 0.5774 ,0.8165 },
	};

	VertexPN v[] = {
		VertexPN(p[1],n[0]),VertexPN(p[2],n[0]),VertexPN(p[0],n[0]),
		VertexPN(p[3],n[1]),VertexPN(p[6],n[1]),VertexPN(p[2],n[1]),

		VertexPN(p[7],n[2]),VertexPN(p[4],n[2]),VertexPN(p[6],n[2]),
		VertexPN(p[4],n[3]),VertexPN(p[1],n[3]),VertexPN(p[0],n[3]),

		VertexPN(p[6],n[4]),VertexPN(p[0],n[4]),VertexPN(p[2],n[4]),
		VertexPN(p[3],n[5]),VertexPN(p[5],n[5]),VertexPN(p[7],n[5]),

		VertexPN(p[1],n[0]),VertexPN(p[3],n[0]),VertexPN(p[2],n[0]),
		VertexPN(p[3],n[1]),VertexPN(p[7],n[1]),VertexPN(p[6],n[1]),

		VertexPN(p[7],n[2]),VertexPN(p[5],n[2]),VertexPN(p[4],n[2]),
		VertexPN(p[4],n[3]),VertexPN(p[5],n[3]),VertexPN(p[1],n[3]),

		VertexPN(p[6],n[4]),VertexPN(p[4],n[4]),VertexPN(p[0],n[4]),
		VertexPN(p[3],n[5]),VertexPN(p[1],n[5]),VertexPN(p[5],n[5]),
	};

	int indices[36];
	for (int i = 0; i < 36; i++)
		indices[i] = i;*/

	HRESULT hr = this->vbCube.Initialize(this->device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	//Load Index Data
	hr = this->ibCube.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return hr;
	}


	VertexPN dummy_verts[1000];
	for (int i = 0; i < 1000; i++)
		dummy_verts[i] = VertexPN(0, 0, 0, 0, 0, 0);

	hr = this->vbProbes.Initialize(this->device.Get(), dummy_verts, ARRAYSIZE(dummy_verts), true);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	//Initialize Constant Buffer(s)
	hr = this->cbColoredObject.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
		return false;
	}

	hr = this->cbLight.Initialize(this->device.Get(), this->deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to initialize constant buffer.");
		return false;
	}

	camera.SetPosition(0, -5.0f, 0);
	camera.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	return true;
}
