#pragma once
#include <vector>
#include <DirectXMath.h>

typedef unsigned int UINT;

const bool VSYNC = true;
const float DEG_TO_RAD = 0.0174532925F;
const int SHADOWMAP_WIDTH = 2048;
const int SHADOWMAP_HEIGHT = 2048;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const float SCREEN_DEPTHL = 1000.0f;
const float SCREEN_NEARL = 0.1f;
const float PI = 3.1415926535898f;
const UINT NVIDIA = 0x10DEu;
const UINT AMD = 0x1002u;
const UINT INTEL = 0x8086u;
const DirectX::XMFLOAT4 BACKGROUNDCOLOR = { 0.2f, 0.2f, 0.2f, 1.0f };
const float RAYCASTDISTANCE = 500.0f;

namespace Variables
{
	static int currentAxis;
}

enum class Modes 
{ 
	CAMERA, 
	LIGHT, 
};

struct Matrices
{
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMFLOAT3 cameraPosition;
	bool DrawNormal;
	bool DrawSpec;
	bool DrawDisp;
	bool DrawHardShadows;

	inline Matrices(DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, DirectX::XMVECTOR cameraPosition)
	{
		this->worldMatrix = worldMatrix;
		this->viewMatrix = viewMatrix;
		this->projectionMatrix = projectionMatrix;
		this->cameraPosition.x = cameraPosition.m128_f32[0];
		this->cameraPosition.y = cameraPosition.m128_f32[1];
		this->cameraPosition.z = cameraPosition.m128_f32[2];
	};
};
struct LightData
{
	DirectX::XMMATRIX lightViewMatrix;
	DirectX::XMMATRIX lightProjectionMatrix;
	
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT4 specularColor;
	DirectX::XMFLOAT3 lightDirection;
	DirectX::XMFLOAT3 cameraDirection;
	float specularIntensity;

	DirectX::XMFLOAT3 lightPosition;

	inline LightData(DirectX::XMMATRIX lightViewMatrix, DirectX::XMMATRIX lightProjectionMatrix, 
		DirectX::XMFLOAT4 ambientColor, DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT4 specularColor,
		float specularIntensity, DirectX::XMFLOAT3 lightPosition, DirectX::XMFLOAT3 lightDirection, 
		DirectX::XMFLOAT3 cameraDirection)
	{
		this->lightViewMatrix = lightViewMatrix;
		this->lightProjectionMatrix = lightProjectionMatrix;
		this->ambientColor = ambientColor;
		this->diffuseColor = diffuseColor;
		this->specularColor = specularColor;
		this->specularIntensity = specularIntensity;
		this->lightPosition = lightPosition;
		this->lightDirection = lightDirection;
		this->cameraDirection = cameraDirection;
	}
};

struct MSAASettings
{
	std::vector<int> SampleCounts;
	std::vector<int> QualityLevel;
};

struct Triangle
{
	DirectX::XMVECTOR v1;
	DirectX::XMVECTOR v2;
	DirectX::XMVECTOR v3;

	float center[3];
	float lowerCorner[3];
	float upperCorner[3];

	DirectX::XMVECTOR PointIntersect;
	float distance;

	bool intersection = false;
	inline Triangle(DirectX::XMVECTOR P1, DirectX::XMVECTOR P2, DirectX::XMVECTOR P3)
	{
		v1 = P1;
		v2 = P2;
		v3 = P3;

		CalculateCenter();

	};
	inline void CalculateCenter()
	{
		using namespace DirectX;
		XMFLOAT3 result;

		result.x = (v1.m128_f32[0] + v2.m128_f32[0] + v3.m128_f32[0]) / 3;
		result.y = (v1.m128_f32[1] + v2.m128_f32[1] + v3.m128_f32[1]) / 3;
		result.z = (v1.m128_f32[2] + v2.m128_f32[2] + v3.m128_f32[2]) / 3;

		center[0] = (XMVectorGetX(v1) + XMVectorGetX(v2) + XMVectorGetX(v3)) / 3;
		center[1] = (XMVectorGetY(v1) + XMVectorGetY(v2) + XMVectorGetY(v3)) / 3;
		center[2] = (XMVectorGetZ(v1) + XMVectorGetZ(v2) + XMVectorGetZ(v3)) / 3;

		lowerCorner[0] = std::fmin(XMVectorGetX(v1), std::fmin(XMVectorGetX(v2), XMVectorGetX(v3)));
		lowerCorner[1] = std::fmin(XMVectorGetY(v1), std::fmin(XMVectorGetY(v2), XMVectorGetY(v3)));
		lowerCorner[2] = std::fmin(XMVectorGetZ(v1), std::fmin(XMVectorGetZ(v2), XMVectorGetZ(v3)));
		upperCorner[0] = std::fmax(XMVectorGetX(v1), std::fmax(XMVectorGetX(v2), XMVectorGetX(v3)));
		upperCorner[1] = std::fmax(XMVectorGetY(v1), std::fmax(XMVectorGetY(v2), XMVectorGetY(v3)));
		upperCorner[2] = std::fmax(XMVectorGetZ(v1), std::fmax(XMVectorGetZ(v2), XMVectorGetZ(v3)));

	};
};

struct Ray
{
	DirectX::XMVECTOR m_Origin;
	DirectX::XMVECTOR m_Direction;

	inline Ray(DirectX::XMVECTOR origin, DirectX::XMVECTOR direction)
	{
		m_Origin = origin;
		m_Direction = direction;
	};
};
