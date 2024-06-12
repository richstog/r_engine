#version 420 core 

in vec2 texCoord;

layout(std140, binding = 0) uniform Camera3D
{
    mat4 projection;
    mat4 view;
    vec3 position;
} Camera3D;

struct LightData
{
    vec3 position;
    vec3 color;
    vec3 attenuation;
    vec4 direction_angle;
    mat4 vp[6];
};

layout(std140, binding = 2) uniform Light
{
    LightData data[64];
    int count;
} light_f;

layout(std140, binding = 3) uniform Sun
{
    vec3 direction;
    vec3 color;
    mat4 vp[4];
} sun;

uniform float Camera3D_cascade_distances[4]; // Размер массива должен соответствовать количеству каскадов

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gBaseColor;
uniform sampler2D gRMS;
uniform sampler2D gEmittedColor;
uniform sampler2DArray sunShadowDepth;
uniform samplerCubeArray pointShadowDepth;
uniform sampler2D ssao;
uniform usampler2D gID;
uniform samplerCube reflections;

uniform uvec3 selectedID;

layout(std140, binding = 4) uniform gamma
{
    float inv_gamma;
};

out vec4 color; 

const float PI = 3.14159265359;

float D(vec3 H, vec3 N, float a)
{
    float tmp = max(dot(N, H), 0);
    tmp = tmp*tmp*(a*a-1)+1;
    return a*a/(PI * tmp*tmp);
}

float G_Sclick_Beckmann(float NDotDir, float a)
{
    float tmp = (a+1)*(a+1) / 8;
    return 1 / (NDotDir * (1 - tmp) + tmp);
}

float G_Smith(float LDotN, float CamDotN, float a)
{
    return G_Sclick_Beckmann(LDotN, a) * G_Sclick_Beckmann(CamDotN, a);
}

vec3 F(vec3 H, vec3 Cam_vertex, float metallic, float specular, vec3 base_color)
{
    vec3 F0 = mix(vec3(0.08 * specular), base_color, metallic);
    return F0 + (1 - F0) * pow(1 - max(dot(H, Cam_vertex),0), 5);
}

float G_Sclick_Beckmann_HS(float NDotDir, float a)
{
    float tmp = (a+1)*(a+1) / 2;
    return 1 / (NDotDir * (1 - tmp) + tmp);
}

float G_Smith_HS(float LDotN, float CamDotN, float a)
{
    return G_Sclick_Beckmann_HS(LDotN, a) * G_Sclick_Beckmann_HS(CamDotN, a);
}

vec3 F_roughness(vec3 H, vec3 Cam_vertex, float metallic, float roughness, float specular, vec3 base_color)
{
    vec3 F0 = mix(vec3(0.08 * specular), base_color, metallic);
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1 - max(dot(H, Cam_vertex),0), 5);
}

void main() 
{ 
    // Получим данные из текстур буфера
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 N = normalize(texture(gNormal, texCoord).rgb);
    vec3 base_color = texture(gBaseColor, texCoord).rgb;
    float roughness = texture(gRMS, texCoord).r;
    float metallic = texture(gRMS, texCoord).g;
    float specular = texture(gRMS, texCoord).b;
    float ssao_value = texture(ssao, texCoord).r;
    
    // Переменные используемые в цикле:
    vec3 L_vertex; // Расположение источника относительно фрагмента
    float L_distance; // Расстояние от поверхности до источника
    vec3 Cam_vertex = normalize(Camera3D.position - fragPos); // Расположение камеры относительно фрагмента
    vec3 ks; // Интенсивность зеркального отражения
    vec3 fd, fs; // Диффузное и зеркальное отражения
    vec3 H; // Вектор половины пути
    float attenuation; // Угасание с учетом расстояния
    float acosA; // Косинус между вектором от поверхности к источнику и обратным направлением источника
    float intensity; // Интенсивность для прожектора
    vec3 fragPosLightSpace; // Фрагмент в пространстве источника
    float shadowValue; // Значение затененности
    vec2 texelSize = 1.0 / textureSize(sunShadowDepth, 0).xy; // Размер текселя текстуры теней
    int x, y, z; // Счетчик для PCF
    float pcfDepth; // Глубина PCF
    float cubemap_offset = 0.05f; // Отступ в текстурных координатах для PCF
    float cubemap_depth; // Дистанция между фрагментом и источником в диапазоне [0;1]

    vec4 fragPosCamSpace = Camera3D.view * vec4(fragPos, 1); // Фрагмент в пространстве камеры
    int cascade_index; // Индекс текущего каскада для вычисления теней
    float CamDotN = dot(Cam_vertex,N); // Скалярное произведение вектора на камеру и нормали
    float LDotN; // Скалярное произведение вектора на источник и нормали

    // Определение индекса каскада в который попадает фрагмент (цикл на 1 меньше чем кол-во каскадов)
    for (cascade_index = 0; cascade_index < 3; cascade_index++)
        if (abs(fragPosCamSpace.z) < Camera3D_cascade_distances[cascade_index])
            break;

    // Фоновая освещенность
    color = vec4(texture(gEmittedColor, texCoord).rgb, 1);
    
    // Если у модели есть нормаль
    if (length(N) > 0)
    {
        // Отражения на основании карт отражений
        vec3 reflectedVec = reflect(-Cam_vertex, N);
        vec3 reflectedColor = textureLod(reflections, reflectedVec, 6*roughness).rgb;

        LDotN = dot(reflectedVec, N);

        // Вектор половины пути
        H = normalize(reflectedVec + Cam_vertex);

        // Зеркальное отражение
        ks = F_roughness(N, Cam_vertex, metallic, roughness, specular, base_color);
        fs = ks * min(D(H, N, roughness*roughness) / 4, 1) * G_Smith_HS(LDotN, CamDotN, roughness*roughness);

        // Результирующий цвет с учетом солнца
        color.rgb += fs * reflectedColor * LDotN;

        
        // Расчет солнца, если его цвет не черный
        if (length(sun.color) > 0)
        {
            // Расположение фрагмента в координатах теневой карты
            fragPosLightSpace = (sun.vp[cascade_index] * vec4(fragPos, 1.0)).xyz;
            // Переход от [-1;1] к [0;1]
            fragPosLightSpace = (fragPosLightSpace + vec3(1.0)) / 2;
            // Сдвиг для решения проблемы акне
            fragPosLightSpace.z -= max(0.05 * (1.0 - dot(N, sun.direction)), 0.005);
            // Проверка PCF
            shadowValue = 0.0;
            texelSize = 1.0 / textureSize(sunShadowDepth, 0).xy; // Размер текселя текстуры теней
            for(x = -1; x <= 1; ++x)
            {
                for(y = -1; y <= 1; ++y)
                {
                    pcfDepth = texture(sunShadowDepth, vec3(fragPosLightSpace.xy + vec2(x, y) * texelSize, cascade_index)).r;
                    shadowValue += fragPosLightSpace.z > pcfDepth  ? 1.0 : 0.0;        
                }    
            }
            shadowValue /= 9.0;
            // Рассчитываем освещенность, если значение тени меньше 1
            if (shadowValue < 1.0)
            {
                // Данные об источнике относительно фрагмента
                L_vertex = normalize(sun.direction);
                LDotN = dot(L_vertex,N);
                if (LDotN > 0)
                {
                    // Вектор половины пути
                    H = normalize(L_vertex + Cam_vertex);

                    // Зеркальное отражение
                    ks = F(H, Cam_vertex, metallic, specular, base_color);
                    fs = ks * min(D(H, N, roughness*roughness) / 4, 1) * G_Smith(LDotN, CamDotN, roughness*roughness);

                    // Диффузное отражение
                    fd = (1 - length(ks)/length(base_color)) * base_color;

                    // Результирующий цвет с учетом солнца
                    color.rgb += (fd + fs) * sun.color * LDotN * (1.0 - shadowValue);
                }
            }
        } 

        // Цикл по источникам света
        int i;
        for (i = 0; i < light_f.count; i++)
        {
            // Обнулим значение тени
            shadowValue = 0;
            // Позиция фрагмента относительно источника
            fragPosLightSpace = fragPos - light_f.data[i].position;
            // Дистанция между фрагментом и источником в диапазоне [0;1]
            cubemap_depth = length(fragPosLightSpace) / light_f.data[i].attenuation.r;
            // Сдвиг для решения проблемы акне
            cubemap_depth -= max(0.05 * (1.0 - dot(N, sun.direction)), 0.005);
            for(x = -1; x <= 1; ++x)
            {
                for(y = -1; y <= 1; ++y)
                {
                    for(z = -1; z <= 1; ++z)
                    {
                        // Значение из кубической текстуры с учетом источника (i)
                        pcfDepth = texture(pointShadowDepth, vec4(fragPosLightSpace + vec3(x, y, z)*cubemap_offset, i)).r; 
                        if(cubemap_depth > pcfDepth)
                            shadowValue += 1.0;
                    }
                }
            }
            shadowValue /= (27);
            if (shadowValue < 1.0)
            {
                // Данные об источнике относительно фрагмента
                L_vertex = light_f.data[i].position - fragPos;
                // Расстояние от поверхности до источника
                L_distance = length(L_vertex);

                // Проверка на дистанцию
                if (L_distance < light_f.data[i].attenuation.r)
                {
                    // Нормирование вектора
                    L_vertex = normalize(L_vertex);
                    // арккосинус между вектором от поверхности к источнику и обратным направлением источника
                    acosA = degrees(acos(dot(-L_vertex, normalize(light_f.data[i].direction_angle.rgb))));
                    // Если угол меньше угла источника или угол источника минимален, то считаем освещенность
                    if(acosA <= light_f.data[i].direction_angle.a) 
                    {
                        LDotN = dot(L_vertex,N);
                        if (LDotN > 0)
                        {
                            // Вектор половины пути
                            H = normalize(L_vertex + Cam_vertex);

                            // Угасание с учетом расстояния
                            attenuation = 1 / (1 + light_f.data[i].attenuation[1] * L_distance + light_f.data[i].attenuation[2] * L_distance * L_distance);

                            // Зеркальное отражение
                            ks = F(H, Cam_vertex, metallic, specular, base_color);
                            fs = ks * min(D(H, N, roughness*roughness) / 4, 1) * G_Smith(LDotN, CamDotN, roughness*roughness);

                            // Диффузное отражение
                            fd = (1 - length(ks)/length(base_color)) * base_color;

                            // Если источник - прожектор, то добавим смягчение
                            if (light_f.data[i].direction_angle.a < 180)
                            {
                                intensity = clamp((light_f.data[i].direction_angle.a - acosA) / 5, 0.0, 1.0);  
                                fd *= intensity;
                                fs *= intensity;
                            }

                            color.rgb += (fd + fs) * light_f.data[i].color * attenuation * LDotN * (1.0 - shadowValue);
                        }
                    }
                }
            }
        }
    }

    // Применение гамма-коррекции
    color.rgb = pow(color.rgb * ssao_value, vec3(inv_gamma));

    vec3 ID = texture(gID, texCoord).rgb;
    // Обводка выбранного объекта
    if (length(selectedID.rg) > 0 && selectedID.rg == ID.rg && ID.b == 0)
    {
        int border_width = 3;
        vec2 size = 1.0f / textureSize(gID, 0);
        for (int i = -border_width; i <= +border_width; i++)
            for (int j = -border_width; j <= +border_width; j++)
            {
                if (i == 0 && j == 0)
                    continue;

                if (texture(gID, texCoord + vec2(i, j) * size).rg != selectedID.rg)
                    color.rgb = vec3(1.0);
            }
    }
} 