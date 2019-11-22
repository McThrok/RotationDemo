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

	Quaternion startRotationQ;
	Quaternion endRotationQ;

	Vector3 startPosition;
	Vector3 endPosition;

	Vector3 positionEuler;
	Vector3 rotationEuler;

	Vector3 positionQuat;
	Quaternion rotationQuat;

	float time;
	bool paused;
	float animationTime;
	bool slerp;
	bool loop;
	int frames;

	vector<Matrix> framesEuler;
	vector<Matrix> framesQuat;

	void Init();
	void Reset();
	void Update(float dt);
	void UpdateFrames();

	Matrix GetModelMatrixEuler(float animationProgress);
	Matrix GetModelMatrixQuat(float animationProgress);
	Matrix GetModelMatrixEuler();
	Matrix GetModelMatrixQuat();

	Quaternion EtoQ(Vector3 vecotr);
};

