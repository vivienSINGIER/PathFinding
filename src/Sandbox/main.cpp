#include "pch.h"

int main()
{
    //Window creation
    Window window(L"Test", 1920, 1080);

    //Camera creation
    Camera camera(CameraType::PERSPECTIVE);
    camera.SetPosition({0.0f, 20.0f, -20.0f});
    camera.SetRotation({45.0f, 0.0f, 0.0f});
    camera.SetFOV(gce::PI/4.0f);
    camera.SetFarPlane(500.0f);
    camera.SetNearPlane(0.001f);

    //Texture loading
    Texture texture("res/Textures/uv.png");

    //Mesh creation
    Cube floor;
    floor.SetTexture(texture);
    floor.SetPosition({0.0f, 0.0f, 0.0f});
    floor.SetScale({20.0f, 0.1f, 20.0f});

    Cube wall;
    wall.SetTexture(texture);
    wall.SetPosition({0.0f, 5.0f, 5.0f});
    wall.SetScale({20.0f, 0.1f, 20.0f});
    wall.SetRotation({90.0f, 0.0f, 0.0f});

    Capsule capsule;
    capsule.SetPosition({-4.0f, 5.0f, 0.0f});
    
    Cube cube;
    cube.SetColor({0.0f, 1.0f, 0.0f});
    cube.SetPosition({-2.0f, 5.0f, 0.0f});
    cube.SetScale({1.0f, 1.0f, 1.0f});
    cube.SetRotation({45.0f, 45.0f, 0.0f});

    Donut donut;
    donut.SetPosition({0.0f, 5.0f,0.0f });

    Cylinder cylinder;
    cylinder.SetPosition({2.0f, 5.0f, 0.0f});

    Sphere sphere;
    sphere.SetPosition({4.0f, 5.0f, 0.0f});

    HalfSphere half_sphere;
    half_sphere.SetPosition({6.0f, 5.0f, 0.0f});

    IcoSphere ico_sphere;
    ico_sphere.SetPosition({-6.0f, 5.0f, 0.0f});

	gce::Font font(L"Arial");

	Text text;
    text.SetFont(&font);
    text.SetText(L"Use WASD to move the camera");
    text.SetPosition({25.0f, 25.0f});
	text.SetColor(gce::Color::White);

    //Rendering Loop
    while (window.IsOpen())
    {
        window.Begin(camera);
            window.Draw(wall);
            window.Draw(floor);
        
            window.Draw(capsule);
            window.Draw(cube);
            window.Draw(donut);
            window.Draw(cylinder);
            window.Draw(sphere);
            window.Draw(half_sphere);
            window.Draw(ico_sphere);

			window.DrawText(text);

        window.End();
        
        window.Display();

        //Game logic
        cube.Rotate({0.005f, 0.0f ,0.0f});

        gce::Vector3f32 dir = {};
        dir.x = GetKey(Keyboard::D) - GetKey(Keyboard::A);
        dir.z = GetKey(Keyboard::W) - GetKey(Keyboard::S);
        dir.SelfNormalize();
        dir *= 0.016f;
        
        camera.Translate(dir);
        
    }
    return 0;
}