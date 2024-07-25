#include <raylib.h>
#include "engine.hh"
#include "common.hh"
#include "ecamera.hh"

Model ground;
Model Knot;
Model cube;
Model sphere;

void TestScene(Shader *s)
{
  bool isSet = ground.materials[0].shader.id == 0;

  if(s != nullptr && !isSet)
    {
      ground.materials[0].shader = *s;
      sphere.materials[0].shader = *s;
      Knot.materials[0].shader   = *s;
      cube.materials[0].shader   = *s;
    }
  DrawModel(ground, (Vector3){0, -0.5, 0}, 2.0f, WHITE);
  DrawModel(cube, (Vector3){0, 0.5, 0}, 1.0f, BLUE);
  DrawModel(Knot, (Vector3){2, 0.5, 0}, 0.2f, RED);
  DrawModel(sphere, (Vector3){-2, 0.5, 0}, 1.0f, YELLOW);
}

int main()
{
  EngineOptions options = (EngineOptions){"Engine", SCREEN_WIDTH, SCREEN_HEIGHT, ASPECT_RATIO};
  Engine        engine(options, TestScene);
  Texture2D     albedo = LoadTexture(TEXTURE_DIR "gold/albedo.png");
  engine.SetScene(TestScene);
  ground                                                = LoadModelFromMesh(GenMeshPlane(10, 10, 1, 1));
  sphere                                                = LoadModelFromMesh(GenMeshSphere(0.5, 32, 32));
  Knot                                                  = LoadModelFromMesh(GenMeshKnot(1, 3.0f, 25, 150));
  cube                                                  = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
  sphere.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = albedo;
  Knot.materials[0].maps[MATERIAL_MAP_ALBEDO].texture   = albedo;
  cube.materials[0].maps[MATERIAL_MAP_ALBEDO].texture   = albedo;
  ground.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = albedo;
  engine.Run();
  UnloadTexture(albedo);
  UnloadModel(ground);
  UnloadModel(sphere);
  UnloadModel(Knot);
  UnloadModel(cube);
  CloseWindow();
  return 0;
}