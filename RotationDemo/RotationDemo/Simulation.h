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
	Vector3 startRotation;
	Vector3 endRotation;

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
	void UpdateValues();
	void Update(float dt);
	void UpdateFrames();

	Matrix GetModelMatrixEuler(float animationProgress);
	Matrix GetModelMatrixQuat(float animationProgress);
	Matrix GetModelMatrixEuler();
	Matrix GetModelMatrixQuat();

	Quaternion EtoQ(Vector3 vecotr);
};

