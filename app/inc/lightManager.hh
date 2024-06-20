#pragma once
#include <raymath.h>
#include <raylib.h>
#include <vector>
#include <functional>
#include "common.hh"
#include "light.hh"
#include "ecamera.hh"

class LightManagerLocs
{
  public:
  int colSpecular;
  int colAmbient;
  int viewPos;
  int shininess;
  int useSpecularMap;
  int useNormalMap;

  explicit LightManagerLocs(Shader *shader) { init(shader); }

  void init(Shader *shader)
  {
    colSpecular    = GetShaderLocation(*shader, "colSpecular");
    colAmbient     = GetShaderLocation(*shader, "colAmbient");
    viewPos        = GetShaderLocation(*shader, "viewPos");
    shininess      = GetShaderLocation(*shader, "shininess");
    useSpecularMap = GetShaderLocation(*shader, "useSpecularMap");
    useNormalMap   = GetShaderLocation(*shader, "useNormalMap");
  }
};

class LightService
{
  std::vector<Light *> lights;
  Shader              *shader;
  LightManagerLocs    *locs;
  Vector3              colSpecular;
  Vector3              colAmbient;
  Vector3              viewPos;
  float                shininess;
  int                  useSpecularMap;
  int                  useNormalMap;
  int                  lightCount    = 0;
  int                  selectedLight = 0;

  public:
  explicit LightService(Shader *shader) : shader(shader), locs(new LightManagerLocs(shader)) { init(); }

  ~LightService()
  {
    delete locs;
    for(auto light : lights) { delete light; }
  }

  void init()
  {
    locs->init(shader);
    SetColAmbient({0.5f, 0.5f, 0.5f});
    SetColSpecular({1.0f, 1.0f, 1.0f});
    SetShininess(32.0f);
    SetUseSpecularMap(0);
    SetUseNormalMap(0);
  }

  // Setters
  void SetViewPos(Vector3 pos)
  {
    viewPos = pos;
    SetShaderValue(*shader, locs->viewPos, &viewPos, SHADER_UNIFORM_VEC3);
  }

  void SetColSpecular(Vector3 col)
  {
    colSpecular = col;
    SetShaderValue(*shader, locs->colSpecular, &colSpecular, SHADER_UNIFORM_VEC3);
  }

  void SetColAmbient(Vector3 col)
  {
    colAmbient = col;
    SetShaderValue(*shader, locs->colAmbient, &colAmbient, SHADER_UNIFORM_VEC3);
  }

  void SetShininess(float shininess)
  {
    this->shininess = shininess;
    SetShaderValue(*shader, locs->shininess, &shininess, SHADER_UNIFORM_FLOAT);
  }

  void SetUseSpecularMap(int useSpecularMap)
  {
    this->useSpecularMap = useSpecularMap;
    SetShaderValue(*shader, locs->useSpecularMap, &useSpecularMap, SHADER_UNIFORM_INT);
  }

  void SetUseNormalMap(int useNormalMap)
  {
    this->useNormalMap = useNormalMap;
    SetShaderValue(*shader, locs->useNormalMap, &useNormalMap, SHADER_UNIFORM_INT);
  }

  // Actions
  int CreatePointLight(Vector3 pos, Vector3 target, Color color, float xRot)
  {
    auto   *light           = new Light(shader);
    Vector3 normalizedColor = {static_cast<float>(color.r) / 255, static_cast<float>(color.g) / 255, static_cast<float>(color.b) / 255};

    light->SetId(lightCount);
    light->SetXRotation(xRot);
    light->SetDiffuseColor(normalizedColor);
    light->SetSpecularColor(normalizedColor);
    light->SetPosition(pos);
    light->SetTarget(target);
    light->SetInnerCutOff(15.5f);
    light->SetOuterCutOff(10.0f);
    light->SetType(OMNILIGHT);
    light->SetDepthBias(0.001f);
    light->SetShadow(1);
    light->SetEnabled(1);
    light->SetConstant(1.0f);
    light->SetLinear(0.09f);
    light->SetQuadratic(0.032f);
    light->SetShadowMapTxlSz(1.0f / SHADOW_MAP_SIZE);

    lights.push_back(light);
    return lightCount++;
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

  void DrawShadowMaps() const
  {
    for(const auto &light : lights) { light->DrawShadowMap(); }
  }

  void DrawLights() const
  {
    for(const auto &light : lights)
      {
        if(light->IsEnabled()) light->Draw(selectedLight);
      }
  }

  void UpdateShadowMaps() const
  {
    for(const auto &light : lights) { light->UpdateShadowMap(); }
  }

  void Update(Vector3 viewPos)
  {
    for(auto &light : lights)
      {
        if(light->IsEnabled())
          {
            ECamera *camera = light->GetCamera();
            light->RotateAroundTarget(light->GetAngle(), 0.0f);
            light->UpdateVpMatrix();
            light->SetVpMatrix(light->GetVpMatrix());
          }
      }
    SetViewPos(viewPos);
  }

  void toggleLight(int id) { lights[id]->ToggleEnabled(); }

  void toggleShadow(int id) { lights[id]->ToggleShadow(); }

  void AddActions()
  {
    if(IsKeyPressed(KEY_N)) { selectedLight = (selectedLight + 1) % lightCount; }
    if(IsKeyPressed(KEY_M)) { toggleLight(selectedLight); }
    if(IsKeyPressed(KEY_B)) { toggleShadow(selectedLight); }
    if(IsKeyPressed(KEY_P)) { selectedLight = (selectedLight - 1) >= 0 ? (selectedLight - 1) : selectedLight; }
    if(IsKeyPressed(KEY_S)) { SetShininess(shininess * 2); }
    if(IsKeyPressed(KEY_A)) { SetShininess(shininess / 2); }
  }

  void DrawActions(float X, float Y)
  {
    DrawText("N: Next Light", X, Y, 10, WHITE);
    DrawText("M: Toggle Light", X, Y + 10, 10, WHITE);
    DrawText("B: Toggle Shadow", X, Y + 20, 10, WHITE);
    DrawText("P: Previous Light", X, Y + 30, 10, WHITE);
    DrawText("S: Increase Shininess", X, Y + 40, 10, WHITE);
    DrawText("A: Decrease Shininess", X, Y + 50, 10, WHITE);
  }

  const std::vector<Light *> &GetLights() const { return lights; }
};