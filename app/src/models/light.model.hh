#pragma once

#include <raymath.h>
#include <raylib.h>
#include <vector>
#include <functional>
#include "config.hh"
#include "locations.hh"
#include "camera.service.hh"
#include "shadowmap.model.hh"

class Light
{
  int            id;
  ShadowMap     *shadowMap;
  Shader        *shader;
  CameraService *camera;
  Matrix         matLight;
  Vector3        position       = {0.0f, 0.0f, 0.0f};
  Vector3        direction      = {0.0f, 0.0f, 0.0f};
  Vector3        color          = {1.0f, 1.0f, 1.0f};
  float          energy         = 0.0f;
  Vector3        specular       = {1.0f, 1.0f, 1.0f};
  float          size           = 0.0f;
  float          innerCutOff    = 0.0f;
  float          outerCutOff    = 0.0f;
  float          constant       = 0.0f;
  float          linear         = 0.0f;
  float          quadratic      = 0.0f;
  float          shadowMapTxlSz = 0.0f;
  float          depthBias      = 0.0f;
  int            type;
  int            shadow;
  int            enabled;
  float          xRotation;
  LightLocs      locs;

  public:
  explicit Light(Shader *shader, int id = 0)
      : shader(shader), id(id), shadowMap(new ShadowMap()),
        camera(new CameraService({1.0f, 2.0f, 4.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 90.0f, CAMERA_PERSPECTIVE, 1))
  {
    shadowMapTxlSz = 1.0f / SHADOW_MAP_SIZE;
    locs.init(id, shader);
  }

  ~Light()
  {
    delete shadowMap;
    delete camera;
  }

  // Setters
  void SetId(int id)
  {
    this->id = id;
    locs.init(id, shader);
  }

  void SetXRotation(float rotation) { xRotation = rotation; }

  void SetColor(Vector3 color)
  {
    color = color;
    SetShaderValue(*shader, locs.color, &color, SHADER_UNIFORM_VEC3);
  }

  void SetEnergy(float energy)
  {
    this->energy = energy;
    SetShaderValue(*shader, locs.energy, &energy, SHADER_UNIFORM_FLOAT);
  }

  void SetSpecular(Vector3 color)
  {
    specular = color;
    SetShaderValue(*shader, locs.specular, &specular, SHADER_UNIFORM_VEC3);
  }

  void SetPosition(Vector3 pos, bool updateCamera = false)
  {
    position = pos;
    if(updateCamera) { camera->SetPosition(position); }
    SetShaderValue(*shader, locs.position, &position, SHADER_UNIFORM_VEC3);
  }

  void SetDirection(Vector3 dir)
  {
    direction = dir;
    SetShaderValue(*shader, locs.direction, &direction, SHADER_UNIFORM_VEC3);
  }

  void SetInnerCutOff(float cutOff)
  {
    innerCutOff = cutOff;
    SetShaderValue(*shader, locs.innerCutOff, &innerCutOff, SHADER_UNIFORM_FLOAT);
  }

  void SetOuterCutOff(float cutOff)
  {
    outerCutOff = cutOff;
    SetShaderValue(*shader, locs.outerCutOff, &outerCutOff, SHADER_UNIFORM_FLOAT);
  }

  void SetConstant(float constant)
  {
    this->constant = constant;
    SetShaderValue(*shader, locs.constant, &constant, SHADER_UNIFORM_FLOAT);
  }

  void SetLinear(float linear)
  {
    this->linear = linear;
    SetShaderValue(*shader, locs.linear, &linear, SHADER_UNIFORM_FLOAT);
  }

  void SetQuadratic(float quadratic)
  {
    this->quadratic = quadratic;
    SetShaderValue(*shader, locs.quadratic, &quadratic, SHADER_UNIFORM_FLOAT);
  }

  void SetShadowMapTxlSz(float size)
  {
    shadowMapTxlSz = size;
    SetShaderValue(*shader, locs.shadowMapTxlSz, &shadowMapTxlSz, SHADER_UNIFORM_FLOAT);
  }

  void SetDepthBias(float bias)
  {
    depthBias = bias;
    SetShaderValue(*shader, locs.depthBias, &depthBias, SHADER_UNIFORM_FLOAT);
  }

  void SetType(int type)
  {
    this->type = type;
    SetShaderValue(*shader, locs.type, &type, SHADER_UNIFORM_INT);
  }

  void SetShadow(int shadow)
  {
    this->shadow = shadow;
    SetShaderValue(*shader, locs.shadow, &shadow, SHADER_UNIFORM_INT);
  }

  void SetEnabled(int enabled)
  {
    this->enabled = enabled;
    SetShaderValue(*shader, locs.enabled, &enabled, SHADER_UNIFORM_INT);
  }

  void SetTarget(Vector3 target)
  {
    direction = Vector3Subtract(target, position);
    SetDirection(direction);
    camera->SetTarget(target);
  }

  void SetMatLight(Matrix matrix)
  {
    matLight = matrix;
    SetShaderValueMatrix(*shader, locs.matLight, matrix);
  }

  void SetSize(float size)
  {
    this->size = size;
    SetShaderValue(*shader, locs.size, &size, SHADER_UNIFORM_FLOAT);
  }

  void UpdateVpMatrix()
  {
    camera->Update();
    matLight = MatrixMultiply(camera->view, camera->projection);
  }

  void UpdateShadowMap() { SetShaderValueTexture(*shader, locs.shadowMap, shadowMap->texture); }

  Matrix GetVpMatrix() const { return matLight; }

  Vector3 GetPosition() const { return position; }

  Vector3 GetDirection() const { return direction; }

  CameraService *GetCamera() const { return camera; }

  ShadowMap *GetShadowMap() const { return shadowMap; }

  int GetType() const { return type; }

  int GetShadow() const { return shadow; }

  float GetAngle() const { return xRotation; }

  Color GetColor() const
  {
    return {static_cast<unsigned char>(color.x * 255), static_cast<unsigned char>(color.y * 255), static_cast<unsigned char>(color.z * 255), 255};
  }

  bool IsEnabled() const { return enabled == 1; }

  // Toggles
  void ToggleEnabled() { SetEnabled(enabled == 1 ? 0 : 1); }

  void ToggleShadow() { SetShadow(shadow == 1 ? 0 : 1); }

  // Actions
  void DrawShadowMap() const { DrawTextureEx(shadowMap->texture, {0, static_cast<float>(id) * 200}, 0, 0.1f, WHITE); }

  void RotateAroundTarget(float dx, float dy)
  {
    camera->RotateAroundTarget(dx, dy);
    camera->Update();
    SetPosition(camera->GetPosition(), false);
    SetDirection(Vector3Subtract(camera->GetTarget(), camera->GetPosition()));
  }

  void Draw(int selected) const
  {
    if(id == selected)
      DrawSphere(position, 0.3f, GetColor());
    else
      DrawCubeWires(position, 0.3, 0.3, 0.3, Color{255, 255, 255, 50});
  }
};
