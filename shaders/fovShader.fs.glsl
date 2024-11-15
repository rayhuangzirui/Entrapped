#version 330 core
out vec4 FragColor;

uniform vec2 playerPosition;
uniform float circleRadius;
uniform vec2 windowSize;
uniform bool is_text;

void main() {
    
    if (is_text) {
        discard;  
        return;
    }
    
    vec2 fragPos = gl_FragCoord.xy;
    float dist = length(fragPos - playerPosition);
    
    float fadeWidth = 50.0;  // Can adjust this for softer/harder edges
    float fadeStart = circleRadius - fadeWidth;
    
    if (dist > circleRadius) {
        // Outside circle - fully dark
        FragColor = vec4(0.0, 0.0, 0.0, 0.98);
    } else if (dist > fadeStart) {
        // Edge fade
       float fade = smoothstep(fadeStart, circleRadius, dist);
        FragColor = vec4(0.0, 0.0, 0.0, 0.98 * fade);
    } else {
        // Inside circle - fully transparent
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