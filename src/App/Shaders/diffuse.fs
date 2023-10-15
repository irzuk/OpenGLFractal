#version 330 core

uniform vec3 resolution;
uniform vec4 settings;
uniform vec3 color;

out vec4 fragColor;
 
int get_iterations()
{   
    float zoom = resolution.z;
    float center_x = resolution.x;
    float center_y = resolution.y;

    float real = ((gl_FragCoord.x / 1080.0 - 0.5) * zoom + center_x) * 5.0;
    float imag = ((gl_FragCoord.y / 1080.0 - 0.5) * zoom + center_y) * 5.0;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < settings.w)
    {
        float tmp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0 * tmp_real * imag) + const_imag;
         
        float dist = real * real + imag * imag;
         
        if (dist > 4.0)
        break;
 
        ++iterations;
    }
    return iterations;
}


vec4 return_color()
{
    int iter = get_iterations();
    if (iter == settings.w)
    {
        gl_FragDepth = 0.0f;
        return vec4(settings.xyz, 1.0f);
    }

    float iterations = float(iter) / settings.w;
    vec3 result_color = mix(settings.xyz, color, iterations); 
    return vec4(result_color, 1.0f);
}

void main() {
	fragColor = return_color();
}