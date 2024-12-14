#version 330 core

in vec3 FragPos; 
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;      // ���� ��ġ
uniform vec3 lightColor;    // ���� ����
uniform vec3 objectColor;   // ��ü ����

void main()
{
    // --- Ambient lighting (ȯ�汤)
    vec3 ambientLight = vec3(0.2);
    vec3 ambient = ambientLight * lightColor;

    // --- Diffuse lighting (Ȯ�걤)
    vec3 normalVector = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // ���� ���� ���
    float diffuseLight = max(dot(normalVector, lightDir), 0.0); // Lambert's Cosine Law
    vec3 diffuse = diffuseLight * lightColor;

    // --- Combine results (���� ���� ȿ��)
    vec3 result = (ambient + diffuse) * objectColor;

    // --- Gamma correction (���� ����)
    vec3 gammaCorrected = pow(result, vec3(1.0 / 2.2)); // ���� ���� ����

    // Final output (���� ���� ���)
    FragColor = vec4(gammaCorrected, 1.0);
}