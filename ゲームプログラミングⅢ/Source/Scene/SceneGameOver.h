#pragma once
#include"Scene.h"
#include"System/Sprite.h"
#include"Life.h"

class Game_Over:public Scene
{
public:
	Game_Over(){}
	~Game_Over(){}

    //‰Šú‰»
    void Initialize();

    //I—¹‰»
    void Finalize();

    //XVˆ—
    void Update(float elapsedTime);

    //•`‰æˆ—
    void Render();

    //GUI•`‰æ
    void DrawGUI(){}
private:
    Sprite* GameOver;
    Life* life[3];
};