#pragma once
#include <stdio.h>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include "config.hh"
#include "light.service.hh"
#include "skybox.service.hh"

class graphics
{
  private:
  void                 *w;  // window handle
  EngineOptions        *o;  // engine options
  std::vector<Shader *> s;  // shaders
  LightService         *ls; // light service
  SkyboxService        *sb; // skybox service

  std::function<void(Shader *)> &drawScene;

  Shader *GetShader(int index) { return s[index]; } // get shader by index
  Shader *GetShaderById(int id)
  {
    auto it = std::find_if(s.begin(), s.end(), [&](const Shader *shader) { return shader->id == id; });
    if(it != s.end()) { return *it; }
    return nullptr;
  }
  static void LogColored(int msgType, const char *text, va_list args)
  {
    switch(msgType)
      {
      case LOG_INFO: printf("[\e[1;32mINFO\e[0m] : "); break;
      case LOG_ERROR: printf("[\e[1;31mERROR\e[0m]: "); break;
      case LOG_WARNING: printf("[\e[1;33mWARN\e[0m] : "); break;
      case LOG_DEBUG: printf("[DEBUG]: "); break;
      default: break;
      }

    vprintf(text, args);
    printf("\n");
    fflush(stdout);
  }

  public:
  graphics(EngineOptions *options, std::function<void(Shader *)> &scene) : o(options), drawScene(scene) { s = std::vector<Shader *>(); }

  ~graphics() { RemoveAllShaders(); }

  void *init()
  {
    if(o->title == nullptr) o->title = "No Name";
    if(o->width == 0) o->width = 800;
    if(o->height == 0) o->height = 450;

    SetTraceLogCallback(LogColored);

    SetConfigFlags(CONFIG_FLAGS);

    InitWindow(o->width, o->height, o->title);

    SetTargetFPS(500);

    w = GetWindowHandle();

    // Add shaders

    Shader *light   = (Shader *)malloc(sizeof(Shader));
    Shader *depth   = (Shader *)malloc(sizeof(Shader));
    Shader *skybox  = (Shader *)malloc(sizeof(Shader));
    Shader *cubemap = (Shader *)malloc(sizeof(Shader));
    Shader *conv    = (Shader *)malloc(sizeof(Shader));

    *light   = LoadShader(SHADER_DIR "lights.vs", SHADER_DIR "lights.fs");
    *depth   = LoadShader(0, SHADER_DIR "depth.fs");
    *skybox  = LoadShader(SHADER_DIR "skybox.vs", SHADER_DIR "skybox.fs");
    *cubemap = LoadShader(SHADER_DIR "cubemap.vs", SHADER_DIR "cubemap.fs");
    *conv    = LoadShader(SHADER_DIR "cubemap.vs", SHADER_DIR "convolution.fs");

    addShader(light);
    addShader(depth);
    addShader(skybox);
    addShader(cubemap);
    addShader(conv);

    ls = new LightService(GetShader(LIGHT_SHADER));

    sb = new SkyboxService(SKYBOX_DIR "skybox.png", GetSkyboxShader(), GetCubemapShader(), GetConvolutionShader());

    if(w == nullptr) { TraceLog(LOG_ERROR, "FAILED TO CREATE WINDOW"); }

    return w;
  }

  void free() { CloseWindow(); }

  // window
  const char *GetTitle() const { return o->title; }

  int GetWindowWidth() const { return o->width; }

  int GetWindowHeight() const { return o->height; }

  void *GetWindowHandle() const { return w; }

  // shaders
  void addShader(Shader *shader) { s.push_back(shader); }

  Shader *GetLightShader() { return GetShader(LIGHT_SHADER); }

  Shader *GetDepthShader() { return GetShader(DEPTH_SHADER); }

  Shader *GetSkyboxShader() { return GetShader(SKYBOX_SHADER); }

  Shader *GetCubemapShader() { return GetShader(CUBEMAP_SHADER); }

  Shader *GetConvolutionShader() { return GetShader(CONVOLUTION_SHADER); }

  void RemoveShader(Shader *shader)
  {
    UnloadShader(*shader);
    auto it = std::find_if(s.begin(), s.end(), [&](const Shader *s) { return s->id == shader->id; });
    if(it != s.end()) { s.erase(it); }
  }

  void RemoveShaderById(int id)
  {
    auto it = std::find_if(s.begin(), s.end(), [&](const Shader *s) { return s->id == id; });
    if(it != s.end())
      {
        UnloadShader(**it);
        delete *it;
        s.erase(it);
      }
  }

  void RemoveAllShaders()
  {
    for(Shader *shader : s)
      {
        UnloadShader(*shader);
        delete shader;
      }
    s.clear();
  }

  // update func
  void Update(Vector3 viewPos) { ls->Update(viewPos); }

  void DoShadowMapping(std::function<void(Shader *)> &scene) { ls->DoShadowMapping(scene); }

  void UpdateShadowMaps() { ls->UpdateShadowMaps(); }

  // draw func
  void SetScene(std::function<void(Shader *)> &scene) { drawScene = scene; }

  // get draw func
  std::function<void(Shader *)> GetScene() { return drawScene; }

  void CreatePointLight(Vector3 pos, Vector3 targ, Color color, float rotationX) { ls->CreatePointLight(pos, targ, color, rotationX); }

  void DrawShadowMaps() { ls->DrawShadowMaps(); }

  void AddActions() { ls->AddActions(); }

  void DrawLights() { ls->DrawLights(); }

  void DrawSkybox() { sb->DrawSkybox(); }

  void DrawActions(float X, float Y) { ls->DrawActions(X, Y); }
};

