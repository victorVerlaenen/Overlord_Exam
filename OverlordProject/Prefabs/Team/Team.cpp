#include "stdafx.h"
#include "Team.h"

#include <ranges>

#include "Prefabs/Player/Player.h"
#include "Prefabs/CharacterHUD/CharacterHUD.h"
#include "Prefabs/Orb/Orb.h"
#include "Prefabs/Pedestal/Pedestal.h"
#include "Components/ControllerComponent.h"

Team::Team(int teamNumber, Orb* pOrb, const std::wstring& teamTexturePath, const std::wstring& teamAuraTexturePath)
	:m_pOrb(pOrb)
	, m_TeamTexturePath(teamTexturePath)
	, m_TeamAuraTexturePath(teamAuraTexturePath)
	, m_TeamNumber(teamNumber)
{

}

Team::~Team()
{
	delete m_pBanishedPlayer;
	m_pBanishedPlayer = nullptr;

	for (auto& key : m_pKilledPlayers | std::views::keys)
	{
		delete key;
		key = nullptr;
	}
}

void Team::AddPlayer(Player* pNewPlayer, const XMFLOAT3& position, bool isActive)
{
	ASSERT_IF_(m_pPlayers.size() == 3);
	pNewPlayer->SetActive(isActive);
	AddChild(static_cast<GameObject*>(pNewPlayer));
	m_pPlayers.emplace_back(pNewPlayer);
	pNewPlayer->GetTransform()->Translate(position);
	m_ResetPositions.emplace_back(std::pair{ position, pNewPlayer });
}

void Team::SetPedestal(Pedestal* pPedestal, const XMFLOAT3& position)
{
	ASSERT_IF_(m_pPedestal != nullptr);
	AddChild(pPedestal);
	m_pPedestal = pPedestal;
	m_pPedestal->GetTransform()->Translate(position);
}

void Team::SetHud(CharacterHUD* pHUD, const XMFLOAT3& position)
{
	ASSERT_IF_(m_pCharacterHUD != nullptr);
	AddChild(pHUD);
	m_pCharacterHUD = pHUD;
	m_pCharacterHUD->GetTransform()->Translate(position);
	const auto pHealth = m_pCharacterHUD->AddChild(new GameObject());
	if (m_TeamNumber == 1)
	{
		pHealth->AddComponent(new SpriteComponent(L"Textures/Health_blue.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
		pHealth->GetTransform()->Translate(-190, -130, 0);
		pHealth->GetTransform()->Scale(m_pCharacterHUD->GetTransform()->GetScale());
	}
	else if (m_TeamNumber == 2)
	{
		pHealth->AddComponent(new SpriteComponent(L"Textures/Health_red.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
		pHealth->GetTransform()->Translate(+190, -130, 0);
		pHealth->GetTransform()->Scale(m_pCharacterHUD->GetTransform()->GetScale());
	}
}

void Team::PassToNextPlayer()
{
	if (m_pPlayers.size() <= 1)
	{
		return;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(false);
	if (m_ActivePlayer == m_pPlayers.size() - 1)
	{
		m_ActivePlayer = 0;
	}
	else
	{
		m_ActivePlayer++;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(true);
	m_pOrb->SetPlayer(m_pPlayers[m_ActivePlayer]);
	m_JustPassed = true;
	m_pCharacterHUD->NextPlayerActivate(m_pPlayers);
}

void Team::PassToPreviousPlayer()
{
	if (m_pPlayers.size() <= 1)
	{
		return;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(false);
	if (m_ActivePlayer <= 0)
	{
		m_ActivePlayer = m_pPlayers.size() - 1;
	}
	else
	{
		m_ActivePlayer--;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_pPlayers[m_ActivePlayer]->SetHasOrb(true);
	m_pOrb->SetPlayer(m_pPlayers[m_ActivePlayer]);
	m_JustPassed = true;
	m_pCharacterHUD->PreviousPlayerActivate(m_pPlayers);
}

void Team::ActiveNextPlayer()
{
	if (m_pPlayers.size() <= 1)
	{
		return;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	if (m_ActivePlayer >= m_pPlayers.size() - 1)
	{
		m_ActivePlayer = 0;
	}
	else
	{
		m_ActivePlayer++;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_JustPassed = true;
	m_pCharacterHUD->NextPlayerActivate(m_pPlayers);
}

void Team::ActivePreviousPlayer()
{
	if (m_pPlayers.size() <= 1)
	{
		return;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(false);
	if (m_ActivePlayer == 0)
	{
		m_ActivePlayer = m_pPlayers.size() - 1;
	}
	else
	{
		m_ActivePlayer--;
	}
	m_pPlayers[m_ActivePlayer]->SetActive(true);
	m_JustPassed = true;
	m_pCharacterHUD->PreviousPlayerActivate(m_pPlayers);
}

void Team::ResetTeam(Player* pScoredPlayer)
{
	for (auto& pPlayer : m_pPlayers)
	{
		FreeRespawnPosition(pPlayer);
	}

	if (pScoredPlayer->GetParent() == this)
	{
		m_pNewBanishedPlayer = pScoredPlayer;
		m_pNewBanishedPlayer->SetHasOrb(false);
		m_pNewBanishedPlayer->SetActive(false);
		m_pPlayers.erase(std::ranges::remove(m_pPlayers, m_pNewBanishedPlayer).begin(), m_pPlayers.end());
	}

	for (int i{}; i < m_pPlayers.size(); ++i)
	{
		SetToNextFreePosition(m_pPlayers[i]);
		m_pPlayers[i]->SetHasOrb(false);
		m_pPlayers[i]->SetActive(false);
	}

	if (m_pBanishedPlayer != nullptr)
	{
		SetToNextFreePosition(m_pBanishedPlayer);
		m_pBanishedPlayer->SetRespawned(true);
		m_PlayerHasToRejoin = true;
	}

	m_pPlayers[0]->SetActive(true);
	m_pCharacterHUD->SetActivePlayer(m_pPlayers[0]);
	m_ActivePlayer = 0;
}

void Team::Kill(Player* pPlayerToDie)
{
	SoundManager::Get()->GetSystem()->playSound(m_pDeadSound, m_pSoundsGroup, false, nullptr);
	GetScene()->GetActiveCamera()->StartScreenShake(.5f, .2f);

	std::cout << "killing player..." << std::endl;
	m_pNewKilledPlayer = pPlayerToDie;
	m_pCharacterHUD->ShowDeadTimer({ pPlayerToDie, 7.f });
	FreeRespawnPosition(pPlayerToDie);

	if (pPlayerToDie->HasOrb() == true)
	{
		m_pOrb->SetIsPickedUp(false);
		m_pOrb->SetPlayer(nullptr);
	}
	pPlayerToDie->SetHasOrb(false);

	m_pPlayers.erase(std::ranges::remove(m_pPlayers, pPlayerToDie).begin(), m_pPlayers.end());

	if (pPlayerToDie->GetIsActive() == true && m_pPlayers.size() > 0)
	{
		pPlayerToDie->SetActive(false);
		m_pPlayers[0]->SetActive(true);
		m_pCharacterHUD->SetActivePlayer(m_pPlayers[0]);
		m_ActivePlayer = 0;
	}
	else
	{
		for (size_t i{}; i < m_pPlayers.size(); ++i)
		{
			if (m_pPlayers[i]->GetIsActive() == true)
			{
				m_ActivePlayer = i;
			}
		}
	}

	std::cout << "active player: " << m_ActivePlayer << std::endl;
	std::cout << "players size: " << m_pPlayers.size() << std::endl;
}

XMFLOAT3 Team::GetActivePlayerPosition() const
{
	if (m_pPlayers.size() > 0)
	{
		return m_pPlayers[m_ActivePlayer]->GetTransform()->GetPosition();
	}
	return { 0,0,0 };
}

void Team::Initialize(const SceneContext& /*sceneContext*/)
{
	//Sound
	SoundManager::Get()->GetSystem()->createStream("Resources/Audio/Death.mp3", FMOD_DEFAULT, nullptr, &m_pDeadSound);

	SoundManager::Get()->GetSystem()->createChannelGroup("Sounds", &m_pSoundsGroup);
	m_pSoundsGroup->setVolume(.3f);
}

void Team::UpdateKilledPlayers(const SceneContext& sceneContext)
{
	for (auto& pKilledPlayer : m_pKilledPlayers)
	{
		pKilledPlayer.second -= sceneContext.pGameTime->GetElapsed();
		if (pKilledPlayer.second <= 0)
		{
			//std::cout << "Player respawned\n";
			std::cout << "spawning player..." << std::endl;
			SetToNextFreePosition(pKilledPlayer.first);
			pKilledPlayer.first->SetRespawned(true);

			AddChild(static_cast<GameObject*>(pKilledPlayer.first));
			if (m_pPlayers.size() <= 0)
			{
				pKilledPlayer.first->SetActive(true);
				m_ActivePlayer = 0;
			}
			m_pPlayers.emplace_back(pKilledPlayer.first);

			m_pKilledPlayers.erase(std::ranges::remove(m_pKilledPlayers, pKilledPlayer).begin(), m_pKilledPlayers.end());

			std::cout << "active player: " << m_ActivePlayer << std::endl;
			std::cout << "players size: " << m_pPlayers.size() << std::endl;
			break;
		}
	}
}

void Team::Update(const SceneContext& sceneContext)
{
	UpdateKilledPlayers(sceneContext);

	if (m_PlayerHasToRejoin == true)
	{
		AddChild(static_cast<GameObject*>(m_pBanishedPlayer));
		m_pPlayers.emplace_back(m_pBanishedPlayer);
		m_pBanishedPlayer = nullptr;
		m_PlayerHasToRejoin = false;
	}
	if (m_pNewBanishedPlayer != nullptr)
	{
		m_pBanishedPlayer = m_pNewBanishedPlayer;
		m_pNewBanishedPlayer->GetComponent<ControllerComponent>()->Move({ 0,100,0 });
		FreeRespawnPosition(m_pBanishedPlayer);
		RemoveChild(m_pBanishedPlayer);
		m_pNewBanishedPlayer = nullptr;
	}
	if (m_pNewKilledPlayer != nullptr)
	{
		m_pNewKilledPlayer->GetComponent<ControllerComponent>()->Move({ 0,100,0 });
		m_pKilledPlayers.emplace_back(std::pair{ m_pNewKilledPlayer, 7.f });
		m_pNewKilledPlayer->SetHasOrb(false);
		m_pNewKilledPlayer->SetActive(false);
		RemoveChild(m_pNewKilledPlayer);
		m_pNewKilledPlayer = nullptr;
	}
}

void Team::SetToNextFreePosition(Player* pPlayer)
{
	for (auto& position : m_ResetPositions)
	{
		if (position.second == nullptr)
		{
			std::cout << "Set to free pos\n";
			position.second = pPlayer;
			pPlayer->GetTransform()->Translate(position.first);
			break;
		}
	}
}

void Team::FreeRespawnPosition(Player* pPlayer)
{
	for (auto& position : m_ResetPositions)
	{
		if (position.second == pPlayer)
		{
			position.second = nullptr;
			break;
		}
	}
}

void Team::Draw(const SceneContext& /*sceneContext*/)
{

}

