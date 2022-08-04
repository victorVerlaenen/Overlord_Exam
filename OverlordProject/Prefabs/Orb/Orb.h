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
	void SetPassed(bool value) { m_JustPassed = value; }
	bool GetPassed() const { return m_JustPassed; }
	void SetIsPickedUp(bool value) { m_IsPickedUp = value; }

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	void PickUpBall(GameObject* pPlayerObject, PxTriggerAction triggerAction);

	bool m_IsPickedUp{ false };
	Player* m_pPlayerPickedUp{ nullptr };
	bool m_JustPassed{ false };
};

