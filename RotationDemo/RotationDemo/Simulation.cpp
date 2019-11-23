#include "Simulation.h"

void Simulation::Init()
{
	startRotationEulerUI = Vector3(0, 0, 0);
	endRotationEulerUI = Vector3(0, 90, 0);

	startPosition = Vector3(0, 0, 0);
	endPosition = Vector3(2, 0, 0);

	paused = false;
	slerp = false;
	loop = false;
	showFrames = false;
	frames = 5;
	animationTime = 2.0f;

	Reset();
	UpdateFrames();
	UpdateRotationsFromEuler();
}

void Simulation::Reset()
{
	time = 0;
}

void Simulation::UpdateRotationsFromEuler()
{
	startRotationEuler = startRotationEulerUI * XM_PI / 180;
	endRotationEuler = endRotationEulerUI * XM_PI / 180;

	startRotationQuatUI = EtoQ(startRotationEuler);
	endRotationQuatUI = EtoQ(endRotationEuler);

	startRotationQuat = startRotationQuatUI;
	endRotationQuat = endRotationQuatUI;

	UpdateFrames();
}

void Simulation::UpdateRotationsFromQuat()
{
	startRotationQuat = startRotationQuatUI;
	endRotationQuat = endRotationQuatUI;

	startRotationEuler = QtoE(startRotationQuat);
	endRotationEuler = QtoE(endRotationQuat);

	startRotationEulerUI = startRotationEuler / XM_PI * 180;
	endRotationEulerUI = endRotationEuler / XM_PI * 180;

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

Quaternion Simulation::EtoQ(Vector3 v)
{
	return XMQuaternionRotationRollPitchYawFromVector(v);
}

Vector3 Simulation::QtoE(Quaternion q)
{
	q.Normalize();
	Vector3 angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = atan2f(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		angles.y = copysignf(XM_PIDIV2, sinp); // use 90 degrees if out of range
	else
		angles.y = asinf(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = atan2(siny_cosp, cosy_cosp);

	return angles;
}

Matrix Simulation::GetModelMatrixEuler(float animationProgress)
{
	Vector3 startRot = startRotationEuler;
	Vector3 endRot = endRotationEuler;

	while (startRot.x > XM_2PI) startRot.x -= XM_2PI;
	while (startRot.y > XM_2PI) startRot.y -= XM_2PI;
	while (startRot.z > XM_2PI) startRot.z -= XM_2PI;

	while (endRot.x > XM_2PI) endRot.x -= XM_2PI;
	while (endRot.y > XM_2PI) endRot.y -= XM_2PI;
	while (endRot.z > XM_2PI) endRot.z -= XM_2PI;

	while (startRot.x < 0) startRot.x += XM_2PI;
	while (startRot.y < 0) startRot.y += XM_2PI;
	while (startRot.z < 0) startRot.z += XM_2PI;

	while (endRot.x < 0) endRot.x += XM_2PI;
	while (endRot.y < 0) endRot.y += XM_2PI;
	while (endRot.z < 0) endRot.z += XM_2PI;

	if (endRot.x - startRot.x > XM_PI) endRot.x -= XM_2PI;
	if (endRot.y - startRot.y > XM_PI) endRot.y -= XM_2PI;
	if (endRot.z - startRot.z > XM_PI) endRot.z -= XM_2PI;

	if (startRot.x - endRot.x > XM_PI) endRot.x += XM_2PI;
	if (startRot.y - endRot.y > XM_PI) endRot.y += XM_2PI;
	if (startRot.z - endRot.z > XM_PI) endRot.z += XM_2PI;

	Vector3 rot = (endRot - startRot) * animationProgress + startRot;
	Vector3 pos = (endPosition - startPosition) * animationProgress + startPosition;


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