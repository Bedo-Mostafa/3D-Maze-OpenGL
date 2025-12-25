#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 WorldPos;

uniform sampler2D textureSampler;
uniform float u_time;
uniform vec3 u_portalCenter; // Center position of the portal

// Noise function for procedural effects
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Fractal Brownian Motion for complex patterns
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for(int i = 0; i < 4; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    // Calculate distance from portal center (horizontal)
    vec2 centerXZ = u_portalCenter.xz;
    vec2 fragXZ = WorldPos.xz;
    float distFromCenter = length(fragXZ - centerXZ);
    
    // Height of fragment (Y coordinate)
    float height = WorldPos.y;
    float heightNorm = height / 2.5; // Normalize by portal height
    
    // Base colors - bright cyan/blue
    vec3 coreColor = vec3(0.0, 0.8, 1.0);      // Bright cyan
    vec3 glowColor = vec3(0.2, 0.6, 1.0);      // Blue
    vec3 particleColor = vec3(0.8, 1.0, 1.0);  // White-cyan
    
    // === VERTICAL LIGHT BEAMS ===
    float angle = atan(fragXZ.y - centerXZ.y, fragXZ.x - centerXZ.x);
    float beamCount = 8.0;
    float beamPattern = sin(angle * beamCount + u_time * 2.0) * 0.5 + 0.5;
    
    // Create sharp beams that pulse
    float beamIntensity = pow(beamPattern, 3.0);
    beamIntensity *= (1.0 - smoothstep(0.6, 1.0, distFromCenter));
    beamIntensity *= smoothstep(0.0, 0.2, heightNorm) * smoothstep(1.0, 0.8, heightNorm);
    
    // === RISING PARTICLES ===
    vec2 particleUV = vec2(angle / 6.28318, heightNorm - u_time * 0.3);
    float particles = fbm(particleUV * 10.0);
    particles *= fbm(particleUV * 5.0 + vec2(u_time * 0.5, 0.0));
    
    // Make particles appear as bright dots
    particles = pow(particles, 4.0);
    particles *= (1.0 - smoothstep(0.5, 0.8, distFromCenter));
    
    // === RADIAL ENERGY RINGS ===
    float rings = sin(distFromCenter * 10.0 - u_time * 3.0) * 0.5 + 0.5;
    rings *= (1.0 - smoothstep(0.0, 0.8, distFromCenter));
    rings *= smoothstep(0.8, 0.0, heightNorm); // Only near bottom
    
    // === PULSING GROUND PLATFORM ===
    float groundPulse = 0.0;
    if(heightNorm < 0.1) {
        groundPulse = (1.0 - distFromCenter / 0.8);
        groundPulse = clamp(groundPulse, 0.0, 1.0);
        groundPulse *= (0.7 + 0.3 * sin(u_time * 2.0));
    }
    
    // === CORE CYLINDER GLOW ===
    float cylinderGlow = 1.0 - smoothstep(0.2, 0.8, distFromCenter);
    cylinderGlow *= (0.6 + 0.4 * sin(u_time * 1.5 + heightNorm * 3.14159));
    
    // === COMBINE ALL EFFECTS ===
    vec3 finalColor = vec3(0.0);
    
    // Core cylinder with flowing texture
    vec2 flowingUV = TexCoord;
    flowingUV.y -= u_time * 0.3;
    vec4 texColor = texture(textureSampler, flowingUV);
    finalColor += texColor.rgb * coreColor * cylinderGlow * 0.8;
    
    // Add vertical beams (brightest)
    finalColor += coreColor * beamIntensity * 2.0;
    
    // Add particles (very bright)
    finalColor += particleColor * particles * 3.0;
    
    // Add energy rings
    finalColor += glowColor * rings * 0.8;
    
    // Add ground platform glow
    finalColor += coreColor * groundPulse * 1.5;
    
    // === FRESNEL EFFECT (edges glow more) ===
    vec3 viewDir = normalize(vec3(0.0, 1.0, 0.0)); // Simplified view direction
    float fresnel = pow(1.0 - abs(dot(Normal, viewDir)), 2.0);
    finalColor += coreColor * fresnel * 0.5;
    
    // === CALCULATE ALPHA ===
    float alpha = cylinderGlow * 0.7;
    alpha += beamIntensity * 0.9;
    alpha += particles * 0.95;
    alpha += rings * 0.6;
    alpha += groundPulse * 0.8;
    alpha = clamp(alpha, 0.0, 1.0);
    
    // Add overall pulse to everything
    float globalPulse = 0.8 + 0.2 * sin(u_time * 1.5);
    finalColor *= globalPulse;
    
    // Output with HDR-like brightness
    FragColor = vec4(finalColor, alpha);
}
