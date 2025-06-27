#include"SceneManager.h"

//更新処理
void SceneManager::Update(float elapsedTime)
{
    if (nextScene != nullptr)
    {
        //古いシーンを終了処理
       //Clear();
        if(currentScene)
            currentScene->Finalize();

        //新しいシーンを設定
        currentScene = nextScene;
        nextScene =nullptr;
        //ChangeScene(currentScene);

        //シーン初期化処理

        if (!currentScene->IsReady())
        {
            currentScene->Initialize();
        }
    }

    if (currentScene != nullptr)
    {
        currentScene->Update(elapsedTime);
    }
}

void SceneManager::Render()
{
    if (currentScene != nullptr)
    {
        currentScene->Render();
    }
}

void SceneManager::DrawGUI()
{
    if (currentScene != nullptr)
    {
        currentScene->DrawGUI();
    }
}

void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(Scene* scene)
{
    //新しいシーンを設定
    nextScene = scene;
}


