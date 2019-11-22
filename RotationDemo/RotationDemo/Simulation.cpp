#include "Simulation.h"

void Simulation::Init()
{
	simulationSpeed = 1;
	density = 1;
	cubeSize = 1;
	initialVelocity = 0;
	paused = false;
	gravityOn = true;
	maxProbes = 100;
	initialAngle = 0;
	delta_time = 0.001;
	probesCycleCount = 10;

	Reset();
}

void Simulation::Reset()
{
	UpdateTensor();
	Q = Quaternion::Identity;

	W = Vector3(1, 1, 1);
	W.Normalize();
	W *= initialVelocity;

	time = 0;
	probesCounter = 0;
	probes.clear();
}

void Simulation::UpdateTensor()
{
	I = XMFLOAT3X3(
		2.0f / 3.0f, -0.25, -0.25,
		-0.25, 2.0f / 3.0f, -0.25,
		-0.25, -0.25, 2.0f / 3.0f);

	float m = cubeSize * cubeSize * cubeSize * density;
	I *= m;

	InvI = I.Invert();

	initialRotation = XMMatrixRotationZ(-XM_PI / 4) * XMMatrixRotationY(-XMScalarACos(sqrtf(3.0f) / 3.0f));
	//initialRotation = Matrix::Identity;
	initialRotation *= XMMatrixRotationY(XM_PI * initialAngle / 180.0f);
	G = XMVector3Normalize(XMVector3TransformNormal(Vector3(0, 0, -1), initialRotation.Transpose()));
	R = cubeSize * Vector3(0.5f, 0.5f, 0.5f);
}

void Simulation::Update(float dt)
{
	if (paused)
		return;

	time += dt / 1000;
	float timePerStep = delta_time / simulationSpeed;

	while (time >= timePerStep)
	{
		Update();
		UpdateProbes();
		time -= timePerStep;
	}
}

void Simulation::Update()
{
	Quaternion w0 = Quaternion(W, 0);
	Quaternion k1 = delta_time * 0.5f * XMQuaternionMultiply((Q), w0);
	Quaternion k2 = delta_time * 0.5f * XMQuaternionMultiply((Q + k1 * 0.5f), w0);
	Quaternion k3 = delta_time * 0.5f * XMQuaternionMultiply((Q + k2 * 0.5f), w0);
	Quaternion k4 = delta_time * 0.5f * XMQuaternionMultiply((Q + k3), w0);

	Quaternion newQ = XMQuaternionNormalize(Q + (Quaternion)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f));


	Vector3 N = Vector3(0, 0, 0);
	if (gravityOn)
		N = XMVector3Cross(R, XMVector3Rotate(G, XMQuaternionInverse(Q)));

	Vector3 Iww = XMVector3Cross(XMVector3TransformNormal(W, I), W);
	k1 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	Iww = XMVector3Cross(XMVector3TransformNormal(W + k1 * 0.5f, I), W + k1 * 0.5f);
	k2 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	Iww = XMVector3Cross(XMVector3TransformNormal(W + k2 * 0.5f, I), W + k2 * 0.5f);
	k3 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	Iww = XMVector3Cross(XMVector3TransformNormal(W + k3, I), W + k3);
	k4 = delta_time * XMVector3TransformNormal(N + Iww, InvI);

	Vector3 newW = W + (Vector3)((k1 + 2.0f * k2 + 2.0f * k3 + k4) / 6.0f);

	Q = newQ;
	W = newW;
}

void Simulation::UpdateProbes()
{
	while (probesCounter >= probesCycleCount)
		probesCounter -= probesCycleCount;

	if (probesCounter == 0)
	{
		Vector3 v = XMVector3TransformNormal(2 * R, GetModelMatrix());
		probes.push_back(VertexPN(v, { 0,0,0 }));
		if (probes.size() > maxProbes)
			probes.erase(probes.begin(), probes.begin() + probes.size() - maxProbes);
	}

	probesCounter++;
}

Matrix Simulation::GetModelMatrix()
{
	return Matrix::CreateScale(cubeSize) * (Matrix)XMMatrixRotationQuaternion(Q) * initialRotation;
}
