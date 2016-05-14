#include "graphicscore.h"
#include "modellibrary.h"
#include "d3dmodel.h"
#include "d3dmath.h"
#include "d3dcamera.h"
#include "d3drendertotexture.h"
#include "input.h"
#include "path.h"
#include "gameobject.h"
#include "light.h"
#include "colorshader.h"
#include "depthShader.h"
#include "lineShader.h"
#include "generationShader.h"
#include "kdTree.h"
#include "line.h"
#include "ParticleSystem.h"
#include "generatedModel.h"
#include <string>
#include <windef.h>
#include <iostream>

GraphicsCore::GraphicsCore() :
	m_path(nullptr), m_Direct3DWrapper(nullptr), m_Camera(nullptr),
	m_colShader(nullptr), m_depthShader(nullptr), m_lineShader(nullptr), m_Light(nullptr),
	m_kdtree(nullptr),m_modelLib(nullptr)
{
}

GraphicsCore::~GraphicsCore()
{
	delete m_colShader;
	delete m_depthShader;
	delete m_lineShader;
	delete m_path;
	delete m_Camera;
	delete m_Light;
	delete m_Direct3DWrapper;
	delete m_kdtree;
	m_collPoints.clear();
	if (m_particleSystems.size() > 0)
	{
		for each (ParticleSystem* sys in m_particleSystems)
		{
			delete sys;
		}
	}
	if (m_renderable.size() > 0)
	{
		for each (D3Dmodel* model in m_renderable)
		{
			delete model;
		}
	}
	if (m_renderableLines.size() > 0)
	{
		for each (Line* line in m_renderableLines)
		{
			delete line;
		}
	}
	return;
}

bool GraphicsCore::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	m_hwndin = hwnd;
	XMVECTOR tempPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR tempRot = XMQuaternionRotationRollPitchYaw(0, 0, 0);
	D3Dmodel* m_Model = nullptr;
	D3Dmodel* m_Model1 = nullptr;
	D3Dmodel* m_Model2 = nullptr;
	D3Dmodel* m_Model3 = nullptr;
	D3Dmodel* m_Model4 = nullptr;
	D3Dmodel* m_Model5 = nullptr;
	D3Dmodel* m_Model6 = nullptr;
	D3Dmodel* m_Model7 = nullptr;
	D3Dmodel* m_Model8 = nullptr;
	D3Dmodel* m_Model9 = nullptr;
	D3Dmodel* m_Model10 = nullptr;
	ParticleSystem* ps = nullptr;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_Direct3DWrapper = new D3Dc();
	if (!m_Direct3DWrapper) return false;

	result = m_Direct3DWrapper->Init(screenWidth, screenHeight, VSYNC, hwnd, false);
	if (!result) return false;

	m_modelLib = new ModelLibrary();

	m_genModel = new GeneratedModel();
	m_genModel->Init('2', m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext());

	tempPos = XMVectorSet(-4.0f, 0.0f, -2.0f, 0.0f);
	ps = new ParticleSystem();
	ps->Init(m_Direct3DWrapper->GetDevice(),m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	m_particleSystems.push_back(ps);

	tempPos = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	m_Camera = new D3DCamera();
	if (!m_Camera) return false;
	m_Camera->Init(screenWidth, screenHeight, m_Direct3DWrapper->GetDeviceContext(),tempPos,tempRot);

	tempPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_Model = new D3Dmodel(m_modelLib);
	if (!m_Model) return false;
	result = m_Model->Init("Data/sht/cube.sht",L"",L"", L"", m_Direct3DWrapper->GetDevice(),m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	if (!result) return false;
	m_renderable.push_back(m_Model);

	tempPos = XMVectorSet(5.0f, 5.0f, 0.0f, 0.0f);
	m_Model1 = new D3Dmodel(m_modelLib);
	if (!m_Model1) return false;
	result = m_Model1->Init("Data/sht/cube.sht",L"", L"", L"", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(),tempPos,tempRot);
	if (!result) return false;
	m_renderable.push_back(m_Model1);

	tempPos = XMVectorSet(5.0f, 0.0f, 0.0f, 0.0f);
	m_Model2 = new D3Dmodel(m_modelLib);
	if (!m_Model2) return false;
	result = m_Model2->Init("Data/sht/cube.sht", L"Texture/brick/text_brick.dds", L"Texture/brick/norm_brick.dds", L"Texture/brick/disp_brick.dds", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(),tempPos,tempRot);
	if (!result) return false;
	m_renderable.push_back(m_Model2);

	tempPos = XMVectorSet(0.0f,-5.0f, 0.0f, 0.0f);
	m_Model3 = new D3Dmodel(m_modelLib);
	if (!m_Model3) return false;
	result = m_Model3->Init("Data/sht/plane.sht", L"Texture/stones/text_ground.dds", L"Texture/stones/norm_ground.dds", L"Texture/stones/disp_ground.dds", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	m_Model3->SetScale(10,1,10);
	if (!result) return false;
	m_renderable.push_back(m_Model3);

	tempPos = XMVectorSet(0.0f, 10.0f, -5.0f, 0.0f);
	tempRot = XMQuaternionRotationRollPitchYaw(0,0,0);

	m_Light = new Light();
	if (!m_Light) return false;
	m_Light->Init(SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, m_Direct3DWrapper->GetDeviceContext(),tempPos,tempRot);
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	m_Model4 = new D3Dmodel(m_modelLib);
	if (!m_Model4) return false;
	result = m_Model4->Init("Data/sht/cube.sht",L"",L"", L"", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), m_Light->GetPosition(), m_Light->GetRotation());
	m_Model4->SetScale(0.5,0.5,0.5);
	if (!result) return false;
	m_Model4->SetRenderOnShadowMap(false);
	m_renderable.push_back(m_Model4);

	tempPos = XMVectorSet(10.0f, 10.0f, 10.0f, 0.0f);
	m_Model5 = new D3Dmodel(m_modelLib);
	if (!m_Model5) return false;
	result = m_Model5->Init("Data/sht/sphere.sht", L"", L"", L"", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	m_Model5->SetScale(3, 3, 3);
	if (!result) return false;
	m_renderable.push_back(m_Model5);

	tempPos = XMVectorSet(10.0f, 30.0f, 10.0f, 0.0f);
	m_Model6 = new D3Dmodel(m_modelLib);
	if (!m_Model6) return false;
	result = m_Model6->Init("Data/sht/sphere.sht", L"", L"", L"", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	m_Model6->SetScale(3, 3, 3);
	if (!result) return false;
	m_renderable.push_back(m_Model6);

	tempPos = XMVectorSet(10.0f, 30.0f, 10.0f, 0.0f);
	m_Model7 = new D3Dmodel(m_modelLib);
	if (!m_Model7) return false;
	result = m_Model7->Init("Data/sht/sphere.sht", L"", L"", L"", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	m_Model7->SetScale(8, 8, 8);
	if (!result) return false;
	m_renderable.push_back(m_Model7);

	tempPos = XMVectorSet(70.0f, 30.0f, 55.0f, 0.0f);
	m_Model9 = new D3Dmodel(m_modelLib);
	if (!m_Model9) return false;
	result = m_Model9->Init("Data/sht/cube.sht", L"", L"", L"", m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
	m_Model9->SetScale(3, 3, 3);
	if (!result) return false;
	m_renderable.push_back(m_Model9);
	m_RenderTexture = new D3DRenderToTexture();
	if (!m_RenderTexture) return false;
	result = m_RenderTexture->Init(m_Direct3DWrapper->GetDevice(),SHADOWMAP_WIDTH,SHADOWMAP_HEIGHT,
	m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()], m_Direct3DWrapper->GetMSAASettings().QualityLevel[m_Direct3DWrapper->GetMSAA()], DXGI_FORMAT_R32G32B32A32_FLOAT);

	if (!result) return false;

	m_kdtree = new KdTree();

	//Shader Initialisation
	m_depthShader = new DepthShader();
	if (!m_depthShader) return false;
	result = m_depthShader->Init(m_Direct3DWrapper->GetDevice(), hwnd);
	if (!result) return false;

	m_colShader = new ColorShader();
	if (!m_colShader) return false;
	result = m_colShader->Init(m_Direct3DWrapper->GetDevice(), hwnd);
	if (!result) return false;

	m_lineShader = new LineShader();
	if (!m_lineShader) return false;
	result = m_lineShader->Init(m_Direct3DWrapper->GetDevice(), hwnd);
	if (!result) return false;

	m_genShader = new GenerationShader();
	if (FAILED(m_genShader->Init(m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext())))
		return false;
	
	m_path = new Path();
	if (!m_path) return false;

	m_msaaText = new D3DRenderToTexture();
	m_msaaText->Init(m_Direct3DWrapper->GetDevice(), screenWidth, screenHeight,
	m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()], m_Direct3DWrapper->GetMSAASettings().QualityLevel[m_Direct3DWrapper->GetMSAA()], DXGI_FORMAT_R8G8B8A8_UNORM);

	int i = 0;
	for each (D3Dmodel* model in m_renderable)
	{
		model->Render(m_Direct3DWrapper->GetDeviceContext());
		XMMATRIX worldMatrixE;
		m_Direct3DWrapper->GetWorldMatrix(worldMatrixE);
		XMMATRIX worldMatrix = model->adjustWorldmatrix(worldMatrixE);
		if(i!=3) m_kdtree->coll->Triangles(model, worldMatrix); 
		i++;
	}
	m_kdtree->Init(&(m_kdtree->coll->GetTrianglesAsPointers()), m_Direct3DWrapper->GetDevice(), {200,128,0});
	std::cout<<std::to_string(m_kdtree->coll->GetTriangles().size())+" :Traingles in Scene \n";

	return true;
}

bool GraphicsCore::Frame(float delta_time, Input* inKey, bool Editmode)
{
	bool result;

	result = Render(delta_time, inKey, Editmode);
	if (!result) return false;

	return true;
}

bool GraphicsCore::Render(float delta_time, Input* inKey, bool Editmode)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	XMVECTOR translateC = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotateC = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR translateL = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR rotateL = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	if (inKey->Keystate('1') && !inKey->KeystateOld('1')) m = Modes::CAMERA;
	if (inKey->Keystate('2') && !inKey->KeystateOld('2')) m = Modes::LIGHT;
	if (Editmode)
	{
		
		m_Camera->speedMovement = 10.0f;
		m_Camera->speedRotation = 1.0f;
		if (m == Modes::CAMERA)
		{
			if (inKey->Keystate('W'))	
				rotateC = XMVectorSetX(rotateC, m_Camera->speedRotation*delta_time);
			if (inKey->Keystate('S'))	
				rotateC = XMVectorSetX(rotateC, -1 * m_Camera->speedRotation*delta_time);
			if (inKey->Keystate('A'))	
				rotateC = XMVectorSetY(rotateC, -1 * m_Camera->speedRotation*delta_time);
			if (inKey->Keystate('D'))	
				rotateC = XMVectorSetY(rotateC, m_Camera->speedRotation*delta_time);
			if (inKey->Keystate('Q'))	
				rotateC = XMVectorSetZ(rotateC, m_Camera->speedRotation*delta_time);
			if (inKey->Keystate('E'))	
				rotateC = XMVectorSetZ(rotateC, -1 * m_Camera->speedRotation*delta_time);

			// Up
			if (inKey->Keystate(VK_UP))		
				translateC = XMVectorSetZ(translateC, m_Camera->speedMovement*delta_time);
			// Down
			if (inKey->Keystate(VK_DOWN))		
				translateC = XMVectorSetZ(translateC, -1 * m_Camera->speedMovement*delta_time);
			//Left
			if (inKey->Keystate(VK_LEFT))		
				translateC = XMVectorSetX(translateC, -1 * m_Camera->speedMovement*delta_time);
			//Right
			if (inKey->Keystate(VK_RIGHT))		
				translateC = XMVectorSetX(translateC, m_Camera->speedMovement*delta_time);
			//Up
			if (inKey->Keystate(VK_SHIFT))
				translateC = XMVectorSetY(translateC, m_Camera->speedMovement*delta_time);
			//Down									 
			if (inKey->Keystate(VK_CONTROL))
				translateC = XMVectorSetY(translateC, -1 * m_Camera->speedMovement*delta_time);
			// Set point using camera Rotation and Position;
			if (inKey->Keystate(VK_SPACE) && !inKey->KeystateOld(VK_SPACE)) m_path->AddPoint(m_Camera->GetPosition(), m_Camera->GetRotation());
		}
		if (m == Modes::LIGHT)
		{
			if (inKey->Keystate('W'))	
				rotateL = XMVectorSetX(rotateL, m_Light->speedRotation*delta_time);
			if (inKey->Keystate('S'))	
				rotateL = XMVectorSetX(rotateL, -1 * m_Light->speedRotation*delta_time);
			if (inKey->Keystate('A'))	
				rotateL = XMVectorSetY(rotateL, -1 * m_Light->speedRotation*delta_time);
			if (inKey->Keystate('D'))	
				rotateL = XMVectorSetY(rotateL, m_Light->speedRotation*delta_time);
			if (inKey->Keystate('Q'))	
				rotateL = XMVectorSetZ(rotateL, m_Light->speedRotation*delta_time);
			if (inKey->Keystate('E'))	
				rotateL = XMVectorSetZ(rotateL, -1 * m_Light->speedRotation*delta_time);

			// Forwards
			if (inKey->Keystate(VK_UP))		
				translateL = XMVectorSetZ(translateL, m_Light->speedMovement*delta_time);
			// Backwards									 
			if (inKey->Keystate(VK_DOWN))	
				translateL = XMVectorSetZ(translateL, -1 * m_Light->speedMovement*delta_time);
			//Left									 
			if (inKey->Keystate(VK_LEFT))	
				translateL = XMVectorSetX(translateL, -1 * m_Light->speedMovement*delta_time);
			//Right									 
			if (inKey->Keystate(VK_RIGHT))	
				translateL = XMVectorSetX(translateL, m_Light->speedMovement*delta_time);
			//Up
			if (inKey->Keystate(VK_SHIFT))	
				translateL = XMVectorSetY(translateL,  m_Light->speedMovement*delta_time);
			//Down									 
			if (inKey->Keystate(VK_CONTROL))	
				translateL = XMVectorSetY(translateL, -1 * m_Light->speedMovement*delta_time);
		}
		if (inKey->lMousestate() && !inKey->lMousestateOld() && m_hwndin == GetActiveWindow())
		{
			inKey->mousePressed = true;
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(GetActiveWindow(), &p);
			inKey->SetMouseCoord(p.x, p.y);
		}
		if (inKey->Keystate(VK_PRIOR) && !inKey->KeystateOld(VK_PRIOR))
		{
			int temp = m_colShader->GetFilter();
			if (temp < 9) temp++;
			else temp = 0;
			m_colShader->SetFilter(temp);
			SetWindowNameOnFilterChange();
		}
		if (inKey->Keystate(VK_NEXT) && !inKey->KeystateOld(VK_NEXT))
		{
			int temp = m_colShader->GetFilter();
			if (temp > 0) temp--;
			else temp = 8;
			m_colShader->SetFilter(temp);
			SetWindowNameOnFilterChange();
		}
		if (inKey->Keystate(VK_OEM_PLUS) && !inKey->KeystateOld(VK_OEM_PLUS))
		{
			int temp = m_colShader->GetMip();
			if (temp < 9) temp++;
			else temp = 0;
			m_colShader->SetMip(temp);

		}
		if (inKey->Keystate(VK_OEM_MINUS) && !inKey->KeystateOld(VK_OEM_MINUS))
		{
			int temp = m_colShader->GetMip();
			if (temp > 0) temp--;
			else temp = 9;
			m_colShader->SetMip(temp);

		}
		if (inKey->Keystate('K') && !inKey->KeystateOld('K'))
		{
			if (m_displayKd) m_displayKd = false;
			else m_displayKd = true;
		}
		if (inKey->Keystate(VK_OEM_COMMA) && !inKey->KeystateOld(VK_OEM_COMMA))
		{
			int msaa = m_Direct3DWrapper->GetMSAA();
			if (msaa < m_Direct3DWrapper->GetMSAASettings().QualityLevel.size()-1)
				msaa++;
			else msaa = 0;
			m_Direct3DWrapper->SetMSAA(msaa);
			
			delete(m_RenderTexture);
			m_RenderTexture = new D3DRenderToTexture();
			if (!m_RenderTexture) return false;
			result = m_RenderTexture->Init(m_Direct3DWrapper->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
				m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()], m_Direct3DWrapper->GetMSAASettings().QualityLevel[m_Direct3DWrapper->GetMSAA()], DXGI_FORMAT_R32G32B32A32_FLOAT);
			if (!result) return false;

			delete(m_msaaText);
			m_msaaText = new D3DRenderToTexture();
			m_msaaText->Init(m_Direct3DWrapper->GetDevice(),m_screenWidth,m_screenHeight,
				m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()], m_Direct3DWrapper->GetMSAASettings().QualityLevel[m_Direct3DWrapper->GetMSAA()], DXGI_FORMAT_R8G8B8A8_UNORM);
			SetWindowNameOnFilterChange();
		}
		if (inKey->Keystate('N') && !inKey->KeystateOld('N'))
		{
			for each(D3Dmodel* model in m_renderable)
			{
				if (model->GetDrawNormalMap())
				{
					model->SetDrawNormalMap(false);
				}
				else
				{
					model->SetDrawNormalMap(true);
				}
			}
		}
		if (inKey->Keystate('P') && !inKey->KeystateOld('P'))
		{
			for each(D3Dmodel* model in m_renderable)
			{
				if (model->GetDrawSpec())
				{
					model->SetDrawSpec(false);
				}
				else
				{
					model->SetDrawSpec(true);
				}
			}
		}
		if (inKey->Keystate('L') && !inKey->KeystateOld('L'))
		{
			for each(D3Dmodel* model in m_renderable)
			{
				if (model->GetDrawDisp())
				{
					model->SetDrawDisp(false);
				}
				else
				{
					model->SetDrawDisp(true);
				}
			}
		}
		// set number of iterations of RayMarching
		if (m_colShader != nullptr)
		{
			if (inKey->Keystate('0') && !inKey->KeystateOld('0'))
			{
				int it = m_colShader->GetNumberOfIterations();
				if(it < 30)
					it++;
				m_colShader->SetNumberOfIterations(it);

				std::cout << "Displacement Number of Iterations: " << it <<std::endl;
			}
			if (inKey->Keystate('9') && !inKey->KeystateOld('9'))
			{
				int it = m_colShader->GetNumberOfIterations();
				if (it > 0)
					it--;
				m_colShader->SetNumberOfIterations(it);
				std::cout << "Displacement Number of Iterations: " << it << std::endl;
			}
			if (inKey->Keystate('8') && !inKey->KeystateOld('8'))
			{
				int it = m_colShader->GetNumberOfIterationsRefined();
				if (it < 10)
					it++;
				m_colShader->SetNumberOfIterationsRefined(it);

				std::cout << "Displacement Number of Refinement Iterations: " << it << std::endl;
			}
			if (inKey->Keystate('7') && !inKey->KeystateOld('7'))
			{
				int it = m_colShader->GetNumberOfIterationsRefined();
				if (it > 0)
					it--;
				m_colShader->SetNumberOfIterationsRefined(it);
				std::cout << "Displacement Number of Refinement Iterations: " << it << std::endl;
			}
		}
	}
	else
	{
		if (inKey->Keystate(VK_SPACE) && !inKey->KeystateOld(VK_SPACE))
		{
			if (m_path->GetPathSize() < 3)
			{
				MessageBox(m_hwndin, "You have to enter at least 4 Points", "Error", MB_OK);
				return false;
			}
			if (!m_playback)
			{
				// INterpolation setup
				m_playback = true;
				m_Camera->speedMovement = 500.0f;
				m_elapsedTime = 0;
				m_currentPoint = 0;
				m_sublength = XMVector4Length(m_path->GetPositionOfPoint(0) + m_path->GetPositionOfPoint(1)).m128_f32[0];
				m_partialTime = m_sublength / (m_Camera->speedMovement*delta_time);
			}
			else
			{
				m_playback = false;
			}

		}
		if (m_playback)
		{
			float t;
			if (m_elapsedTime > m_partialTime)
			{
				if (m_currentPoint < m_path->GetPathSize() - 2)
				{
					// Setting up for interpolation to the next point in the path
					m_currentPoint++;
					m_elapsedTime = 0;
					m_sublength = XMVector4Length(m_path->GetPositionOfPoint(m_currentPoint) + m_path->GetPositionOfPoint(m_currentPoint + 1)).m128_f32[0];
					m_partialTime = m_sublength / (m_Camera->speedMovement*delta_time);
				}
				else
				{
					m_playback = false;
				}
			}
			t = m_elapsedTime / m_partialTime;
			// If currentPoint is the starting point correct the interpolation and set currentPoint -1 to the currentPoint
			if (m_currentPoint == 0)
			{
				translateC = D3DMath::KochanekBartels(t, 0, 0, 0, m_path->GetPositionOfPoint(m_currentPoint), m_path->GetPositionOfPoint(m_currentPoint), m_path->GetPositionOfPoint(m_currentPoint + 1), m_path->GetPositionOfPoint(m_currentPoint + 2));
				rotateC = D3DMath::Squad(t, m_path->GetRotationOfPoint(m_currentPoint), m_path->GetRotationOfPoint(m_currentPoint), m_path->GetRotationOfPoint(m_currentPoint + 1), m_path->GetRotationOfPoint(m_currentPoint + 2));
			}
			else if (m_currentPoint == m_path->GetPathSize() - 2)
			{
				translateC = D3DMath::KochanekBartels(t, 0, 0, 0, m_path->GetPositionOfPoint(m_currentPoint - 1), m_path->GetPositionOfPoint(m_currentPoint), m_path->GetPositionOfPoint(m_currentPoint + 1), m_path->GetPositionOfPoint(m_currentPoint + 1));
				rotateC = D3DMath::Squad(t, m_path->GetRotationOfPoint(m_currentPoint - 1), m_path->GetRotationOfPoint(m_currentPoint), m_path->GetRotationOfPoint(m_currentPoint + 1), m_path->GetRotationOfPoint(m_currentPoint + 1));
			}
			else
			{
				translateC = D3DMath::KochanekBartels(t, 0, 0, 0, m_path->GetPositionOfPoint(m_currentPoint - 1), m_path->GetPositionOfPoint(m_currentPoint), m_path->GetPositionOfPoint(m_currentPoint + 1), m_path->GetPositionOfPoint(m_currentPoint + 2));
				rotateC = D3DMath::Squad(t, m_path->GetRotationOfPoint(m_currentPoint - 1), m_path->GetRotationOfPoint(m_currentPoint), m_path->GetRotationOfPoint(m_currentPoint + 1), m_path->GetRotationOfPoint(m_currentPoint + 2));
			}
		}
		m_elapsedTime += delta_time;
	}

	result = RenderTexture(Editmode, translateL, rotateL);
	
	if (m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()] > 1)
	{
		m_msaaText->ClearRenderTarget(m_Direct3DWrapper->GetDeviceContext(),BACKGROUNDCOLOR);
		m_msaaText->SetRenderTarget(m_Direct3DWrapper->GetDeviceContext());
		
	}
	if (!result) return false;
	m_Camera->ResetViewport(m_Direct3DWrapper->GetDeviceContext());
	m_Direct3DWrapper->BeginScene(BACKGROUNDCOLOR);

	m_Camera->Render(translateC, rotateC, Editmode, m_playback);
	m_Light->Render(translateL, rotateL, Editmode);

	m_Direct3DWrapper->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Camera->GetProjectionMatrix(projectionMatrix);

	MatricesGen mg(worldMatrix, viewMatrix, projectionMatrix, m_Light->GetPosition());
	Matrices sceneInfo(worldMatrix, viewMatrix, projectionMatrix,m_Camera->GetPosition());
	LightData lightInfo(m_Light->GetViewMatrix(), m_Light->GetProjectionMatrix(), m_Light->GetAmbientColor(), 
						m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), m_Light->GetSpecularIntensity(), 
						m_Light->GetXMFLOAT3Position(),m_Light->GetXMFLOAT3Rotation(),m_Camera->GetLookAt());

	m_renderable[4]->SetPosition(m_Light->GetPosition());
	m_renderable[4]->SetRotation(m_Light->GetRotation());

	m_genShader->Render(m_Direct3DWrapper->GetDeviceContext(), m_Direct3DWrapper->GetDevice(), mg, m_genModel);
	
	for each(Line* line in 	m_conLines)
	{
		line->Render(m_Direct3DWrapper->GetDeviceContext());
		result = m_lineShader->Render(m_Direct3DWrapper->GetDeviceContext(), m_Direct3DWrapper->GetDevice(), line->GetIndexCount(), sceneInfo);
	}

	for each(Line* line in m_renderableLines)
	{
		line->Render(m_Direct3DWrapper->GetDeviceContext());
		result = m_lineShader->Render(m_Direct3DWrapper->GetDeviceContext(), m_Direct3DWrapper->GetDevice(), line->GetIndexCount(), sceneInfo);
	}
	if (m_displayKd)
	{
		m_kdtree->Render(m_Direct3DWrapper->GetDeviceContext());
		result = m_lineShader->Render(m_Direct3DWrapper->GetDeviceContext(), m_Direct3DWrapper->GetDevice(), m_kdtree->GetIndexCount(), sceneInfo);
	}
	for each (D3Dmodel* model in m_renderable)
	{
		sceneInfo.DrawNormal = model->GetDrawNormalMap();
		sceneInfo.DrawSpec   = model->GetDrawSpec();
		sceneInfo.DrawDisp = model->GetDrawDisp();
		model->Render(m_Direct3DWrapper->GetDeviceContext());
		sceneInfo.worldMatrix = model->adjustWorldmatrix(worldMatrix);
		result = m_colShader->Render(m_Direct3DWrapper->GetDeviceContext(),m_Direct3DWrapper->GetDevice(), model->GetIndexCount(), sceneInfo, lightInfo, m_RenderTexture->GetShaderRessourceView(), model->GetTexture()->GetResourceView(),model->GetNormalMap()->GetResourceView(), model->GetDisplacementMap()->GetResourceView());
		if (!result) return false;	
	}
	m_Direct3DWrapper->EnableAlphaBlending();
	for each (ParticleSystem* ps in m_particleSystems)
	{
		ps->Render(m_Direct3DWrapper->GetDeviceContext(),m_Direct3DWrapper->GetDevice(),delta_time,sceneInfo,m_Camera->GetPosition());
	}
	m_Direct3DWrapper->DisableAlphaBlending();
	if (inKey->mousePressed)
	{
		float length = 0;
		int mouseX, mouseY = 0;
		inKey->GetMouseCoord(mouseX, mouseY);
		Ray ray = m_kdtree->coll->MouseToWWorld(mouseX, mouseY, m_screenWidth, m_screenHeight, sceneInfo, m_Camera->GetPosition());
		//Collision detection
		m_kdtree->rayCast(m_kdtree->getRoot(),ray,length,RAYCASTDISTANCE,0);

		for (int i = 0; i<m_renderableLines.size(); ++i)
		{
			m_renderableLines.pop_back();
		}
		if (length > 0 && length < std::numeric_limits<float>::infinity())
		{
			OutputDebugStringA("Yes");
			int i = 0;
			int index = std::numeric_limits<float>::infinity();
			length = std::numeric_limits<float>::infinity();
			for each (Triangle tri in  m_kdtree->coll->GetTriangles())
			{
				if (tri.intersection)
				{
					if (tri.distance < length)
					{
						OutputDebugStringA(std::to_string(i).c_str());
						OutputDebugStringA("\n");
						length = tri.distance;
						index = i;
					}
					m_kdtree->coll->ResetTriangle(i);
				}
				i++;
			}
			if (index != std::numeric_limits<float>::infinity())
			{
				Triangle tri = m_kdtree->coll->GetTriangle(index);
				//Hit Trinagle Render
				Line* lineV1 = new Line();
				Line* lineV2 = new Line();
				Line* lineV3 = new Line();
				lineV1->Init(m_Direct3DWrapper->GetDevice(), tri.v1, tri.v2, { 0,255,0 });
				lineV2->Init(m_Direct3DWrapper->GetDevice(), tri.v2, tri.v3, { 0,255,0 });
				lineV3->Init(m_Direct3DWrapper->GetDevice(), tri.v3, tri.v1, { 0,255,0 });
				m_renderableLines.push_back(lineV1);
				m_renderableLines.push_back(lineV2);
				m_renderableLines.push_back(lineV3);

				std::cout << std::to_string(tri.distance) + " :Distance to picked Triangle \n";
				
				ParticleSystem* ps;
				DirectX::XMVECTOR tempPos = tri.PointIntersect;
				DirectX::XMVECTOR tempRot = DirectX::XMVectorSet(0.0f,0.0f,0.0f,0.0f);
				ps = new ParticleSystem();
				ps->Init(m_Direct3DWrapper->GetDevice(), m_Direct3DWrapper->GetDeviceContext(), tempPos, tempRot);
				m_particleSystems.push_back(ps);

				if (m_collPoints.size() < 2)
				{
					m_collPoints.push_back(tri.PointIntersect);
					if (m_collPoints.size() == 2)
					{
						Line* lineColl = new Line();
						lineColl->Init(m_Direct3DWrapper->GetDevice(), m_collPoints[0], m_collPoints[1], { 0,0,255 });
						m_conLines.push_back(lineColl);

						XMVECTOR delta = m_collPoints[0] - m_collPoints[1];
						float distance = fabs(delta.m128_f32[0]);
						std::cout << std::to_string(distance) + " :Distance between picked Triangles \n";
						for (int i = 0; m_collPoints.size(); ++i)
						{
							m_collPoints.pop_back();
						}
					}
				}
			}
		}
		else
		{
			OutputDebugStringA("No");
		}
		// Ray Render
		if (length == 0) length = RAYCASTDISTANCE;
		XMVECTOR end = ray.m_Origin + ray.m_Direction * length;
		Line* line = new Line();
		line->Init(m_Direct3DWrapper->GetDevice(), ray.m_Origin, end, { 255,0,0 });
		m_renderableLines.push_back(line);
	}
	
    if (m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()] > 1)
	{
		m_Direct3DWrapper->GetDeviceContext()->ResolveSubresource(
			m_Direct3DWrapper->GetBackBuffer(),
			0,
			m_msaaText->GetTexture(),
			0,
			DXGI_FORMAT_R8G8B8A8_UNORM
		);
	}
	m_Direct3DWrapper->EndScene();
	inKey->mousePressed = false;
	return true;
}

bool GraphicsCore::RenderTexture(bool Editmode, XMVECTOR translateL, XMVECTOR rotateL)
{
	using namespace DirectX;

	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;
	XMFLOAT3 pos;
	bool result = true;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_Direct3DWrapper->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_Direct3DWrapper->GetDeviceContext(), {0.0f,0.0f,0.0f,0.0f });
	
	m_Light->Render(translateL, rotateL, Editmode);

	m_Direct3DWrapper->GetWorldMatrix(worldMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	for each (D3Dmodel* model in m_renderable)
	{
		if (model->GetRenderOnShadowMap())
		{
			model->Render(m_Direct3DWrapper->GetDeviceContext());
			result = m_depthShader->Render(m_Direct3DWrapper->GetDeviceContext(), model->GetIndexCount(), model->adjustWorldmatrix(worldMatrix), lightViewMatrix, lightProjectionMatrix);
			if (!result) return false;
		}
	}
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3DWrapper->SetBackBufferRenderTarget();
	return result;
}

void GraphicsCore::SetWindowNameOnFilterChange()
{
	std::string composite = "";
	composite += m_Direct3DWrapper->GetMSAASettings().QualityLevel[m_Direct3DWrapper->GetMSAA()] + " :QualityLevel ";
	composite += m_Direct3DWrapper->GetMSAASettings().SampleCounts[m_Direct3DWrapper->GetMSAA()] + " :SampleCount -";
	switch (m_colShader->GetFilter())
	{
	case 0:
		composite += "D3D11_FILTER_MIN_MAG_MIP_POINT";
		break;
	case 1:
		composite += "D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR";
		break;
	case 2:
		composite += "D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT";
		break;
	case 3:
		composite += "D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR";
		break;
	case 4:
		composite += "D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT";
		break;
	case 5:
		composite += "D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR";
		break;
	case 6:
		composite += "D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT";
		break;
	case 7:
		composite += "D3D11_FILTER_MIN_MAG_MIP_LINEAR";
		break;
	case 8:
		composite += "D3D11_FILTER_ANISOTROPIC";
		break;
	}
	SetWindowText(GetActiveWindow(), composite.c_str());
}
