#pragma once

#include <Maths/Vector3.h>

class Scene;

class Entity
{
    struct Target 
    {
		gce::Vector3f32 position;
        float distance;
		bool isSet;
    	bool isReached;
    };

protected:
    gce::Geometry* m_Shape;
    gce::Vector3f32 mDirection;
	Target mTarget;
	float m_Radius = 0.f;
    float mSpeed = 0.f;
    float mSpeedFactor = 1.f;
    bool mToDestroy = false;
    int mTag = -1;
	bool mRigidBody = false;

public:
	bool GoToDirection(int x, int y, float speed = -1.f);
    bool GoToPosition(int x, int y, float speed = -1.f);
    void SetPosition(float x, float y, float ratioX = 0.5f, float ratioY = 0.5f);
	void SetDirection(float x, float y, float speed = -1.f);
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetSpeedFactor(float speedFactor) { mSpeedFactor = speedFactor; }
	float GetSpeedFactor() { return mSpeedFactor; }
	void SetTag(int tag) { mTag = tag; }
	float GetRadius() const { return m_Radius; }
	void SetRigidBody(bool isRigitBody) { mRigidBody = isRigitBody; }
	bool IsRigidBody() const { return mRigidBody; }

    gce::Vector3f32 GetPosition(float ratioX = 0.5f, float ratioY = 0.5f) const;
	gce::Geometry* GetShape() { return m_Shape; }

	bool IsTag(int tag) const { return mTag == tag; }
    bool IsColliding(Entity* other) const;
	bool IsInside(float x, float y) const;

    void Destroy();
	bool ToDestroy() const { return mToDestroy; }
	
	template<typename T>
	T* GetScene() const;

    Scene* GetScene() const;
	float GetDeltaTime() const;

    template<typename T>
    T* CreateEntity(float radius, const sf::Color& color);

protected:
    Entity() = default;
    ~Entity() = default;

    virtual void OnUpdate() {};
    virtual void OnCollision(Entity* collidedWith) {};
	virtual void OnInitialize() {};
	virtual void OnDestroy() {};
	
private:
    void Update();
	void Initialize(float radius);
	void Repulse(Entity* other);

    friend class GameManager;
    friend Scene;
};

#include "Entity.inl"