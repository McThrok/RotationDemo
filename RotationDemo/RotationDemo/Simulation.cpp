#include "Simulation.h"

void Simulation::Init()
{
	startPosition = Vector3(0, 0, 0);
	endPosition = Vector3(2, 0, 0);

	paused = false;
	slerp = false;
	loop = false;
	showFrames = false;
	frames = 5;
	animationTime = 2.0f;

	Reset();
	UpdateFramesEuler();
	UpdateFramesQuat();
	UpdateRotationsFromEuler();
}

void Simulation::Reset()
{
	time = 0;
}

void Simulation::UpdateRotationsFromEuler()
{
	startRotationQuat = EtoQ(startRotationEuler * XM_PI / 180);
	endRotationQuat = EtoQ(endRotationEuler * XM_PI / 180);

	UpdateFramesEuler();
	UpdateFramesQuat();
}

void Simulation::UpdateRotationsFromQuat()
{
	startRotationQuat.Normalize();
	endRotationQuat.Normalize();

	startRotationEuler = QtoE(startRotationQuat) / XM_PI * 180;
	endRotationEuler = QtoE(endRotationQuat) / XM_PI * 180;

	UpdateFramesEuler();
	UpdateFramesQuat();
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
	UpdateFramesEuler(); 
	UpdateFramesQuat();
}
void Simulation::UpdateFramesEuler()
{
	framesEuler.clear();

	for (int i = 1; i < frames + 1; i++)
	{
		float animationProgress = 1.0f * i / (frames + 1);
		framesEuler.push_back(GetModelMatrixEuler(animationProgress));
	}
}

void Simulation::UpdateFramesQuat()
{
	framesQuat.clear();

	for (int i = 1; i < frames + 1; i++)
	{
		float animationProgress = 1.0f * i / (frames + 1);
		framesQuat.push_back(GetModelMatrixQuat(animationProgress));
	}
}

Quaternion Simulation::EtoQ(Vector3 v)
{
	double cy = cos(v.z * 0.5);
	double sy = sin(v.z * 0.5);
	double cp = cos(v.y * 0.5);
	double sp = sin(v.y * 0.5);
	double cr = cos(v.x * 0.5);
	double sr = sin(v.x * 0.5);

	Quaternion q;
	q.w = cy * cp * cr + sy * sp * sr;
	q.x = cy * cp * sr - sy * sp * cr;
	q.y = sy * cp * sr + cy * sp * cr;
	q.z = sy * cp * cr - cy * sp * sr;

	return q;
	//return XMQuaternionRotationMatrix(GetRotationMatrix(v));
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

Matrix Simulation::GetRotationMatrix(Vector3 v)
{
	Matrix m = Matrix::Identity;
	m *= Matrix::CreateRotationX(v.x);
	m *= Matrix::CreateRotationY(v.y);
	m *= Matrix::CreateRotationZ(v.z);
	return m;
}

Matrix Simulation::GetModelMatrixEuler(float animationProgress)
{
	Vector3 startRot = startRotationEuler * XM_PI / 180;
	Vector3 endRot = endRotationEuler * XM_PI / 180;

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

	return GetRotationMatrix(rot) * Matrix::CreateTranslation(pos);
}

Matrix Simulation::GetModelMatrixQuat(float animationProgress)
{
	Vector3 pos = (endPosition - startPosition) * animationProgress + startPosition;

	auto s = startRotationQuat;
	auto e = endRotationQuat;

	s.Normalize();
	e.Normalize();

	Quaternion rot;

	if (!slerp)
	{
		if (s.Dot(e) < 0.0f)
			e = -e;

		rot = (e - s) * animationProgress + s;
	}
	else
	{
		float dot = s.Dot(e);

		if (dot < 0.0f) {
			e = -e;
			dot = -dot;
		}

		const float eps = 0.9995f;
		if (dot > eps) {
			Quaternion rot = s + animationProgress * (e - s);
		}
		else
		{
			float theta_0 = acosf(dot);        
			float theta = theta_0 * animationProgress;         
			float sin_theta = sinf(theta);     
			float sin_theta_0 = sinf(theta_0); 
			
			float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;  
			float s1 = sin_theta / sin_theta_0;

			rot = (s0 * s) + (s1 * e);
		}
	}

	rot.Normalize();

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