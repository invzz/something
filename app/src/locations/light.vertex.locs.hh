#pragma once

class LightVertexLocs
{
  public:
  int mvp;
  int matView;
  int matProjection;
  int matModel;
  int matNormal;

  void init(Shader *s)
  {
    mvp           = rlGetLocationUniform(s->id, "mvp");
    matView       = rlGetLocationUniform(s->id, "matView");
    matProjection = rlGetLocationUniform(s->id, "matProjection");
    matModel      = rlGetLocationUniform(s->id, "matModel");
    matNormal     = rlGetLocationUniform(s->id, "matNormal");
  }
};