#version 330 core
out vec4 FragColor;

uniform vec2 playerPosition;
uniform float circleRadius;
uniform vec2 windowSize;
uniform mat3 view;
uniform mat3 projection;

void main() {
    vec2 fragPos = gl_FragCoord.xy;
    
    // Use camera-adjusted player position
    vec2 adjustedPlayerPos = playerPosition;
    
    // Calculate distance from fragment to player position
    float dist = length(fragPos - adjustedPlayerPos);
    
    float fadeWidth = 100.0;
    float fadeStart = circleRadius - fadeWidth;
    
    // Calculate visibility
    float visibility = 1.0 - smoothstep(fadeStart, circleRadius, dist);
    
    // Create the fog of war effect
    if (dist > circleRadius) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.98);
    } else if (dist > fadeStart) {
        float fade = smoothstep(fadeStart, circleRadius, dist);
        FragColor = vec4(0.0, 0.0, 0.0, 0.98 * fade);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
