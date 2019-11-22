#include "Simulation.h"

void Simulation::Init()
{
	startRotation = Vector3(0, 0, 0);
	endRotation = Vector3(0, -XM_PIDIV4, 0);
	startPosition = Vector3(0, 0, 0);
	endPosition = Vector3(2, 0, 2);

	paused = false;
	slerp = false;
	loop = false;
	frames = 5;
	animationTime = 2.0f;

	Reset();
}

void Simulation::Reset()
{
	positionEuler = startPosition;
	rotationEuler = startRotation;

	startRotationQ = EtoQ(startRotation);
	endRotationQ = EtoQ(endRotation);

	positionQuat = startPosition;
	rotationQuat = EtoQ(startRotation);
	time = 0;
}

void Simulation::Update(float dt)
{
	if (!paused)
		time += dt / 1000.0f;

	if (loop)
		time -= animationTime;
	else
		time = min(time, animationTime);
}

void Simulation::UpdateFrames()
{
	framesEuler.clear();
	framesQuat.clear();

	for (int i = 0; i < frames + 2; i++)
	{
		float animationProgress = i / (frames + 1);
		framesEuler.push_back(GetModelMatrixEuler(animationProgress));
		framesQuat.push_back(GetModelMatrixQuat(animationProgress));
	}
}

Quaternion Simulation::EtoQ(Vector3 rotation)
{
	return XMQuaternionRotationRollPitchYawFromVector(rotation);
}

Matrix Simulation::GetModelMatrixEuler(float animationProgress)
{
	Vector3 pos = (endPosition - startPosition) * animationProgress + startPosition;
	Vector3 rot = (endRotation - startRotation) * animationProgress + endRotation;

	return Matrix::CreateTranslation(pos) * Matrix::CreateFromYawPitchRoll(rot.x, rot.y, rot.z);//check order!!
}

Matrix Simulation::GetModelMatrixQuat(float animationProgress)
{
	Vector3 pos = (endPosition - startPosition) * animationProgress + startPosition;
	Quaternion rot;
	if (slerp)
		rot = Quaternion::Slerp(startRotationQ, endRotationQ, animationProgress);
	else
		rot = Quaternion::Lerp(startRotationQ, endRotationQ, animationProgress);

	return Matrix::CreateTranslation(pos) * Matrix::CreateFromQuaternion(rot);
}

Matrix Simulation::GetModelMatrixEuler()
{
	return GetModelMatrixEuler(time / animationTime);
}

Matrix Simulation::GetModelMatrixQuat()
{
	return GetModelMatrixQuat(time / animationTime);
}