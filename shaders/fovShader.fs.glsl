#version 330 core
out vec4 FragColor;

uniform vec2 playerPosition;
uniform float circleRadius;
uniform vec2 windowSize;
uniform bool is_text;
uniform float time;

void main() {
    // Always make text visible by not applying darkness to it
    if (is_text) {  // Changed from is_text == 1 to just is_text
        discard;    // Makes the FOV effect completely transparent for text
        return;
    }
    
    vec2 fragPos = gl_FragCoord.xy;
    float dist = length(fragPos - playerPosition);
    
    float fadeWidth = 100.0;
    float fadeStart = circleRadius - fadeWidth*(0.5*sin(0.1*time)+2);
    
    if (dist > circleRadius) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.98);
    } else if (dist > fadeStart) {
        float fade = smoothstep(fadeStart, circleRadius, dist);
        FragColor = vec4(0.0, 0.0, 0.0, 0.98 * fade);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}

//void main() {
    // Get current fragment position
  //  vec2 fragPos = gl_FragCoord.xy;
    //float dist = length(fragPos - playerPosition);
    
    // If this is text OR within circle radius, make transparent
    //if (is_text || dist <= circleRadius) {
      //  discard;  // Make completely transparent
       // return;
    //}
    
    // Everything else gets the darkness
    //FragColor = vec4(0.0, 0.0, 0.0, 0.98);
//}