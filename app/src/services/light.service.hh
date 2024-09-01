#pragma once
#include <raymath.h>
#include <raylib.h>
#include <vector>
#include <functional>
#include "config.hh"
#include "light.model.hh"
#include "camera.service.hh"
#include "locations.hh"

class LightService
{
  std::vector<Light *> lights;
  Shader              *shader;
  LightGeneralLocs    *locs;
  Vector3              colAmbient    = {0.0f, 0.0f, 0.0f};
  Vector3              viewPos       = {0.0f, 0.0f, 0.0f};
  int                  count         = 0;
  int                  selectedLight = 0;

  public:
  explicit LightService(Shader *shader) : shader(shader), locs(new LightGeneralLocs()) { init(); }

  ~LightService()
  {
    delete locs;
    for(auto light : lights) { delete light; }
  }

  void init()
  {
    locs->init(shader);
    SetColAmbient({0.5f, 0.5f, 0.5f});
  }

  // Setters
  void SetViewPos(Vector3 pos)
  {
    viewPos = pos;
    SetShaderValue(*shader, locs->viewPos, &viewPos, SHADER_UNIFORM_VEC3);
  }

  void SetColAmbient(Vector3 col)
  {
    colAmbient = col;
    SetShaderValue(*shader, locs->colAmbient, &colAmbient, SHADER_UNIFORM_VEC3);
  }

  // Actions
  int CreatePointLight(Vector3 position, Vector3 target, Color color, float x_rotation)
  {
    auto   *light           = new Light(shader);
    Vector3 normalizedColor = {static_cast<float>(color.r) / 255, static_cast<float>(color.g) / 255, static_cast<float>(color.b) / 255};
    light->SetId(count);
    light->SetXRotation(x_rotation);
    light->SetPosition(position);
    light->SetTarget(target);
    light->SetColor(normalizedColor);
    light->SetSpecular(normalizedColor);
    light->SetInnerCutOff(INNER_CUTOFF);
    light->SetOuterCutOff(OUTER_CUTOFF);
    light->SetType(OMNILIGHT);
    light->SetDepthBias(DEFAULT_DEPTH_BIAS);
    light->SetShadow(1);
    light->SetEnabled(1);
    light->SetConstant(ATTENUATION_CONSTANT);
    light->SetLinear(ATTENUATION_LINEAR);
    light->SetQuadratic(ATTENUATION_QUADRATIC);
    light->SetShadowMapTxlSz(1.0f / SHADOW_MAP_SIZE);
    lights.push_back(light);
    return count++;
  }

  void DoShadowMapping(const std::function<void(Shader *)> &drawScene)
  {
    for(auto &light : lights)
      {
        light->GetShadowMap()->Begin();
        light->GetCamera()->Begin();
        drawScene(nullptr);
        light->GetCamera()->End();
        light->GetShadowMap()->End();
      }
  }

  void drawShadows() const
  {
    for(const auto &light : lights) { light->DrawShadowMap(); }
  }

  void draw() const
  {
    for(const auto &light : lights)
      {
        if(light->IsEnabled()) light->Draw(selectedLight);
      }
  }

  void updateShadows() const
  {
    for(const auto &light : lights) { light->UpdateShadowMap(); }
  }

  void update(Vector3 viewPos)
  {
    for(auto &light : lights)
      {
        if(light->IsEnabled())
          {
            CameraService *camera = light->GetCamera();
            light->RotateAroundTarget(light->GetAngle(), 0.0f);
            light->UpdateMatLight();
            light->SetMatLight(light->GetMatLight());
          }
      }
    SetViewPos(viewPos);
  }

  void toggleLight(int id) { lights[id]->ToggleEnabled(); }

  void toggleShadow(int id) { lights[id]->ToggleShadow(); }

  void AddActions()
  {
    if(IsKeyPressed(KEY_N)) { selectedLight = (selectedLight + 1) % count; }
    if(IsKeyPressed(KEY_M)) { toggleLight(selectedLight); }
    if(IsKeyPressed(KEY_B)) { toggleShadow(selectedLight); }
    if(IsKeyPressed(KEY_P)) { selectedLight = (selectedLight - 1) >= 0 ? (selectedLight - 1) : selectedLight; }
  }

  void DrawActions(float X, float Y)
  {
    DrawText("N: Next Light", X, Y, 10, WHITE);
    DrawText("M: Toggle Light", X, Y + 10, 10, WHITE);
    DrawText("B: Toggle Shadow", X, Y + 20, 10, WHITE);
    DrawText("P: Previous Light", X, Y + 30, 10, WHITE);
  }

  const std::vector<Light *> &GetLights() const { return lights; }
};