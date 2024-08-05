#pragma once

#include "light.attribute.locs.hh"
#include "light.fragment.locs.hh"
#include "light.vertex.locs.hh"

class LightLocs
{
  public:
  int matLight;
  int shadowMap;
  int position;
  int direction;
  int color;
  int energy;
  int specular;
  int size;
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

  lightFragmentShaderLocs fs;
  LightVertexLocs         vs;
  lightAttributeLocs      as;

  void init(int index, Shader *shader)
  {
    matLight       = GetShaderLocation(*shader, TextFormat("matLights[%d]", index));
    shadowMap      = GetShaderLocation(*shader, TextFormat("lights[%d].shadowMap", index));
    position       = GetShaderLocation(*shader, TextFormat("lights[%d].position", index));
    direction      = GetShaderLocation(*shader, TextFormat("lights[%d].direction", index));
    color          = GetShaderLocation(*shader, TextFormat("lights[%d].color", index));
    energy         = GetShaderLocation(*shader, TextFormat("lights[%d].energy", index));
    specular       = GetShaderLocation(*shader, TextFormat("lights[%d].specular", index));
    size           = GetShaderLocation(*shader, TextFormat("lights[%d].size", index));
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
    fs.Init(shader);
    vs.init(shader);
    as.init(shader);
  }
};