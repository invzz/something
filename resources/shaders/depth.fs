#version 300 es
precision mediump float;
precision mediump int;
in vec2 fragTexCoord;

// Output fragment color
out vec4 finalColor;

uniform sampler2D texture0;

const float near = 0.1;
const float far = 10.0;

void main() {
    float depth = texture(texture0, vec2(fragTexCoord.x, 1.0 - fragTexCoord.y)).r;
    depth = (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
    finalColor = vec4(vec3(depth / far), 1.0);
}