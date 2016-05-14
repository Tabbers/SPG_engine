#include "d3dcamera.h"
#include "globaldefinitions.h"

D3DCamera::D3DCamera()
{
	m_position = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	m_rotation = XMQuaternionIdentity();
}

D3DCamera::~D3DCamera()
{
}

void D3DCamera::Init(int screenWidth, int screenHeight, ID3D11DeviceContext* devCon, XMVECTOR position, XMVECTOR rotation)
{
	// Setup the viewport
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_position = position;
	m_rotation = rotation;

	devCon->RSSetViewports(1, &m_viewport);

	// Set up fov to 90°
	m_fieldOfView = PI/4.0f;
	//get Apsect ratio
	m_screenAspect = (float)screenWidth / (float)screenHeight;

	//Set the projection matrix
	m_projectionMatrix = XMMatrixPerspectiveFovLH(m_fieldOfView, m_screenAspect, SCREEN_NEAR, SCREEN_DEPTH);

	//OrthoMatrix
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

}

void D3DCamera::Render(XMVECTOR translate, XMVECTOR rotate, bool move, bool playback)
{
	XMVECTOR upVector, lookAtVector, rightVector;
	XMMATRIX rotationMatrix;

	rightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	lookAtVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	if (move)
	{
		XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(rotate);
		quaternion = XMQuaternionNormalize(quaternion);
		m_rotation = XMQuaternionMultiply(quaternion, m_rotation);
	}
	if (playback) m_rotation = rotate;
	rotationMatrix = XMMatrixRotationQuaternion(m_rotation);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);
	rightVector = XMVector3TransformCoord(upVector, rotationMatrix);
	if (move)
	{
		translate = XMVector3TransformCoord(translate, rotationMatrix);
		translate = XMVectorSetW(translate, 0);
		m_position += translate;
	}
	if (playback) m_position = translate;

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(m_position, lookAtVector);

	m_lookAtVector.x = lookAtVector.m128_f32[0];
	m_lookAtVector.y = lookAtVector.m128_f32[1];
	m_lookAtVector.z = lookAtVector.m128_f32[2];

	
	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(m_position, lookAtVector, upVector);
	return;
}
void D3DCamera::ResetViewport(ID3D11DeviceContext* devCon)
{
	devCon->RSSetViewports(1, &m_viewport);
}
