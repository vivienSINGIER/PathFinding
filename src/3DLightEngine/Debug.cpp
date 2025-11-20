#include "3DLightEngine/Debug.h"

#include "3DLightEngine/GameManager.h"

// #include <SFML/Graphics/RenderWindow.hpp>

#include <string>

#include "../GC-simple-render/Geometries.h"

Debug* Debug::Get()
{
	static Debug mInstance;

	return &mInstance;
}

void Debug::Init()
{
	for (int i = 0; i < 50; i++)
	{
		DebugGeo geo;
		geo.pGeometry = new Cylinder();
		m_vLines.push_back(geo);
	}

	for (int i = 0; i < 50; i++)
	{
		DebugGeo geo;
		geo.pGeometry = new Sphere();
		m_vSpheres.push_back(geo);
	}

	for (int i = 0; i < 500; i++)
	{
		DebugGeo geo;
		geo.pGeometry = new Cube();
		m_vCubes.push_back(geo);
	}
}

void Debug::Draw(Window& win)
{
	// Lines
	for (int i = 0; i < m_vLines.size(); i++)
	{
		if (m_vLines[i].isUsed == false)
			continue;
		win.Draw(*m_vLines[i].pGeometry);
	}
	
	// Spheres
	for (int i = 0; i < m_vSpheres.size(); i++)
	{
		if (m_vSpheres[i].isUsed == false)
			continue;
		win.Draw(*m_vSpheres[i].pGeometry);
	}

	// Cubes
	for (int i = 0; i < m_vCubes.size(); i++)
	{
		if (m_vCubes[i].isUsed == false)
			continue;
		win.Draw(*m_vCubes[i].pGeometry);
	}

	// Text
	for (int i = 0; i < m_vTexts.size(); i++)
	{
		if (m_vTexts[i].isUsed == false)
			continue;
		win.DrawText(*m_vTexts[i].pText);
	}
}

void Debug::Reset()
{
	for (int i = 0; i < m_vLines.size(); i++)
	{
		Geometry* geo = m_vLines[i].pGeometry;
		if (geo == nullptr) continue;

		geo->SetPosition({0.0f, 0.0f, 0.0f});
		geo->SetRotation({0.0f, 0.0f, 0.0f});
		geo->SetScale({1.0f, 1.0f, 1.0f});
		m_vLines[i].isUsed = false;
	}

	for (int i = 0; i < m_vSpheres.size(); i++)
	{
		Geometry* geo = m_vSpheres[i].pGeometry;
		if (geo == nullptr) continue;

		geo->SetPosition({0.0f, 0.0f, 0.0f});
		geo->SetRotation({0.0f, 0.0f, 0.0f});
		geo->SetScale({1.0f, 1.0f, 1.0f});
		m_vSpheres[i].isUsed = false;
	}

	for (int i = 0; i < m_vCubes.size(); i++)
	{
		Geometry* geo = m_vCubes[i].pGeometry;
		if (geo == nullptr) continue;

		geo->SetPosition({0.0f, 0.0f, 0.0f});
		geo->SetRotation({0.0f, 0.0f, 0.0f});
		geo->SetScale({1.0f, 1.0f, 1.0f});
		m_vCubes[i].isUsed = false;
	}
}

int Debug::GetFirstAvailableLine()
{
	for (int i = 0; i < m_vLines.size(); i++)
	{
		if (m_vLines[i].isUsed == false) return i;
	}
	return -1;
}

int Debug::GetFirstAvailableText()
{
	for (int i = 0; i < m_vTexts.size(); i++)
	{
		if (m_vTexts[i].isUsed == false) return i;
	}
	return -1;
}

int Debug::GetFirstAvailableSphere()
{
	for (int i = 0; i < m_vSpheres.size(); i++)
	{
		if (m_vSpheres[i].isUsed == false) return i;
	}
	return -1;
}

int Debug::GetFirstAvailableCube()
{
	for (int i = 0; i < m_vCubes.size(); i++)
	{
		if (m_vCubes[i].isUsed == false) return i;
	}
	return -1;
}

void Debug::DrawLine(gce::Vector3f32 const& start, gce::Vector3f32 const& end, gce::Vector3f32 const& color)
{
	int index = Get()->GetFirstAvailableLine();
	if (index == -1) return;
	
	DebugGeo& geo = Get()->m_vLines[index];
	Geometry* cyl = geo.pGeometry;
	if (cyl == nullptr) return;
	cyl->SetColor(color);
	geo.isUsed = true;
	
	gce::Vector3f32 dir = end - start;
	gce::Vector3f32 tiltDir = dir;
	tiltDir.SelfNormalize();
	float length = dir.Norm();
	
	gce::Vector3f32 mid = start + dir * 0.5f;
	cyl->SetPosition(mid);
	
	cyl->SetScale({0.1f, length, 0.1f});
	
	dir.x = gce::Clamp(dir.x, -1.0f, 1.0f);
	dir.y = 0.f;
	dir.z = gce::Clamp(dir.z, -1.0f, 1.0f);
	
	gce::Vector3f32 rot;

	float tilt = asinf(tiltDir.y) * 180.0f / gce::PI;
	
	if (dir == gce::Vector3f32(1.0f, 0.0f, 0.0f))
		rot = {0.0f, 0.0f, 90.0f + tilt};
	if (dir == gce::Vector3f32(-1.0f, 0.0f, 0.0f))
		rot = {0.0f, 0.0f, -90.0f - tilt};
	if (dir == gce::Vector3f32(0.0f, 0.0f, 1.0f))
		rot = {90.0f - tilt, 0.0f, 00.0f};
	if (dir == gce::Vector3f32(0.0f, 0.0f, -1.0f))
		rot = {-90.0f + tilt, 0.0f, 0.0f};
	if (dir == gce::Vector3f32(1.0f, 0.0f, 1.0f))
		rot = {0.0f, -45.0f, 90.0f + tilt};
	if (dir == gce::Vector3f32(-1.0f, 0.0f, 1.0f))
		rot = {0.0f, 45.0f, 90.0f - tilt};
	if (dir == gce::Vector3f32(1.0f, 0.0f, -1.0f))
		rot = {0.0f, 45.0f, 90.0f + tilt};
	if (dir == gce::Vector3f32(-1.0f, 0.0f, -1.0f))
		rot = {0.0f, -45.0f, 90.0f - tilt};

	dir = end - start;
	dir.SelfNormalize();
	
	// if (start.y != end.y)
	// {
	// 	float tilt = asinf(dir.y) * 180.0f / gce::PI;
	// 	if (dir.y != 0.0f)
	// 		rot.z += tilt;
	// 	if (dir.x != 0.0f)
	// 		rot.x += tilt;
	// 	// rot.x += tilt;
	// }
	
	cyl->SetRotation(rot);
}

void Debug::DrawCube(gce::Vector3f32 const& center, gce::Vector3f32 const& size, gce::Vector3f32 const& color)
{
	int index = Get()->GetFirstAvailableCube();
	if (index == -1) return;

	DebugGeo& geo = Get()->m_vCubes[index];
	Geometry* cube = geo.pGeometry;
	if (cube == nullptr) return;
	cube->SetColor(color);
	geo.isUsed = true;

	cube->SetPosition(center);
	cube->SetScale({abs(size.x), abs(size.y), abs(size.z)});
}

void Debug::DrawSphere(gce::Vector3f32 const& center, float radius, gce::Vector3f32 const& color)
{
	int index = Get()->GetFirstAvailableSphere();
	if (index == -1) return;

	DebugGeo& geo = Get()->m_vSpheres[index];
	Geometry* sphere = geo.pGeometry;
	if (sphere == nullptr) return;
	sphere->SetColor(color);
	geo.isUsed = true;

	sphere->SetPosition(center);
	sphere->SetScale({radius, radius, radius});
}


// void Debug::DrawText(float x, float y, const std::string& text, const sf::Color& color)
// {
// 	DrawText(x, y, text, 0.f, 0.f, color);
// }
//
// void Debug::DrawText(float x, float y, const std::string& text, float ratioX, float ratioY, const sf::Color& color)
// {
// 	_ASSERT(ratioX >= 0.f && ratioX <= 1.f);
// 	_ASSERT(ratioY >= 0.f && ratioY <= 1.f);
//
// 	sf::Text sfText;
//
// 	sfText.setFont(GameManager::Get()->GetFont());
// 	sfText.setString(text);
// 	sfText.setCharacterSize(20);
// 	sfText.setFillColor(color);
// 	sfText.setPosition(x, y);
//
// 	const sf::FloatRect& bounds = sfText.getLocalBounds();
// 	sfText.setOrigin(bounds.width * ratioX, bounds.height * ratioY);
//
// 	Debug::Get()->mTexts.push_back(sfText);
// }

