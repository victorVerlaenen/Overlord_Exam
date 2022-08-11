#include "stdafx.h"
#include "Aura.h"

#include "Materials/DiffuseMaterial/DiffuseMaterial.h"
#include "Prefabs/Team/Team.h"
#include "Prefabs/Player/Player.h"

Aura::Aura(const XMFLOAT3& position, float radius)
	:m_Position(position)
	,m_Radius(radius)
{
}

void Aura::Initialize(const SceneContext& /*sceneContext*/)
{
	SetTag(L"Aura");
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	GetTransform()->Translate(m_Position);
	GetTransform()->Scale(m_Radius);
	const auto pModel = AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	const auto pAuraMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pAuraMaterial->SetDiffuseTexture(static_cast<Team*>(GetParent()->GetParent())->GetAuraTexturePath());
	pModel->SetMaterial(pAuraMaterial);

	m_pColliderObject = AddChild(new GameObject());
	m_pRigidBody = m_pColliderObject->AddComponent(new RigidBodyComponent(false));
	m_pRigidBody->AddCollider(PxSphereGeometry(m_Radius * 4.5f), *pDefaultMaterial, true);
	m_pColliderObject->SetOnTriggerCallBack(std::bind(&Aura::KillPlayer, this, std::placeholders::_2, std::placeholders::_3));
}

void Aura::Update(const SceneContext& /*sceneContext*/)
{
	const auto pParentPosition = GetParent()->GetTransform()->GetPosition();
	m_pColliderObject->GetTransform()->Translate(m_Position.x + pParentPosition.x, m_Position.y + pParentPosition.y, m_Position.z + pParentPosition.z);
}

void Aura::KillPlayer(GameObject* pPlayerObject, PxTriggerAction triggerAction)
{
	if (pPlayerObject->GetTag() == L"Player")
	{
		if (triggerAction == PxTriggerAction::ENTER)
		{
			const auto pPlayer = dynamic_cast<Player*>(pPlayerObject);
			if(pPlayer->GetRespawned() == true)
			{
				return;
			}
			if(dynamic_cast<Team*>(pPlayerObject->GetParent())->GetTeamNumber() != static_cast<Team*>(GetParent()->GetParent())->GetTeamNumber())
			{
				//std::cout << "Kill" << std::endl;
				dynamic_cast<Team*>(pPlayerObject->GetParent())->Kill(pPlayer);
			}
		}
	}
}