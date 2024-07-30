#pragma once
class LightLocs
{
  public:
  int vpMatrix;
  int shadowMap;
  int position;
  int direction;
  int diffuse;
  int specular;
  int innerCutOff;
  int outerCutOff;
  int constant;
  int linear;
  int quadratic;
  int shadowMapTxlSz;
  int depthBias;
  int type;
  int shadow;
  int enabled;

  void init(int index, Shader *shader)
  {
    vpMatrix       = GetShaderLocation(*shader, TextFormat("matLights[%d]", index));
    shadowMap      = GetShaderLocation(*shader, TextFormat("lights[%d].shadowMap", index));
    position       = GetShaderLocation(*shader, TextFormat("lights[%d].position", index));
    direction      = GetShaderLocation(*shader, TextFormat("lights[%d].direction", index));
    diffuse        = GetShaderLocation(*shader, TextFormat("lights[%d].diffuse", index));
    specular       = GetShaderLocation(*shader, TextFormat("lights[%d].specular", index));
    innerCutOff    = GetShaderLocation(*shader, TextFormat("lights[%d].innerCutOff", index));
    outerCutOff    = GetShaderLocation(*shader, TextFormat("lights[%d].outerCutOff", index));
    constant       = GetShaderLocation(*shader, TextFormat("lights[%d].constant", index));
    linear         = GetShaderLocation(*shader, TextFormat("lights[%d].linear", index));
    quadratic      = GetShaderLocation(*shader, TextFormat("lights[%d].quadratic", index));
    shadowMapTxlSz = GetShaderLocation(*shader, TextFormat("lights[%d].shadowMapTxlSz", index));
    depthBias      = GetShaderLocation(*shader, TextFormat("lights[%d].depthBias", index));
    type           = GetShaderLocation(*shader, TextFormat("lights[%d].type", index));
    shadow         = GetShaderLocation(*shader, TextFormat("lights[%d].shadow", index));
    enabled        = GetShaderLocation(*shader, TextFormat("lights[%d].enabled", index));
  }
};