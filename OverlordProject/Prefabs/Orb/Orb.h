#pragma once
class Player;

class Orb : public GameObject
{
public:
	Orb();
	~Orb() override = default;

	Orb(const Orb& other) = delete;
	Orb(Orb&& other) noexcept = delete;
	Orb& operator=(const Orb& other) = delete;
	Orb& operator=(Orb&& other) noexcept = delete;

	void SetPlayer(Player* pPlayer) { m_pPlayerPickedUp = pPlayer; }
	void SetIsPickedUp(bool value) { m_IsPickedUp = value; }
	void SetOrbPosition(const XMFLOAT3& pos) { m_CurrentOrbPosition = pos; }

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	void PickUpBall(GameObject* pPlayerObject, PxTriggerAction triggerAction);

	bool m_IsPickedUp{ false };
	Player* m_pPlayerPickedUp{ nullptr };
	XMFLOAT3 m_CurrentOrbPosition{};
};

