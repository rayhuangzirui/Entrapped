#version 330
#define EPSILON 0.001
#define MAP_LEN 16

// From Vertex Shader
in vec3 vcolor;
in vec2 tex_position;
in vec2 screen_position;

// Application data
uniform vec3 fcolor;

// Output color
layout(location = 0) out vec4 color;

int[MAP_LEN*MAP_LEN] map = int[](
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,0,1,1,0,1,1,1,1,0,1,1,1,1,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
    1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,1,
    1,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,
    1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,
    1,1,0,0,0,0,1,0,0,0,0,0,0,0,1,1,
    1,1,1,0,0,0,0,0,0,1,1,0,0,0,1,1,
    1,1,1,1,0,0,0,0,0,1,0,0,0,0,1,1,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,0,0,0,0,0,1,1,1,0,0,0,0,1,1,
    1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,
    1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,
    1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
);

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
    vec2 center_pos = vec2(1296/2.0, 720/2.0);
    vec2 lm_pos = center_pos - 8*48.f;
    lm_pos.x = floor(lm_pos.x/48.0);
    lm_pos.y = floor(lm_pos.y/48.0);

    coords.x = floor(coords.x/48.0) - lm_pos.x;
    coords.y = floor(coords.y/48.0) - lm_pos.y;
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
                vec2 wall_pos = vec2((int(ip.x) + 0.5f) * 48.f, (int(ip.y) + 0.5f) * 48.f);
                vec2 wall_size = vec2(48.f, 48.f);
                float wall_left = wall_pos.x - wall_size.x / 2.0f;
                float wall_right = wall_pos.x + wall_size.x / 2.0f;
                float wall_top = wall_pos.y - wall_size.y / 2.0f;
                float wall_bottom = wall_pos.y + wall_size.y / 2.0f;
                float overlap_x = 0.0f;
                float overlap_y = 0.0f;
                // Calculate the minimum translation distance along x
                if (p.x < wall_pos.x) {
                    overlap_x = p.x - wall_left;
                }
                else {
                    overlap_x = wall_right - p.x;
                }

                // Calculate the minimum translation distance along y
                if (p.y < wall_pos.y) {
                    overlap_y = p.y - wall_top;
                }
                else {
                    overlap_y = wall_bottom - p.y;
                }

                // Determine the axis of minimum penetration
                if (overlap_x < overlap_y) {
                    // Move point out along x-axis
                    float mtv_x = (p.x < wall_pos.x) ? -overlap_x : overlap_x;
                    p.x += mtv_x;
                }
                else {
                    // Move entity out along y-axis
                    float mtv_y = (p.y < wall_pos.y) ? -overlap_y : overlap_y;
                    p.y += mtv_y;
                }
            }
        }

        float d = distance(p, center_pos);
        // Is the pixel in view of the light?
        if(d >= total_distance - EPSILON)
            col_factor = 0.0f;
        else
            col_factor = 1.0f;
    }
    else // Wall
        col_factor = 1.0f;

    color = color * col_factor;
}
