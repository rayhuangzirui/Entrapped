#version 330
#define MAP_LEN 30

// From Vertex Shader
in vec3 vcolor;
in vec2 tex_position;
in vec2 screen_position;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

// Map Computation
uniform int[MAP_LEN*MAP_LEN] map;
uniform vec2 world_offset;
uniform vec2 world_position;

// Field of View
uniform float circle_radius;
uniform float time;

// Get wall type (only 0 or 1 for now)
int getMap(int x, int y)
{
    if(x < 0 || x >= MAP_LEN || y < 0 || y >= MAP_LEN)
    {
        return 1;
    }
        
    int index = x + y * MAP_LEN;
    return map[index];
}

// From normalized uvs to "world space"
vec2 translateCoordsToMap(vec2 coords)
{
    // float cell_size = 2.0/MAP_LEN;
    // coords.x = floor((coords.x+1.0)/cell_size);
    // coords.y = floor((coords.y+1.0)/cell_size);

    float cell_size = 48.0;
    coords.x = floor((coords.x)/cell_size);
    coords.y = floor((coords.y)/cell_size);
    return coords;
}

void main()
{
	color = vec4(fcolor * vcolor, 1.0);
	color.r = fcolor.r;
	color.g = fcolor.g;
	color.b = color.b + fcolor.b;
    vec2 tpos = screen_position-world_offset;
    vec2 twpos = translateCoordsToMap(tpos);
    vec2 center_pos = world_position-world_offset;

    vec2 rayDir = tpos - center_pos;
    float directionAngle = atan(rayDir.y, rayDir.x);

    float d = 100.0;
    float col_factor = 0.0f;


    if(getMap(int(twpos.x), int(twpos.y)) == 0)
    {
        float max_substep_distance = 1.0f; 
        vec2 total_movement = tpos - center_pos;
        float total_distance = length(total_movement);
        int sub_steps = max(1, int(total_distance / max_substep_distance));
        vec2 movement_per_substep = total_movement / float(sub_steps);
        vec2 p = center_pos;
        for (int step = 0; step < sub_steps; ++step) {
            p = p + movement_per_substep;
            vec2 ip = translateCoordsToMap(p);
            if(getMap(int(ip.x), int(ip.y)) == 1){
                break;
            }
        }

        float d = distance(p, center_pos);
        // Is the pixel in view of the light?
        if(d >= total_distance - 0.1)
            col_factor = 0.0f;
        else
            col_factor = 1.0f;
    }
    else{ // Wall
        col_factor = 0.8f;
    }

    float fade_width = 100.0;
    float fade_start = circle_radius - fade_width*(0.5*sin(0.1*time)+2);

    float dist = distance(tpos, center_pos);
    if (dist > circle_radius) {
        col_factor = 1.0f;
    } else if (dist > fade_start) {
        float fade = smoothstep(fade_start, circle_radius, dist);
        col_factor = col_factor + 1.0f*fade;
        if(col_factor > 1.0f){
            col_factor = 1.0f;
        }
    }

    color = color * col_factor;

}
