#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "graphics.hh"

#define ECAMERA_IMPLEMENTATION
#include "ecamera.hh"

class Engine
{
  private:
  EngineOptions o;
  graphics     *g;
  bool          drawShadowMap = false;

  public:
  ECamera *c;

  Engine(EngineOptions options, std::function<void(Shader *)> drawScene) : o(options)
  {
    g = new graphics(&o, drawScene);
    g->init();
    c = new ECamera((Vector3){0.0f, 10.0f, 10.0f}, (Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 90.0f, CAMERA_PERSPECTIVE, o.aspectRatio);
  }
  ~Engine() { delete g; }

  EngineOptions getOptions() { return o; }

  void ToggleShadowMap() { drawShadowMap = !drawShadowMap; }

  void AddActions()
  {
    if(IsKeyPressed(KEY_F1)) { ToggleShadowMap(); }

    c->AddActions();
    g->AddActions();
  }

  void Update() { g->Update(c->GetPosition()); }

  void SetScene(std::function<void(Shader *)> drawScene) { g->SetScene(drawScene); }

  void Run()
  {
    float speed = 0.05f;

    g->CreatePointLight((Vector3){5.0f, 2.0f, 5.0f}, Vector3Zero(), BLUE, speed);
    g->CreatePointLight((Vector3){5.0f, 2.0f, -5.0f}, Vector3Zero(), RED, -speed / 2);

    auto drawFunc = g->GetScene();

    while(!WindowShouldClose())
      {
        AddActions();
        Update();
        g->DoShadowMapping(drawFunc);
        BeginDrawing();
        {
          ClearBackground(BLACK);
          c->Begin();
          {
            g->UpdateShadowMaps();
            drawFunc(g->GetLightShader());
            g->DrawLights();
          }
          c->End();
          if(drawShadowMap)
            {
              BeginShaderMode(*g->GetDepthShader());
              g->DrawShadowMaps();
              EndShaderMode();
            }
          DrawFPS(10, 10);
          g->DrawActions(10, 30);
        }
        EndDrawing();
      }
  }
};

#endif // ENGINE_CORE_H