#pragma once
#include <d3d11.h>
#include <vector>
#include <math.h> 
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <algorithm>

#include "Graphics/Vertex.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;



class Simulation
{
public:
	Vector3 startRotationEulerUI;
	Vector3 endRotationEulerUI;

	Quaternion startRotationQuatUI;
	Quaternion endRotationQuatUI;

	Vector3 startRotationEuler;
	Vector3 endRotationEuler;

	Quaternion startRotationQuat;
	Quaternion endRotationQuat;

	Vector3 startPosition;
	Vector3 endPosition;

	float time;
	bool paused;
	float animationTime;
	bool slerp;
	bool loop;
	int frames;
	bool showFrames;

	vector<Matrix> framesEuler;
	vector<Matrix> framesQuat;

	void Init();
	void Reset();
	void Update(float dt);
	void UpdateFrames();

	void UpdateRotationsFromEuler();
	void UpdateRotationsFromQuat();

	Matrix GetModelMatrixEuler(float animationProgress);
	Matrix GetModelMatrixQuat(float animationProgress);
	Matrix GetModelMatrixEuler();
	Matrix GetModelMatrixQuat();

	Quaternion EtoQ(Vector3 v);
	Vector3 QtoE(Quaternion q);
};

