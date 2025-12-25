#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform float u_time;
uniform float u_pulse;

void main() {
    // Orange/Gold color scheme (contrasts with cyan portal)
    vec3 coreColor = vec3(1.0, 0.6, 0.1);     // Orange
    vec3 glowColor = vec3(1.0, 0.8, 0.2);     // Golden
    
    // Fresnel effect (edges glow more)
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
    float fresnel = pow(1.0 - abs(dot(normalize(Normal), viewDir)), 3.0);
    
    // Pulsing animation
    float pulse = u_pulse;
    
    // Energy pattern using UV coordinates
    float pattern = sin(TexCoord.x * 10.0 + u_time * 2.0) * 
                    sin(TexCoord.y * 10.0 + u_time * 2.0);
    pattern = pattern * 0.5 + 0.5;
    
    // Combine effects
    vec3 finalColor = mix(coreColor, glowColor, fresnel);
    finalColor += glowColor * pattern * 0.3;
    finalColor *= pulse;
    
    // High alpha for glow
    float alpha = 0.8 + fresnel * 0.2;
    
    FragColor = vec4(finalColor, alpha);
}
