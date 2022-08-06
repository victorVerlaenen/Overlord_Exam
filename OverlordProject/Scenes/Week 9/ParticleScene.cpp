#include "stdafx.h"
#include "ParticleScene.h"

#include "Materials/ColorMaterial.h"

void ParticleScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
	//Particle System
	ParticleEmitterSettings settings{};
	/*settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,1.f,1.f, .6f };*/

	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 5.f;
	settings.maxSize = 6.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = -5.f;
	settings.maxScale = -6.0f;
	settings.minEmitterRadius = .5f;
	settings.maxEmitterRadius = .7f;
	settings.color = { 0.9411764f,0.3607843f,0.0705882f, .6f };

	const auto pObject = AddChild(new GameObject);
	m_pEmitter = pObject->AddComponent(new ParticleEmitterComponent(L"Textures/Fire.png", settings, 200));

	//Teapot
	m_pTeapot = AddChild(new GameObject());
	const auto pModel = m_pTeapot->AddComponent(new ModelComponent(L"Meshes/Teapot.ovm"));

	const auto pMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pMaterial->SetColor(XMFLOAT4{ Colors::White });
	pModel->SetMaterial(pMaterial);
	m_pTeapot->GetTransform()->Scale(.3f, .3f, .3f);

	m_pEmitter->GetTransform()->Translate(10, 0, 0);
}

void ParticleScene::Update()
{
	
}

void ParticleScene::OnGUI()
{
	ImGui::Checkbox("Auto Move", &m_AutoMove);
	m_pEmitter->DrawImGui();
}
