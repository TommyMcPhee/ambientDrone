#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform float droneAmplitude;

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    outputColor = vec4(droneAmplitude, normalized.x, normalized.y, 1.0);
}