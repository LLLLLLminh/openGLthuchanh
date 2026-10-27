#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float sunHeight;
uniform float daylightFactor;
vec3 sunsetTint = vec3(1.0f, 0.7f, 0.45f);
vec3 noonTint = vec3(1.0f, 1.0f, 0.95f);

void main()
{    
    if (sunHeight > 0){
        vec3 skyColor = texture(skybox, TexCoords).rgb;

        skyColor = mix(skyColor * sunsetTint, skyColor, daylightFactor);
        FragColor = vec4(skyColor, 1.0);
    }
    else
        FragColor = texture(skybox, TexCoords);
}
