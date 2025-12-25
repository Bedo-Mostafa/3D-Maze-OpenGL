#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Textures
uniform sampler2D textureSampler;

// Camera/View
uniform vec3 viewPos;

// Lighting
uniform vec3 lightPos;
uniform vec3 lightColor;

// Material properties
struct Material {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};
uniform Material material;

// === NEW: FLASHLIGHT ===
uniform bool u_flashlightEnabled;
uniform vec3 u_flashlightPos;
uniform vec3 u_flashlightDir;
uniform float u_flashlightCutoff;      // Inner cone angle (cos)
uniform float u_flashlightOuterCutoff; // Outer cone angle (cos)
uniform float u_flashlightIntensity;

// === NEW: FOG ===
uniform bool u_fogEnabled;
uniform vec3 u_fogColor;
uniform float u_fogDensity;
uniform float u_fogStart;
uniform float u_fogEnd;

// === NEW: PORTAL LIGHT ===
uniform bool u_portalLightEnabled;
uniform vec3 u_portalPos;
uniform vec3 u_portalColor;
uniform float u_portalRadius;

void main() {
    // Sample texture
    vec4 texColor = texture(textureSampler, TexCoord);
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // =====================================================
    // 1. AMBIENT LIGHTING (Base darkness)
    // =====================================================
    vec3 ambient = material.ambient * lightColor * texColor.rgb;
    
    // =====================================================
    // 2. DIFFUSE LIGHTING (Overhead light - very subtle)
    // =====================================================
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse * diff * lightColor * texColor.rgb;
    
    // =====================================================
    // 3. SPECULAR LIGHTING (Subtle highlights)
    // =====================================================
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * lightColor;
    
    // =====================================================
    // 4. FLASHLIGHT (Main light source)
    // =====================================================
    vec3 flashlightContribution = vec3(0.0);
    
    if (u_flashlightEnabled) {
        vec3 flashDir = normalize(u_flashlightPos - FragPos);
        float distance = length(u_flashlightPos - FragPos);
        
        // Spotlight cone calculation
        float theta = dot(flashDir, normalize(-u_flashlightDir));
        float epsilon = u_flashlightCutoff - u_flashlightOuterCutoff;
        float intensity = clamp((theta - u_flashlightOuterCutoff) / epsilon, 0.0, 1.0);
        
        // Attenuation (distance falloff)
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        // Flashlight diffuse
        float flashDiff = max(dot(norm, flashDir), 0.0);
        vec3 flashDiffuse = flashDiff * texColor.rgb * u_flashlightIntensity;
        
        // Flashlight specular
        vec3 flashReflect = reflect(-flashDir, norm);
        float flashSpec = pow(max(dot(viewDir, flashReflect), 0.0), material.shininess * 2.0);
        vec3 flashSpecular = flashSpec * vec3(0.3);
        
        flashlightContribution = (flashDiffuse + flashSpecular) * intensity * attenuation;
    }
    
    // =====================================================
    // 5. PORTAL LIGHT (Cyan glow on nearby surfaces)
    // =====================================================
    vec3 portalContribution = vec3(0.0);
    
    if (u_portalLightEnabled) {
        vec3 portalDir = u_portalPos - FragPos;
        float portalDist = length(portalDir);
        portalDir = normalize(portalDir);
        
        // Only light surfaces within range
        if (portalDist < u_portalRadius) {
            float portalDiff = max(dot(norm, portalDir), 0.0);
            float portalAttenuation = 1.0 - (portalDist / u_portalRadius);
            portalAttenuation = portalAttenuation * portalAttenuation; // Quadratic falloff
            
            portalContribution = u_portalColor * portalDiff * portalAttenuation * 0.8;
        }
    }
    
    // =====================================================
    // 6. COMBINE ALL LIGHTING
    // =====================================================
    vec3 result = ambient + diffuse + specular + flashlightContribution + portalContribution;
    
    // =====================================================
    // 7. FOG (Distance-based darkness)
    // =====================================================
    if (u_fogEnabled) {
        float distance = length(viewPos - FragPos);
        
        // Linear fog
        float fogFactor = (u_fogEnd - distance) / (u_fogEnd - u_fogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        
        // Exponential fog (optional, looks better)
        // fogFactor = exp(-u_fogDensity * distance);
        // fogFactor = clamp(fogFactor, 0.0, 1.0);
        
        result = mix(u_fogColor, result, fogFactor);
    }
    
    FragColor = vec4(result, 1.0);
}
