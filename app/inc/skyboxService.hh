#include "common.hh"
#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"

class skyBoxServiceLocs
{
  public:
  int environmentMap;
  int doGamma;

  explicit skyBoxServiceLocs(Shader *shader)
  {
    // 1init(shader);
  }

  void init(Shader *shader)
  {
    environmentMap = GetShaderLocation(*shader, "environmentMap");
    doGamma        = GetShaderLocation(*shader, "doGamma");
  }
};

class cubemapLocs
{
  public:
  int equirectangularMap;

  explicit cubemapLocs(Shader *shader)
  {
    // 1init(shader);
  }

  void init(Shader *shader) { equirectangularMap = GetShaderLocation(*shader, "equirectangularMap"); }
};

class skyBoxService
{
  TextureCubemap cubemap;  // The skybox.
  cubemapLocs   *cubeLocs; // The cubemap locations.

  char               skyboxFileName[256] = {0}; // The filename of the skybox texture.
  Shader            *skyboxShader;              // The skybox shader
  Shader            *cubemapShader;             // The cubemap shader
  skyBoxServiceLocs *skyboxLocs;                // Shader locations

  bool useHDR = false; // Flag indicating if the skybox is HDR (high dynamic range).
                       // Define the positions of the vertices for a cube

  // Load skybox model
  Mesh  cube;
  Model skybox;

  void setHDR(int *useHDR) { SetShaderValue(*skyboxShader, skyboxLocs->doGamma, useHDR, SHADER_UNIFORM_INT); }

  void setEnvironmentMap(int *value) { SetShaderValue(*skyboxShader, skyboxLocs->environmentMap, value, SHADER_UNIFORM_INT); }

  void setEquirectangularMap(int *value) { SetShaderValue(*cubemapShader, cubeLocs->equirectangularMap, value, SHADER_UNIFORM_INT); }

  void LoadSkyBox(Image *img)
  {
    setEnvironmentMap((int[1]){MATERIAL_MAP_CUBEMAP});
    setHDR((int[1]){useHDR ? 1 : 0});
    setEquirectangularMap((int[1]){0});
    skybox.materials[0].shader                             = *skyboxShader;
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(*img, CUBEMAP_LAYOUT_AUTO_DETECT); // CUBEMAP_LAYOUT_PANORAMA
  }

  public:
  skyBoxService(const char *filename, Shader *skyboxShader, Shader *cubeShader)
      : skyboxShader(skyboxShader), skyboxLocs(new skyBoxServiceLocs(skyboxShader)), cubemapShader(cubeShader), cubeLocs(new cubemapLocs(cubeShader))
  {
    cube   = GenMeshCube(1.0f, 1.0f, 1.0f);
    skybox = LoadModelFromMesh(cube);
    skyboxLocs->init(skyboxShader);
    cubeLocs->init(cubemapShader);
    Image img = LoadImage(filename);
    LoadSkyBox(&img);
    UnloadImage(img);
  };

  void LoadSkyBox(const char *filename)
  {
    Image img = LoadImage(filename);
    LoadSkyBox(&img);
    UnloadImage(img);
  }

  void DrawSkybox()
  {
    // We are inside the cube, we need to disable backface culling!
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
  }

  void AddActions() {}

  ~skyBoxService() {}
};