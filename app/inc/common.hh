#pragma once
#include <rlgl.h>
#include "shadowMap.hh"

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 720
#define CONFIG_FLAGS  FLAG_MSAA_4X_HINT //| FLAG_FULLSCREEN_MODE

// Aspect ratio
#define ASPECT_RATIO (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT

// light types
#define DIRECTIONAL 0
#define OMNILIGHT   1
#define SPOTLIGHT   2

// light count
#define NUM_LIGHTS 4

// shadow map size
#define SHADOW_MAP_SIZE 2048

// shader types
typedef enum shaderType
{
  LIGHT_SHADER   = 0,
  DEPTH_SHADER   = 1,
  SKYBOX_SHADER  = 2,
  CUBEMAP_SHADER = 3
} shaderType;

#ifdef USE_NVIDIA
extern "C"
{
  __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

struct EngineOptions
{
  char const *title;
  int         width;
  int         height;
  float       aspectRatio;
};
