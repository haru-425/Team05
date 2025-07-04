#pragma once

//V[
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //ú»
    virtual void Initialize() = 0;

    //I¹»
    virtual void Finalize() = 0;

    //XV
    virtual void Update(float elapsedTime) = 0;

    //`æ
    virtual void Render() = 0;

    //GUI`æ
    virtual void DrawGUI() = 0;

    //õ®¹µÄ¢é©
    bool IsReady()const { return ready; }

    //õ®¹Ýè
    void SetReady() { ready = true; }

private:
    bool ready = false;
};