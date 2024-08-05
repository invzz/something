#include "config.hh"
#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"

class SkyboxService
{
  TextureCubemap cubeMap;
  TextureCubemap IrradMap;
  char           skyboxFileName[256] = {0};
  Shader        *skyboxShader;
  Shader        *cubemapShader;
  Shader        *convolutionShader;
  SkyBoxLocs    *skyboxLocs;
  cubemapLocs   *cubeLocs;
  bool           useHDR = false;
  Mesh           cube;
  Model          skybox;

  void setHDR(int *useHDR) { SetShaderValue(*skyboxShader, skyboxLocs->doGamma, useHDR, SHADER_UNIFORM_INT); }
  void setEnvironmentMap(int *value) { SetShaderValue(*skyboxShader, skyboxLocs->environmentMap, value, SHADER_UNIFORM_INT); }
  void setEquirectangularMap(int *value) { SetShaderValue(*cubemapShader, cubeLocs->equirectangularMap, value, SHADER_UNIFORM_INT); }
  void load(Image *img)
  {
    setEnvironmentMap((int[1]){MATERIAL_MAP_CUBEMAP});
    setHDR((int[1]){useHDR ? 1 : 0});
    setEquirectangularMap((int[1]){0});
    cubeMap = LoadTextureCubemap(*img, CUBEMAP_LAYOUT_AUTO_DETECT);
    CreateIrradianceMap();
  }
  void CreateIrradianceMap()
  {
    int size         = cubeMap.width / 16;
    size             = (size < 8) ? 8 : size;
    unsigned int rbo = rlLoadTextureDepth(size, size, true);
    IrradMap.id      = rlLoadTextureCubemap(NULL, size, cubeMap.format);
    rlCubemapParameters(IrradMap.id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_LINEAR);
    rlCubemapParameters(IrradMap.id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);
    unsigned int fbo = rlLoadFramebuffer();
    rlFramebufferAttach(fbo, rbo, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
    rlFramebufferAttach(fbo, IrradMap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X, 0);
    if(rlFramebufferComplete(fbo)) { TraceLog(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", fbo); }
    rlEnableShader(convolutionShader->id);
    Matrix matFboProjection = MatrixPerspective(90.0 * DEG2RAD, 1.0, 0.1, 10.0);
    rlSetUniformMatrix(convolutionShader->locs[SHADER_LOC_MATRIX_PROJECTION], matFboProjection);
    Matrix fboViews[6] = {MatrixLookAt(Vector3Zero(), (Vector3){1.0f, 0.0f, 0.0f}, (Vector3){0.0f, -1.0f, 0.0f}),
                          MatrixLookAt(Vector3Zero(), (Vector3){-1.0f, 0.0f, 0.0f}, (Vector3){0.0f, -1.0f, 0.0f}),
                          MatrixLookAt(Vector3Zero(), (Vector3){0.0f, 1.0f, 0.0f}, (Vector3){0.0f, 0.0f, 1.0f}),
                          MatrixLookAt(Vector3Zero(), (Vector3){0.0f, -1.0f, 0.0f}, (Vector3){0.0f, 0.0f, -1.0f}),
                          MatrixLookAt(Vector3Zero(), (Vector3){0.0f, 0.0f, 1.0f}, (Vector3){0.0f, -1.0f, 0.0f}),
                          MatrixLookAt(Vector3Zero(), (Vector3){0.0f, 0.0f, -1.0f}, (Vector3){0.0f, -1.0f, 0.0f})};
    rlViewport(0, 0, size, size);
    rlDisableBackfaceCulling();
    rlActiveTextureSlot(0);
    rlEnableTextureCubemap(cubeMap.id);
    for(int i = 0; i < 6; i++)
      {
        rlSetUniformMatrix(convolutionShader->locs[SHADER_LOC_MATRIX_VIEW], fboViews[i]);
        rlFramebufferAttach(fbo, IrradMap.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_CUBEMAP_POSITIVE_X + i, 0);
        rlEnableFramebuffer(fbo);
        rlClearScreenBuffers();
        rlLoadDrawCube();
      }
    rlDisableShader();
    rlDisableTextureCubemap();
    rlDisableFramebuffer();
    rlUnloadFramebuffer(fbo);
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    rlEnableBackfaceCulling();
  }

  public:
  SkyboxService(const char *filename, Shader *skyboxShader, Shader *cubeShader, Shader *conv)
      : skyboxShader(skyboxShader), skyboxLocs(new SkyBoxLocs()), cubemapShader(cubeShader), cubeLocs(new cubemapLocs()), convolutionShader(conv)
  {
    cube   = GenMeshCube(1.0f, 1.0f, 1.0f);
    skybox = LoadModelFromMesh(cube);
    skyboxLocs->init(skyboxShader);
    cubeLocs->init(cubemapShader);
    Image img = LoadImage(filename);
    load(&img);
    skybox.materials[0].shader                             = *skyboxShader;
    skybox.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = cubeMap;
    UnloadImage(img);
  };

  void load(const char *filename)
  {
    Image img = LoadImage(filename);
    load(&img);
    UnloadImage(img);
  }

  void unload()
  {
    UnloadTexture(cubeMap);
    UnloadTexture(IrradMap);
  }

  void draw()
  {
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);
    rlEnableBackfaceCulling();
    rlEnableDepthMask();
  }

  void AddActions() {}

  ~SkyboxService()
  {
    unload();
    UnloadModel(skybox);
    delete skyboxLocs;
    delete cubeLocs;
  }
};