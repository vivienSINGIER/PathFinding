#pragma once

#include <Maths/Vector3.h>
#include "../GC-simple-render/Geometries.h"

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
    Geometry* m_Shape;
    gce::Vector3f32 mDirection;
	Target mTarget;
	float m_Radius = 0.f;
    float mSpeed = 0.f;
    float mSpeedFactor = 1.f;
    bool mToDestroy = false;
    int mTag = -1;
	bool mRigidBody = false;

public:
	bool GoToDirection(float x, float y, float z, float speed = -1.f);
    bool GoToPosition(float x, float y, float z, float speed = -1.f);
    void SetPosition(float x, float y, float z, float ratioX = 0.5f, float ratioY = 0.5f, float ratioZ = 0.5f);
	void SetDirection(float x, float y, float z, float speed = -1.f);
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetSpeedFactor(float speedFactor) { mSpeedFactor = speedFactor; }
	float GetSpeedFactor() { return mSpeedFactor; }
	void SetTag(int tag) { mTag = tag; }
	float GetRadius() const { return m_Radius; }
	void SetRigidBody(bool isRigitBody) { mRigidBody = isRigitBody; }
	bool IsRigidBody() const { return mRigidBody; }

    gce::Vector3f32 GetPosition(float ratioX = 0.5f, float ratioY = 0.5f, float ratioZ = 0.5f) const;
	Geometry* GetShape() { return m_Shape; }

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
    T* CreateEntity(float radius, gce::Vector3f32 color);

protected:
    Entity() = default;
    ~Entity() = default;

    virtual void OnUpdate() {};
    virtual void OnCollision(Entity* collidedWith) {};
	virtual void OnInitialize() {};
	virtual void OnDestroy() {};
	
private:
    void Update();
	void Initialize(float radius, gce::Vector3f32 color);
	void Repulse(Entity* other);

    friend class GameManager;
    friend Scene;
};

#include "Entity.inl"