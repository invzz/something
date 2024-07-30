#pragma once
class SkyBoxLocs
{
  public:
  int environmentMap;
  int doGamma;

  explicit SkyBoxLocs(Shader *shader)
  {
    // 1init(shader);
  }

  void init(Shader *shader)
  {
    environmentMap = GetShaderLocation(*shader, "environmentMap");
    doGamma        = GetShaderLocation(*shader, "doGamma");
  }
};


