#include "Simulation.h"

void Simulation::Init()
{
	startRotation = Vector3(0, 0, 0);
	endRotation = Vector3(0, -XM_PIDIV4, 0);
	startPosition = Vector3(0, 0, 0);
	endPosition = Vector3(2, 0, 2);

	paused = false;
	slerp = false;
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
		time += dt;

	float animationProgress = time / animationTime;

	UpdateEuler(animationProgress);
	UpdateQuat(animationProgress);
}

void Simulation::UpdateEuler(float animationProgress)
{
	positionEuler = (endPosition - startPosition) * animationProgress + startPosition;
	rotationEuler = (endRotation - startRotation) * animationProgress + endRotation;
}

void Simulation::UpdateQuat(float animationProgress)
{
	positionQuat = (endPosition - startPosition) * animationProgress + startPosition;
	if (slerp)
		rotationQuat = Quaternion::Slerp(startRotationQ, endRotationQ, animationProgress);
	else
		rotationQuat = Quaternion::Lerp(startRotationQ, endRotationQ, animationProgress);
}

Quaternion Simulation::EtoQ(Vector3 rotation)
{
	return XMQuaternionRotationRollPitchYawFromVector(rotation);
}

Matrix Simulation::GetModelMatrixEuler()
{
	return Matrix::CreateTranslation(positionEuler) * Matrix::CreateFromYawPitchRoll(rotationEuler.x, rotationEuler.y, rotationEuler.z);//check order!!
}

Matrix Simulation::GetModelMatrixQuat()
{
	return Matrix::CreateTranslation(positionQuat) * Matrix::CreateFromQuaternion(rotationQuat);
}
