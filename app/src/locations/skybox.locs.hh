#pragma once
class SkyBoxLocs
{
  public:
  int environmentMap;
  int doGamma;

  void init(Shader *shader)
  {
    environmentMap = GetShaderLocation(*shader, "environmentMap");
    doGamma        = GetShaderLocation(*shader, "doGamma");
  }
};


