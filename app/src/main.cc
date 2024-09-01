#include <raylib.h>
#include "engine.model.hh"

Model cube;

void TestScene(Shader *s)
{
  bool isSet = s != nullptr;

  if(isSet) { cube.materials[0].shader = *s; }

  DrawModel(cube, (Vector3){0, 0.5, 0}, 1.0f, WHITE);
}

int main()
{
  EngineOptions options = (EngineOptions){"Engine", SCREEN_WIDTH, SCREEN_HEIGHT, ASPECT_RATIO};
  Engine        engine(options, TestScene);

  engine.SetScene(TestScene);

  cube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

  Texture2D albedo = LoadTexture(TEXTURE_DIR "gold/albedo.png");

  cube.materials[0].maps[MATERIAL_MAP_METALNESS].value = 1.0f;
  cube.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 1.0f;

  // albedo
  cube.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = albedo;
  SetTextureFilter(cube.materials[0].maps[MATERIAL_MAP_ALBEDO].texture, TEXTURE_FILTER_BILINEAR);
  GenTextureMipmaps(&cube.materials[0].maps[MATERIAL_MAP_ALBEDO].texture);

  engine.Run();

  UnloadTexture(albedo);

  UnloadModel(cube);
  CloseWindow();
  return 0;
}