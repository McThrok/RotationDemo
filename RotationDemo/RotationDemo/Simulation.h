#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Graphics/Vertex.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Simulation
{
public:
	Matrix initialRotation;
	float delta_time;
	float time;
	bool paused;
	bool gravityOn;

	int initialAngle;
	float cubeSize;
	float simulationSpeed;
	float density;
	float initialVelocity;

	Matrix InvI;
	Vector3 G;
	Vector3 R;

	Quaternion Q;
	Vector3 W;
	Matrix I;

	vector<VertexPN> probes;
	int maxProbes;
	int probesCycleCount;
	int probesCounter;


	void Init();
	void Reset();
	void Update(float dt);
	void Update();

	Matrix GetModelMatrix();
	void UpdateTensor();
	void UpdateProbes();
};

