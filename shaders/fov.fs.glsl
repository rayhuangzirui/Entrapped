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
void translateCoordsToWorld(inout vec2 coords)
{
    coords.y = 1.0 - coords.y;
    coords.x -= 0.25;
    coords.x *= (1296.0) / (720.0);
    coords *= float(MAP_LEN);
}

void main()
{
	color = vec4(fcolor * vcolor, 1.0);
	color.r = fcolor.r;
	color.g = fcolor.g;
	color.b = color.b + fcolor.b;
    vec2 tpos = screen_position / vec2(1296.0, 720.0);
    translateCoordsToWorld(tpos);
	vec2 center_pos = vec2(8.5, 8.5);

    vec2 rayDir = tpos - center_pos;
    float directionAngle = atan(rayDir.y, rayDir.x);

    float d = 100.0;
    float col_factor = 0.0f;

    if(getMap(int(tpos.x), int(tpos.y)) == 0)
    {
        // Vertical
        if(rayDir.y != 0.0)
        {
            // Step lengths
            float stepX = sign(rayDir.y) / tan(directionAngle);
            float stepY = sign(rayDir.y);

            // Initial intersection
            vec2 p = vec2(0.0, round(center_pos.y + sign(rayDir.y) * 0.5));
            p.x = center_pos.x + -(center_pos.y - p.y) / tan(directionAngle);

            // March
            for(int i = 0; i < 16; i++)
            {
                // Test the tiles sharing an edge
                int testX = int(p.x);
                int testY1 = int(p.y + 0.5);
                int testY2 = int(p.y - 0.5);
                if(map[testX + testY1 * 16] + map[testX + testY2 * 16] <= 0)
                {
                    p += vec2(stepX, stepY);
                }
                else
                    break;
            }

            d = min(length(center_pos - p), d);
        }

        // Horizontal
        if(rayDir.x != 0.0)
        {
            // Step lengths
            float stepX = sign(rayDir.x);
            float stepY = sign(rayDir.x) * tan(directionAngle);

            // Initial intersection
            vec2 p = vec2(round(center_pos.x + sign(rayDir.x) * 0.5), 0.0);
            p.y = center_pos.y + (p.x - center_pos.x) * tan(directionAngle);

            // March
            for(int i = 0; i < 16; i++)
            {
                // Test the tiles sharing an edge
                int testX1 = int(p.x + 0.5);
                int testX2 = int(p.x - 0.5);
                int testY = int(p.y);
                if(map[testX1 + testY * 16] + map[testX2 + testY * 16] <= 0)
                {
                    p += vec2(stepX, stepY);
                }
                else
                    break;
            }

            d = min(length(center_pos - p), d);
        }
        
        float lightToUvLen = length(tpos - center_pos);
        
        // Is the pixel in view of the light?
        if(d >= lightToUvLen - EPSILON)
            col_factor = 0.0f;
        else
            col_factor = 0.5f;
        }
        else // Wall
            col_factor = 1.0f;

    color = color * col_factor;
}
