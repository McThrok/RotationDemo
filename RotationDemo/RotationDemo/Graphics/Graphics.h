#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "..\\Timer.h"
#include "..\\Simulation.h"
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"

using namespace std;

class Graphics
{
public:
	~Graphics();
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	Camera camera;
	Simulation* simulation;
private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();

	void InitGui(HWND hwnd);
	void RenderGui();
	void RenderMainPanel();
	void RenderVisualisation();
	void RenderModel(Matrix worldMatrix);
	void RenderCube(Matrix worldMatrix, Vector4 color);

	void updateFPSCounter();


	int viewportHeight;
	D3D11_VIEWPORT viewportTop;
	D3D11_VIEWPORT viewportDown;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader diagonalPixelshader;

	ConstantBuffer<ColoredObjectBuffer> cbColoredObject;
	ConstantBuffer<LightBuffer> cbLight;

	VertexBuffer<VertexPN> vbCube;
	IndexBuffer ibCube;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	int windowWidth = 0;
	int windowHeight = 0;
	Timer fpsTimer;
};