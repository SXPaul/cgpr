#version 410 core

/*
 * PBR Fragment Shader - Physically Based Rendering using Cook-Torrance BRDF
 * 
 * ============================================================================
 * MATHEMATICAL FOUNDATION (for understanding and exam questions)
 * ============================================================================
 * 
 * The Rendering Equation (simplified for direct lighting):
 *   L_out = integral over hemisphere { f_r * L_in * cos(theta) dw }
 * 
 * For a single light, this simplifies to:
 *   L_out = f_r(l, v) * L_in * (n · l)
 * 
 * where:
 *   f_r = BRDF (Bidirectional Reflectance Distribution Function)
 *   L_in = incoming light radiance
 *   n = surface normal
 *   l = light direction
 *   v = view direction
 * 
 * ----------------------------------------------------------------------------
 * Cook-Torrance BRDF:
 * ----------------------------------------------------------------------------
 *   f_r = k_d * f_lambert + k_s * f_cook-torrance
 * 
 *   f_lambert = albedo / PI                    (diffuse)
 *   f_cook-torrance = DFG / (4 * (n·v) * (n·l)) (specular)
 * 
 * The specular term has three components:
 * 
 * 1. D - Normal Distribution Function (NDF):
 *    How microfacets are distributed. We use GGX/Trowbridge-Reitz:
 *    
 *    D(h) = alpha^2 / (PI * ((n·h)^2 * (alpha^2 - 1) + 1)^2)
 *    
 *    where alpha = roughness^2, h = half vector = normalize(l + v)
 * 
 * 2. F - Fresnel Equation:
 *    How reflectance changes at different angles. Using Schlick's approximation:
 *    
 *    F(v,h) = F0 + (1 - F0) * (1 - v·h)^5
 *    
 *    F0 = base reflectivity (0.04 for dielectrics, albedo for metals)
 * 
 * 3. G - Geometry Function:
 *    Microfacet self-shadowing. Using Smith's method with GGX:
 *    
 *    G(n,v,l) = G_sub(n,v) * G_sub(n,l)
 *    G_sub(n,x) = (n·x) / ((n·x) * (1 - k) + k)
 *    
 *    where k = (roughness + 1)^2 / 8  (for direct lighting)
 * 
 * ----------------------------------------------------------------------------
 * Energy Conservation:
 * ----------------------------------------------------------------------------
 *   k_s = F  (Fresnel gives us the specular reflection coefficient)
 *   k_d = (1 - k_s) * (1 - metallic)  (what's not reflected is refracted)
 *   
 *   Metals absorb refracted light, so diffuse = 0 for pure metals.
 * 
 * ============================================================================
 */

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

// Material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// Texture maps
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D armMap;  // Combined: R=AO, G=Roughness, B=Metallic

uniform bool useAlbedoMap;
uniform bool useNormalMap;
uniform bool useARMMap;

// Lights (supporting up to 4 lights for now)
#define MAX_LIGHTS 4
uniform int numLights;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
uniform int lightTypes[MAX_LIGHTS]; // 0 = point, 1 = directional
uniform vec3 lightDirections[MAX_LIGHTS];

// Camera
uniform vec3 camPos;

// Environment
uniform vec3 ambientColor;

// Constants
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// PBR Functions
// ----------------------------------------------------------------------------

// Normal Distribution Function: GGX/Trowbridge-Reitz
// Describes the density of microfacets oriented towards the half vector
float DistributionGGX(vec3 N, vec3 H, float rough) {
    float a = rough * rough;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0001);
}

// Geometry function: Schlick-GGX (single direction)
// Accounts for microfacet self-shadowing
float GeometrySchlickGGX(float NdotV, float rough) {
    float r = (rough + 1.0);
    float k = (r * r) / 8.0;  // k for direct lighting

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith's method: combines view and light shadowing
float GeometrySmith(vec3 N, vec3 V, vec3 L, float rough) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, rough);
    float ggx2 = GeometrySchlickGGX(NdotL, rough);
    return ggx1 * ggx2;
}

// Fresnel equation: Schlick's approximation
// Describes how reflectivity changes at grazing angles
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

void main() {
    // Sample textures or use uniform values
    vec3 albedoVal = useAlbedoMap ? pow(texture(albedoMap, fs_in.TexCoords).rgb, vec3(2.2)) : albedo;
    
    // Material properties from ARM map or uniform fallback
    float metallicVal = metallic;
    float roughnessVal = roughness;
    float aoVal = ao;
    
    if (useARMMap) {
        // ARM texture: R=AO, G=Roughness, B=Metallic
        vec3 arm = texture(armMap, fs_in.TexCoords).rgb;
        aoVal = arm.r;
        roughnessVal = arm.g;
        metallicVal = arm.b;
    }
    
    // Normal mapping
    vec3 N;
    if (useNormalMap) {
        vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;
        N = normalize(fs_in.TBN * tangentNormal);
    } else {
        N = normalize(fs_in.Normal);
    }
    
    vec3 V = normalize(camPos - fs_in.FragPos);

    // Base reflectivity: 0.04 for dielectrics, albedo for metals
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoVal, metallicVal);

    // Accumulate lighting from all light sources
    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
        vec3 L;
        float attenuation;
        
        if (lightTypes[i] == 1) {
            // Directional light
            L = normalize(-lightDirections[i]);
            attenuation = 1.0;
        } else {
            // Point light
            L = normalize(lightPositions[i] - fs_in.FragPos);
            float distance = length(lightPositions[i] - fs_in.FragPos);
            attenuation = 1.0 / (distance * distance);  // Inverse square falloff
        }
        
        vec3 H = normalize(V + L);
        vec3 radiance = lightColors[i] * lightIntensities[i] * attenuation;

        // Calculate Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughnessVal);
        float G = GeometrySmith(N, V, L, roughnessVal);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        // Specular contribution
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        // Energy conservation: kS + kD = 1
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallicVal;  // Metals have no diffuse

        // Lambertian diffuse
        float NdotL = max(dot(N, L), 0.0);

        Lo += (kD * albedoVal / PI + specular) * radiance * NdotL;
    }

    // Ambient lighting (simple constant ambient for now)
    vec3 ambient = ambientColor * albedoVal * aoVal;

    vec3 color = ambient + Lo;

    // HDR tone mapping (Reinhard)
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
