class LightGeneralLocs
{
  public:
  int colAmbient; // Ambient color
  int viewPos;    // View position

  explicit LightGeneralLocs(Shader *shader)
  {
    // init(shader);
  }

  explicit LightGeneralLocs() {}

  void init(Shader *shader)
  {
    colAmbient = GetShaderLocation(*shader, "colAmbient");
    viewPos    = GetShaderLocation(*shader, "viewPos");
  }
};