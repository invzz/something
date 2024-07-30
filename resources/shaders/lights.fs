#version 330

#define NUM_LIGHTS 4
#define NUM_MATERIAL_MAPS       7
#define NUM_MATERIAL_CUBEMAPS   2   

#define DIRLIGHT  0
#define OMNILIGHT 1
#define SPOTLIGHT 2

#define ALBEDO                  0
#define METALNESS               1
#define NORMAL                  2
#define ROUGHNESS               3
#define OCCLUSION               4
#define EMISSION                5
#define HEIGHT                  6

#define CUBEMAP                 0
#define IRRADIANCE              1

#define PI 3.1415926535897932384626433832795028

in vec4 shadowPos[NUM_LIGHTS];
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec4 fragColor;
flat in mat3 TBN;

out vec4 outColor;

struct MaterialMap {
    sampler2D texture;
    mediump vec4 color;
    mediump float value;
    lowp int isactive;
};

struct MaterialCubemap {
    samplerCube texture;
    mediump vec4 color;
    mediump float value;
    lowp int isactive;
};

struct Light {
    sampler2D shadowMap;
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    float innerCutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    float shadowMapTxlSz;
    float depthBias;
    lowp int type;
    lowp int shadow;
    lowp int enabled;
};

uniform MaterialCubemap cubemaps[NUM_MATERIAL_CUBEMAPS];
uniform MaterialMap maps[NUM_MATERIAL_MAPS];
uniform Light lights[NUM_LIGHTS];

uniform lowp int parallaxMinLayers;
uniform lowp int parallaxMaxLayers;

uniform lowp int useSpecularMap;
uniform lowp int useNormalMap;

uniform sampler2D texture0;   // diffuse
uniform sampler2D texture1;   // specular
uniform sampler2D texture2;   // normal

uniform vec3 colSpecular;     // sent by rlights
uniform vec4 colDiffuse;      // sent by raylib
uniform vec3 colAmbient;      // sent by rlights

uniform float shininess;
uniform vec3 viewPos;

float ShadowCalc(int i) {
    vec4 p = shadowPos[i];
    
    vec3 projCoords = p.xyz / p.w;
    
    projCoords = projCoords * 0.5 + 0.5;
    
    projCoords.z -= lights[i].depthBias;
    
    if(projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0) {
        return 1.0;
    }
    
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            float pcfDepth = texture(lights[i].shadowMap, projCoords.xy + vec2(x, y) * lights[i].shadowMapTxlSz).r;
            shadow += currentDepth > pcfDepth ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

vec3 gammaCorrection(vec3 colour, float gamma) {
    return pow(colour, vec3(1. / gamma));
}

float SchlickFresnel(float u) {
    float m = 1.0 - u;
    float m2 = m * m;
    return m2 *
    m2 * m; // pow(m,5)
}

float DistributionGGX(float cosTheta, float alpha) {
    float a = cosTheta * alpha;
    float k = alpha / (1.0 - cosTheta * cosTheta + a * a);
    return k * k * (1.0 / PI);
}

// From Earl Hammon, Jr. PBR Diffuse Lighting for GGX+Smith Microsurfaces
float GeometrySchlickGGX(float NdotV, float k) {
    return NdotV / (NdotV * (1.0 - k) + k);
}

// SEE: https://www.gdcvault.com/play/1024478/PBR-Diffuse-Lighting-for-GGX
float GeometrySmith(float NdotL, float NdotV, float alpha) {
    return 0.5 / mix(2.0 * NdotL * NdotV, NdotL + NdotV, alpha);
}

vec3 ComputeF0(float metallic, float specular, vec3 albedo) {
    float dielectric = 0.16 * specular * specular;
    // use albedo*metallic as colored specular reflectance at 0 angle for metallic materials
    // SEE: https://google.github.io/filament/Filament.md.html
    return mix(vec3(dielectric), albedo, vec3(metallic));
}

vec2 Parallax(vec2 uv, vec3 V) {
    float height = 1.0 - texture(maps[HEIGHT].texture, uv).r;
    return uv - vec2(V.xy / V.z) * height * maps[HEIGHT].value;
}

vec2 DeepParallax(vec2 uv, vec3 V) {
    float numLayers = mix(float(parallaxMaxLayers), float(parallaxMinLayers), abs(dot(vec3(0.0, 0.0, 1.0), V)));
    
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    vec2 P = V.xy / V.z * maps[HEIGHT].value;
    vec2 deltaTexCoord = P / numLayers;
    
    vec2 currentUV = uv;
    float currentDepthMapValue = 1.0 - texture(maps[HEIGHT].texture, currentUV).y;
    
    while(currentLayerDepth < currentDepthMapValue) {
        currentUV += deltaTexCoord;
        currentLayerDepth += layerDepth;
        currentDepthMapValue = 1.0 - texture(maps[HEIGHT].texture, currentUV).y;
    }
    
    vec2 prevTexCoord = currentUV - deltaTexCoord;
    float afterDepth = currentDepthMapValue + currentLayerDepth;
    float beforeDepth = 1.0 - texture(maps[HEIGHT].texture, prevTexCoord).y - currentLayerDepth - layerDepth;
    
    float weight = afterDepth / (afterDepth - beforeDepth);
    return prevTexCoord * weight + currentUV * (1.0 - weight);
}

void main() {
    // get texture samples
    vec3 diffSample = texture(texture0, fragTexCoord).rgb * colDiffuse.rgb * fragColor.rgb;
    vec3 specSample = (useSpecularMap != 0) ? texture(texture1, fragTexCoord).rgb * colSpecular : colSpecular;
    
    // compute normals
    vec3 normal;
    if(useNormalMap == 0)
    normal = normalize(fragNormal);
    else
    normal = normalize(TBN * (texture(texture2, fragTexCoord).rgb * 2.0 - 1.0));
    
    // compute current view dir for this frag
    vec3 viewDir = normalize(viewPos - fragPosition);
    
    // process lights
    vec3 finalColor = vec3(0.0);
    
    for(int i = 0; i < NUM_LIGHTS; i++) {
        // check if light is enabled
        if(lights[i].enabled != 0) {
            // get lightDir
            vec3 lightDir = (lights[i].type != DIRLIGHT) ? normalize(lights[i].position - fragPosition) : normalize(-lights[i].direction);
            
            // diffuse
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = lights[i].diffuse * diffSample * diff;
            
            // specular (Blinn-Phong)
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
            vec3 specular = lights[i].diffuse * specSample * spec;
            
            // spotlight
            float intensity = 1.0;
            
            if(lights[i].type == SPOTLIGHT) {
                float theta = dot(lightDir, normalize(-lights[i].direction));
                float epsilon = (lights[i].innerCutOff - lights[i].outerCutOff);
                intensity = smoothstep(0.0, 1.0, (theta - lights[i].outerCutOff) / epsilon);
            }
            
            // attenuation
            float distance = length(lights[i].position - fragPosition);
            float attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance));
            
            // shadow
            float currentShadow = (lights[i].shadow != 0) ? ShadowCalc(i) : 1.0;
            
            // add final light color
            finalColor += (diffuse + specular) * intensity * attenuation * currentShadow;
        }
    }
    // ambient
    
    vec3 ambientColor = colAmbient * diffSample;
    
    outColor = vec4(gammaCorrection((ambientColor * 0.1 + finalColor), 2.2), 1.0);
}