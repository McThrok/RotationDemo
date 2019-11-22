#include "Simulation.h"

void Simulation::Init()
{
	startRotation = Vector3(0, 0, 0);
	endRotation = Vector3(0, 90, 0);

	startPosition = Vector3(0, 0, 0);
	endPosition = Vector3(2, 0, 0);

	paused = false;
	slerp = false;
	loop = false;
	showFrames = false;
	frames = 5;
	animationTime = 2.0f;

	Reset();
}

void Simulation::Reset()
{
	UpdateValues();
	UpdateFrames();
	time = 0;
}

void Simulation::UpdateValues()
{
	startRotationEuler = startRotation * XM_PI / 180;
	endRotationEuler = endRotation * XM_PI / 180;

	startRotationQuat = EtoQ(startRotationEuler);
	endRotationQuat = EtoQ(endRotationEuler);

	UpdateFrames();
}

void Simulation::Update(float dt)
{
	if (!paused)
		time += dt / 1000.0f;

	if (time > animationTime)
	{
		if (loop)
			time -= animationTime;
		else
			time = min(time, animationTime);
	}
}

void Simulation::UpdateFrames()
{
	framesEuler.clear();
	framesQuat.clear();

	for (int i = 1; i < frames + 1; i++)
	{
		float animationProgress = 1.0f * i / (frames + 1);
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
	Vector3 rot = (endRotationEuler - startRotationEuler) * animationProgress + startRotationEuler;

	return Matrix::CreateFromYawPitchRoll(rot.y, rot.x, rot.z) * Matrix::CreateTranslation(pos);//check order!!
}

Matrix Simulation::GetModelMatrixQuat(float animationProgress)
{
	Vector3 pos = (endPosition - startPosition) * animationProgress + startPosition;
	Quaternion rot;
	if (slerp)
		rot = Quaternion::Slerp(startRotationQuat, endRotationQuat, animationProgress);
	else
		rot = Quaternion::Lerp(startRotationQuat, endRotationQuat, animationProgress);

	return  Matrix::CreateFromQuaternion(rot) * Matrix::CreateTranslation(pos);
}

Matrix Simulation::GetModelMatrixEuler()
{
	return GetModelMatrixEuler(time / animationTime);
}

Matrix Simulation::GetModelMatrixQuat()
{
	return GetModelMatrixQuat(time / animationTime);
}