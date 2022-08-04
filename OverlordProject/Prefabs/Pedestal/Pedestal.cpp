#include "stdafx.h"
#include "Pedestal.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Prefabs/Player/Player.h"
#include "Scenes/Pyre/PyreScene.h"

Pedestal::Pedestal(const XMFLOAT2& healthPosition, bool isTeamOne)
	:m_pFont(ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt"))
	, m_Text(std::to_wstring(m_Health))
	, m_HealthPosition(healthPosition)
	, m_IsTeamOne(isTeamOne)
{

}

void Pedestal::Initialize(const SceneContext& /*sceneContext*/)
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	const auto pPedestalMesh = AddComponent(new ModelComponent(L"Meshes/Pedestal.ovm"));

	const auto pPedestalActor = AddComponent(new RigidBodyComponent(true));
	const auto pPedestalTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Pedestal.ovpt");
	pPedestalActor->AddCollider(PxTriangleMeshGeometry(pPedestalTriangleMesh, PxMeshScale({ 2.f, 2.f, 2.f })), *pDefaultMaterial);
	pPedestalActor->AddCollider(PxSphereGeometry(1.8f), *pDefaultMaterial, true);
	SetOnTriggerCallBack(std::bind(&Pedestal::Score, this, std::placeholders::_2, std::placeholders::_3));

	const auto pPedestalMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pPedestalMaterial->SetDiffuseTexture(L"Textures/Pedestal/Pedestal_MAT_BaseColor.png");

	pPedestalMesh->SetMaterial(pPedestalMaterial);
}

void Pedestal::Update(const SceneContext& /*sceneContext*/)
{

}

void Pedestal::Draw(const SceneContext& /*sceneContext*/)
{
	TextRenderer::Get()->DrawText(m_pFont, m_Text, m_HealthPosition, XMFLOAT4{ Colors::White });
}

void Pedestal::Score(GameObject* pPlayerObject, PxTriggerAction triggerAction)
{
	if (pPlayerObject->GetTag() == L"Player")
	{
		if (triggerAction == PxTriggerAction::ENTER)
		{
			auto pPlayer = dynamic_cast<Player*>(pPlayerObject);
			//-15 health
			if (pPlayer->HasOrb() && pPlayer->IsTeamOne() == m_IsTeamOne)
			{
				m_Health -= 15;
				m_Text = std::to_wstring(m_Health);

				//reset ball and players
				auto pPyreScene = dynamic_cast<PyreScene*>(GetScene());
				pPyreScene->GoalReset();
			}
			
			//Disable person till next goal
		}
	}
}
