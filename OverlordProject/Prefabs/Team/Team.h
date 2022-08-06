#pragma once
class Orb;
class CharacterHUD;
class Pedestal;
class Player;

class Team : public GameObject
{
public:
	Team(Orb* pOrb,const std::wstring& teamTexturePath,const std::wstring& teamAuraTexturePath);
	~Team() override = default;

	Team(const Team& other) = delete;
	Team(Team&& other) noexcept = delete;
	Team& operator=(const Team& other) = delete;
	Team& operator=(Team&& other) noexcept = delete;

	void AddPlayer(Player* pNewPlayer, const XMFLOAT3& position, bool isActive);
	void SetPedestal(Pedestal* pPedestal, const XMFLOAT3& position);
	void SetHud(CharacterHUD* pHUD, const XMFLOAT3& position);
	const std::wstring& GetTexturePath() { return m_TeamTexturePath; }
	const std::wstring& GetAuraTexturePath() { return m_TeamAuraTexturePath; }

	void PassToNextPlayer();
	void PassToPreviousPlayer();
	void ActiveNextPlayer();
	void ActivePreviousPlayer();

	bool GetJustPassed() const { return m_JustPassed; }
	void ResetPass() { m_JustPassed = false; }

	int GetTeamNumber() const { return m_TeamNumber; }

	void ResetTeam() const;

	const XMFLOAT3& GetActivePlayerPosition() const;
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override;

private:
	std::vector<Player*> m_pPlayers{};
	Player* m_pBanishedPlayer{};
	int m_ActivePlayer{};

	std::vector<XMFLOAT3> m_ResetPositions{};

	Pedestal* m_pPedestal{};
	CharacterHUD* m_pCharacterHUD{};
	Orb* m_pOrb{};
	std::wstring m_TeamTexturePath{};
	std::wstring m_TeamAuraTexturePath{};

	bool m_JustPassed{ false };

	int m_TeamNumber{};
	static int m_NumberOfTeams;
};

