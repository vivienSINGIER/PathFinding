#pragma once

#include <vector>
#include <string>
#include <Maths/Vector3.h>

#include "GC-simple-render/Text.h"
#include "GC-simple-render/Geometry.h"

class Cube;
class Sphere;
class Cylinder;
class GameManager;

struct Line 
{
	gce::Vector3f32 start;
	gce::Vector3f32 end;
};

struct DebugGeo
{
	Geometry* pGeometry = nullptr;
	bool isUsed = false;
};

struct DebugText
{
	Text* pText = nullptr;
	bool isUsed = false;
};

class Debug
{
	std::vector<DebugGeo> m_vLines;
	std::vector<DebugText> m_vTexts;
	std::vector<DebugGeo> m_vSpheres;
	std::vector<DebugGeo> m_vCubes;
	
	void Draw(Window& win);

	void Reset();
	
	int GetFirstAvailableLine();
	int GetFirstAvailableText();
	int GetFirstAvailableSphere();
	int GetFirstAvailableCube();
	
public:
	static Debug* Get();
	void Init();

	static void DrawLine(gce::Vector3f32 const& start, gce::Vector3f32 const& end, gce::Vector3f32 const& color);
	static void DrawCube(gce::Vector3f32 const& center, gce::Vector3f32 const& size, gce::Vector3f32 const& color);
	static void DrawSphere(gce::Vector3f32 const& center, float radius, gce::Vector3f32 const& color);
	static void DrawText(gce::Vector3f32 const& pos, const std::string& text, gce::Vector3f32 const& color) {}

	friend GameManager;
};

