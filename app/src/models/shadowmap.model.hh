#pragma once

#include <rlgl.h>
#include <raymath.h>
#include <raylib.h>
#include <vector>
#include <functional>

class ShadowMap
{
  public:
  unsigned int id;
  int          width;
  int          height;
  Texture2D    texture;

  ShadowMap()
  {
    this->width  = SHADOW_MAP_SIZE;
    this->height = SHADOW_MAP_SIZE;
    Load();
  }

  ~ShadowMap() { Unload(); }

  void Load()
  {
    this->id = rlLoadFramebuffer();

    rlEnableFramebuffer(this->id);

    this->texture.id      = rlLoadTextureDepth(this->width, this->height, false);
    this->texture.width   = this->width;
    this->texture.height  = this->height;
    this->texture.format  = PIXELFORMAT_COMPRESSED_ETC2_RGB;
    this->texture.mipmaps = 1;

    rlTextureParameters(this->texture.id, RL_TEXTURE_WRAP_S, RL_TEXTURE_WRAP_CLAMP);
    rlTextureParameters(this->texture.id, RL_TEXTURE_WRAP_T, RL_TEXTURE_WRAP_CLAMP);
    rlFramebufferAttach(this->id, this->texture.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);
    rlDisableFramebuffer();
  }

  void Unload()
  {
    rlUnloadFramebuffer(this->id);
    rlUnloadTexture(this->texture.id);
  }

  void Begin()
  {
    rlDrawRenderBatchActive();
    rlEnableFramebuffer(this->id);
    rlClearScreenBuffers();
    rlViewport(0, 0, this->width, this->height);    // Set viewport to framebuffer size
    rlMatrixMode(RL_PROJECTION);                    // Switch to projection matrix
    rlLoadIdentity();                               // Reset current matrix (projection)
    rlOrtho(0, this->width, this->height, 0, 0, 1); // Set orthographic projection to current framebuffer size
    rlMatrixMode(RL_MODELVIEW);                     // Switch back to modelview matrix
    rlLoadIdentity();                               // Reset current matrix (modelview)
    rlClearColor(255, 255, 255, 255);
    rlDisableColorBlend();
  }

  void End()
  {
    rlEnableColorBlend();
    rlDisableTexture();

    rlDrawRenderBatchActive();
    rlDisableFramebuffer();

    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());

    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();

    rlOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, 0, 1);

    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
  }
};