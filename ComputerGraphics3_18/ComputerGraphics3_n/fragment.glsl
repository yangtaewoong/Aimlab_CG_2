#version 330 core

in vec3 FragPos; 
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;      // 광원 위치
uniform vec3 lightColor;    // 광원 색상
uniform vec3 objectColor;   // 물체 색상

void main()
{
    // --- Ambient lighting (환경광)
    vec3 ambientLight = vec3(0.2);
    vec3 ambient = ambientLight * lightColor;

    // --- Diffuse lighting (확산광)
    vec3 normalVector = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // 광원 방향 계산
    float diffuseLight = max(dot(normalVector, lightDir), 0.0); // Lambert's Cosine Law
    vec3 diffuse = diffuseLight * lightColor;

    // --- Combine results (최종 조명 효과)
    vec3 result = (ambient + diffuse) * objectColor;

    // --- Gamma correction (감마 보정)
    vec3 gammaCorrected = pow(result, vec3(1.0 / 2.2)); // 감마 보정 적용

    // Final output (최종 색상 출력)
    FragColor = vec4(gammaCorrected, 1.0);
}