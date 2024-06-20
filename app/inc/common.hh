#pragma once
#include <rlgl.h>
#include "shadowMap.hh"

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 800
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
#define LIGHT_SHADER 0
#define DEPTH_SHADER 1

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
