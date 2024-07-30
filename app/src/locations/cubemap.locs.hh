#pragma once
class cubemapLocs
{
  public:
  int equirectangularMap;

  explicit cubemapLocs(Shader *shader)
  {
    // 1init(shader);
  }

  void init(Shader *shader) { equirectangularMap = GetShaderLocation(*shader, "equirectangularMap"); }
};