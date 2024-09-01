#pragma once
#include <rlgl.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define CONFIG_FLAGS  FLAG_MSAA_4X_HINT //| FLAG_FULLSCREEN_MODE
#define ASPECT_RATIO  (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT
// lights
#define NUM_LIGHTS            4
#define SHADOW_MAP_SIZE       2048
#define ATTENUATION_CONSTANT  1.0f
#define ATTENUATION_LINEAR    0.09f
#define ATTENUATION_QUADRATIC 0.032f
#define INNER_CUTOFF          12.5f
#define OUTER_CUTOFF          17.5f
#define DEFAULT_DEPTH_BIAS    0.001f

#ifdef USE_NVIDIA
#ifndef PLATFORM_WEB
extern "C"
{
  __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif
#endif

// shader types
typedef enum shaderType
{
  LIGHT_SHADER       = 0,
  DEPTH_SHADER       = 1,
  SKYBOX_SHADER      = 2,
  CUBEMAP_SHADER     = 3,
  CONVOLUTION_SHADER = 4,
} shaderType;

typedef enum lightType
{
  DIRECTIONAL = 0,
  OMNILIGHT   = 1,
  SPOTLIGHT   = 2
} lightType;

struct EngineOptions
{
  char const *title;
  int         width;
  int         height;
  float       aspectRatio;
};
