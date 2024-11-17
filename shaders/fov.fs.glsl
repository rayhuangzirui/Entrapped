#version 330
#define MAP_LEN 16

// From Vertex Shader
in vec3 vcolor;
in vec2 tex_position;
in vec2 screen_position;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

uniform int[MAP_LEN*MAP_LEN] map;

// Get wall type (only 0 or 1 for now)
int getMap(int x, int y)
{
    if(x < 0 || x >= MAP_LEN || y < 0 || y >= MAP_LEN)
    {
        return 1;
    }
        
    int index = x + y * 16;
    return map[index];
}

// From normalized uvs to "world space"
vec2 translateCoordsToWorld(vec2 coords)
{
    // coords.y = 1.0 - coords.y;
    // coords.x -= 0.25;
    // coords.x *= (1296.0) / (720.0);
    // coords *= float(MAP_LEN);
    // coords.x -= 100.0;
    coords.x = floor(coords.x/48.0);
    coords.y = floor(coords.y/48.0);
    return coords;
}

void main()
{
	color = vec4(fcolor * vcolor, 1.0);
	color.r = fcolor.r;
	color.g = fcolor.g;
	color.b = color.b + fcolor.b;
    vec2 tpos = screen_position;
    vec2 twpos = translateCoordsToWorld(tpos);
	vec2 center_pos = vec2(1296/2.0, 720/2.0);

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
            vec2 ip = translateCoordsToWorld(p);
            if(getMap(int(ip.x), int(ip.y)) == 1){
                break;
            }
        }

        float d = distance(p, center_pos);
        // Is the pixel in view of the light?
        if(d >= total_distance - 0.1)
            col_factor = 0.0f;
        else
            col_factor = 0.5f;
    }
    else // Wall
        col_factor = 1.0f;

    color = color * col_factor;
}
