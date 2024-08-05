#pragma once
class cubemapLocs
{
  public:
  int equirectangularMap;

   void init(Shader *shader) { equirectangularMap = GetShaderLocation(*shader, "equirectangularMap"); }
};