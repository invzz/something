
#pragma once

class lightFragmentShaderLocs
{
  public:
  int ambient;
  int view;
  int diffuse;
  int specular;
  int emission;
  int albedo;
  int metalness;
  int normal;
  int roughness;
  int occlusion;
  int height;
  int brdf;
  int cubemap;
  int irradiance;
  int prefilter;
  int metalness_scale;
  int roughness_scale;
  int ao_light_affect;
  int height_scale;

  void Init(Shader *s)
  {
    ambient         = rlGetLocationUniform(s->id, "colAmbient");
    view            = rlGetLocationUniform(s->id, "viewPos");
    diffuse         = rlGetLocationUniform(s->id, TextFormat("maps[%i].color", MATERIAL_MAP_ALBEDO));
    specular        = rlGetLocationUniform(s->id, TextFormat("maps[%i].color", MATERIAL_MAP_METALNESS));
    emission        = rlGetLocationUniform(s->id, TextFormat("maps[%i].color", MATERIAL_MAP_EMISSION));
    albedo          = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_ALBEDO));
    metalness       = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_METALNESS));
    normal          = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_NORMAL));
    roughness       = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_ROUGHNESS));
    occlusion       = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_OCCLUSION));
    emission        = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_EMISSION));
    height          = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_HEIGHT));
    brdf            = rlGetLocationUniform(s->id, TextFormat("maps[%i].texture", MATERIAL_MAP_HEIGHT + 1));
    cubemap         = rlGetLocationUniform(s->id, "cubemaps[0].texture");
    irradiance      = rlGetLocationUniform(s->id, "cubemaps[1].texture");
    prefilter       = rlGetLocationUniform(s->id, "cubemaps[2].texture");
    metalness_scale = rlGetLocationUniform(s->id, TextFormat("maps[%i].value", MATERIAL_MAP_METALNESS));
    roughness_scale = rlGetLocationUniform(s->id, TextFormat("maps[%i].value", MATERIAL_MAP_ROUGHNESS));
    ao_light_affect = rlGetLocationUniform(s->id, TextFormat("maps[%i].value", MATERIAL_MAP_OCCLUSION));
    height_scale    = rlGetLocationUniform(s->id, TextFormat("maps[%i].value", MATERIAL_MAP_HEIGHT));
  }
};