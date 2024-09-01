#pragma once

#include "graphics.model.hh"
#include "camera.service.hh"

class Engine
{
  private:
  EngineOptions o;
  graphics     *g;
  bool          drawShadowMap = false;
  bool          drawSkybox    = true;

  public:
  CameraService *c;

  Engine(EngineOptions options, std::function<void(Shader *)> drawScene) : o(options)
  {
    g = new graphics(&o, drawScene);
    g->init();
    c = new CameraService((Vector3){0.0f, 10.0f, 10.0f}, (Vector3){0.0f, 0.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 90.0f, CAMERA_PERSPECTIVE, o.aspectRatio);
  }
  ~Engine() { delete g; }

  EngineOptions getOptions() { return o; }

  void ToggleShadowMap() { drawShadowMap = !drawShadowMap; }

  void ToggleSkybox() { drawSkybox = !drawSkybox; }

  void AddActions()
  {
    if(IsKeyPressed(KEY_O)) { ToggleShadowMap(); }
    if(IsKeyPressed(KEY_I)) { ToggleSkybox(); }

    c->AddActions();
    g->AddActions();
  }

  void Update() { g->Update(c->GetPosition()); }

  void SetScene(std::function<void(Shader *)> drawScene) { g->SetScene(drawScene); }

  void Run()
  {
    float speed = 0.05f;

    g->CreatePointLight((Vector3){5.0f, 2.0f, 5.0f}, Vector3Zero(), BLUE, speed);
    g->CreatePointLight((Vector3){5.0f, 2.0f, -5.0f}, Vector3Zero(), RED, -speed );

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
            if(drawSkybox) { g->DrawSkybox(); }
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
