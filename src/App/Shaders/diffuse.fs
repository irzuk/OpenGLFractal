#version 330 core

uniform vec3 resolution;
uniform vec3 settings;

out vec4 fragColor;
 
int get_iterations()
{   
    float zoom = resolution.z;
    float center_x = 0 + resolution.x;
    float center_y = 0 + resolution.y;

    float real = ((gl_FragCoord.x / 1080.0 - 0.5) * zoom + center_x) * 5.0 + resolution.x;
    float imag = ((gl_FragCoord.y / 1080.0 - 0.5) * zoom + center_y) * 5.0 + resolution.y;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < settings.x)
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
    if (iter == settings.x)
    {
        gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    float iterations = float(iter) / settings.x;    
    return vec4(resolution.z, iterations, 0.0f, 1.0f);
}

void main() {
	fragColor = return_color();
}