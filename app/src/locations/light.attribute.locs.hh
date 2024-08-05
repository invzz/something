#pragma once

class lightAttributeLocs
{
  public:
  int position;
  int texcoord01;
  int texcoord02;
  int normal;
  int tangent;
  int color;

  void init(Shader *s)
  {
    position   = rlGetLocationAttrib(s->id, "vertexPosition");
    texcoord01 = rlGetLocationAttrib(s->id, "vertexTexCoord");
    // texcoord02 = rlGetLocationAttrib(s->id, "vertexTexCoord2");
    normal  = rlGetLocationAttrib(s->id, "vertexNormal");
    tangent = rlGetLocationAttrib(s->id, "vertexTangent");
    color   = rlGetLocationAttrib(s->id, "vertexColor");
  }
};