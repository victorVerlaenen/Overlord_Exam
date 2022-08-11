#include "stdafx.h"
#include "CharacterHUD.h"
#include "Prefabs/Player/Player.h"

CharacterHUD::CharacterHUD(std::vector<Player*> pTeam)
	:m_PlayerSelectLocations{ {-140.f, 0.f, .9f}, {0.f, 0.f, .9f}, {130.f, 0.f, .9f} }
	, m_pFont(ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt"))
	, m_pTeam(pTeam)
{
}

CharacterHUD::~CharacterHUD()
{
	
}

void CharacterHUD::NextPlayerActivate(const std::vector<Player*>& pStillActivePlayers)
{
	do
	{
		if (m_SelectedPlayer == 2)
		{
			m_SelectedPlayer = 0;
			m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[0]);
		}
		else
		{
			m_SelectedPlayer++;
			m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[m_SelectedPlayer]);
		}
	} while (std::ranges::find(pStillActivePlayers, m_pTeam[m_SelectedPlayer]) != pStillActivePlayers.end() == false);
}

void CharacterHUD::PreviousPlayerActivate(const std::vector<Player*>& pStillActivePlayers)
{
	do
	{
		if (m_SelectedPlayer == 0)
		{
			m_SelectedPlayer = 2;
			m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[2]);
		}
		else
		{
			m_SelectedPlayer--;
			m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[m_SelectedPlayer]);
		}
	} while (std::ranges::find(pStillActivePlayers, m_pTeam[m_SelectedPlayer]) != pStillActivePlayers.end() == false);
}

void CharacterHUD::SetActivePlayer(Player* pActivePlayer)
{
	for (int i{}; i < m_pTeam.size(); ++i)
	{
		if (m_pTeam[i] == pActivePlayer)
		{
			m_SelectedPlayer = i;
			m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[i]);
			return;
		}
	}
}

void CharacterHUD::ShowDeadTimer(const std::pair<Player*, float>& pKilledPlayer)
{
	for (int i{}; i < m_pTeam.size(); ++i)
	{
		if (m_pTeam[i] == pKilledPlayer.first)
		{
			m_pRespawnTimers.emplace_back(std::pair{ m_PlayerSelectLocations[i], pKilledPlayer.second });
		}
	}
}

void CharacterHUD::Initialize(const SceneContext& /*sceneContext*/)
{
	AddComponent(new SpriteComponent(L"Textures/Character_HUD.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	GetTransform()->Scale(.5f, .5f, 1);

	m_pPlayerSelectCircle = AddChild(new GameObject());
	m_pPlayerSelectCircle->GetTransform()->Translate(m_PlayerSelectLocations[0]);
	m_pPlayerSelectCircle->AddComponent(new SpriteComponent(L"Textures/Aura.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	m_pPlayerSelectCircle->GetTransform()->Scale(.15f, .15f, 1);
}

void CharacterHUD::Update(const SceneContext& /*sceneContext*/)
{

}

void CharacterHUD::Draw(const SceneContext& sceneContext)
{
	for (auto& timer : m_pRespawnTimers)
	{
		timer.second -= sceneContext.pGameTime->GetElapsed();
		timer.second = std::clamp(timer.second, 0.f, 7.f);
		if (timer.second > 0.f)
		{
			//std::wcout << std::to_wstring(timer.second) << std::endl;
			//TextRenderer::Get()->DrawText(m_pFont, std::to_wstring(timer.second), { timer.first.x, timer.first.y }, XMFLOAT4{ Colors::White });
			TextRenderer::Get()->DrawText(m_pFont, std::to_wstring(static_cast<int>(timer.second)), { GetTransform()->GetPosition().x + (timer.first.x / 2.f) - 10.f, GetTransform()->GetPosition().y - 18.f }, XMFLOAT4{ Colors::White });
		}
		else
		{
			//m_pRespawnTimers.erase(std::ranges::remove(m_pRespawnTimers, timer).begin(), m_pRespawnTimers.end());
		}
	}
}
