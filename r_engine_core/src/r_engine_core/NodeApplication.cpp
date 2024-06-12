// #define GLFW_INCLUDE_NONE

// #include <r_engine_core/NodeApplication.hpp>

// #include <memory>
// #include <iostream>
// #include <random>

// #include "r_engine_core/Log.hpp"
// #include "r_engine_core/Event.hpp"
// #include "r_engine_core/Input.hpp"
// #include <r_engine_core/Rendering/OpenGL/Renderer_OpenGL.hpp>
// #include <r_engine_core/Window.hpp>
// #include <r_engine_core/Modules/UIModule.hpp>

// // #include <r_engine_core/Rendering/NodeOpenGL/Buffers.hpp>
// // #include <r_engine_core/Rendering/NodeOpenGL/Camera.hpp>
// // #include <r_engine_core/Rendering/NodeOpenGL/Lights.hpp>
// // #include <r_engine_core/Rendering/NodeOpenGL/Model.hpp>
// // #include <r_engine_core/Rendering/NodeOpenGL/Scene.hpp>
// // #include <r_engine_core/Rendering/NodeOpenGL/Shader.hpp>
// // #include <r_engine_core/Rendering/NodeOpenGL/Texture.hpp>

// #include "./Rendering/NodeOpenGL/Buffers.hpp"
// #include "./Rendering/NodeOpenGL/Camera.hpp"
// #include "./Rendering/NodeOpenGL/Lights.hpp"
// #include "./Rendering/NodeOpenGL/Model.hpp"
// #include "./Rendering/NodeOpenGL/Scene.hpp"
// #include "./Rendering/NodeOpenGL/Shader.hpp"
// #include "./Rendering/NodeOpenGL/Texture.hpp"

// namespace r_engine
// {
//     // STARTING INIT
//     Texture* pgPosition = NULL;
//     Texture* pgNormal = NULL;
//     Texture* pgDiffuseP = NULL;
//     Texture* pgAmbientSpecular = NULL;
//     RBO* pgrbo = NULL;
//     Texture* pssaoTexture = NULL;
//     Texture* pssaoTexture_raw = NULL;

//     int WINDOW_WIDTH = 800;
// 	int WINDOW_HEIGHT = 600;


//     NodeApplication::NodeApplication()
//     {
//         LOG_INFO("Starting Node Application");
//     }

//     NodeApplication::~NodeApplication()
//     {
//         LOG_INFO("Closing Node Application");
//     }

//     void NodeApplication::close()
//     {
//         m_bCloseWindow = true;
//     }

//     int NodeApplication::start(unsigned int window_width, unsigned int window_height, const char* title)
//     {

//         m_pWindow = std::make_unique<Window>(title, window_width, window_height);

//         m_event_dispatcher.add_event_listener<EventMouseMoved>(
//             [](EventMouseMoved& event)
//             {
//                 //LOG_INFO("[MouseMoved] Mouse moved to x {0}; y {1}", event.x, event.y);
//             }
//         );

//         m_event_dispatcher.add_event_listener<EventWindowResize>(
//             [&](EventWindowResize& event)
//             {
//                 LOG_INFO("[WindowResize] Window resized to {0}x{1}", event.width, event.height);
//             }
//         );

//         m_event_dispatcher.add_event_listener<EventWindowClose>(
//             [&](EventWindowClose& event)
//             {
//                 LOG_INFO("[WindowClose]");
//                 close();
//             }
//         );

//         m_event_dispatcher.add_event_listener<EventKeyPressed>(
//             [&](EventKeyPressed& event)
//             {
//                 LOG_INFO("[EventKeyPressed] {0}", static_cast<char>(event.key_code));

//                 if (event.key_code <= KeyCode::KEY_Z)
//                 {
//                     if (event.repeated)
//                     {
//                         LOG_INFO("\t[EventKeyPressed] [Repeated] {0}", static_cast<char>(event.key_code));
//                     } else
//                     {
//                         LOG_INFO("\t[EventKeyPressed] {0}", static_cast<char>(event.key_code));
//                     }
//                 }
                
//                 Input::PressKey(event.key_code);
//             }
//         );

//         m_event_dispatcher.add_event_listener<EventKeyReleased>(
//             [&](EventKeyReleased& event)
//             {
//                 LOG_INFO("[EventKeyReleased] {0}", static_cast<char>(event.key_code));
//                 Input::ReleasedKey(event.key_code);
//             }
//         );

//         m_event_dispatcher.add_event_listener<EventMouseButtonPressed>(
//             [&](EventMouseButtonPressed& event)
//             {
//                 LOG_INFO("[EventMouseButtonPressed] {0}", static_cast<int>(event.mouse_button_code));
//                 LOG_INFO("\t[EventMouseButtonPressed] At ({0}, {1})", event.x_pos, event.y_pos);
                
//                 Input::PressMouseButton(event.mouse_button_code);
//                 on_mouse_button_pressed_event(event.mouse_button_code, event.x_pos, event.y_pos, true);
//             }
//         );

//         m_event_dispatcher.add_event_listener<EventMouseButtonRealeased>(
//             [&](EventMouseButtonRealeased& event)
//             {
//                 LOG_INFO("[EventMouseButtonRealeased] {0}", static_cast<int>(event.mouse_button_code));
//                 LOG_INFO("\t[EventMouseButtonRealeased] At ({0}, {1})", event.x_pos, event.y_pos);
                
//                 Input::ReleaseMouseButton(event.mouse_button_code);
//                 on_mouse_button_pressed_event(event.mouse_button_code, event.x_pos, event.y_pos, false);
//             }
//         );

//         m_pWindow->set_event_callback(
//             [&](BaseEvent& event)
//             {
//                 m_event_dispatcher.dispatch(event);
//             }
//         );

//         Renderer_OpenGL::enable_depth_testing();
	
// 	    // Шейдер для G-буфера
// 	    ShaderProgram gShader;
// 	    // Загрузка и компиляция шейдеров
// 	    gShader.load(GL_VERTEX_SHADER, "shaders/gshader.vert");
// 	    gShader.load(GL_FRAGMENT_SHADER, "shaders/gshader.frag");
// 	    gShader.link();
// 	    // Установим значения текстур
// 	    const char* textures_base_shader_names[] = {"tex_diffuse", "tex_ambient", "tex_specular"};
// 	    gShader.bindTextures(textures_base_shader_names, sizeof(textures_base_shader_names)/sizeof(const char*));
	
// 	    // Загрузка сцены из obj файла
// 	    Scene scene = loadOBJtoScene("../resources/models/blob.obj", "../resources/models/", "../resources/textures/"); 
// 	    scene.root.e_scale() = glm::vec3(0.01);
// 	    scene.root.e_position().z = 1;
// 	    scene.models.begin()->material.kd = {0.5,0.5,0.5};
// 	    scene.models.begin()->material.ka = {0.05,0.05,0.05};

//         // Установка цвета очистки буфера цвета
// 	    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
// 	    // Сдвинем направленный источник света и камеру
// 	    Sun::get().e_direction().z = -1.0;
// 	    Camera::current().e_position().x = 0.3f;
	
// 	    // Источники света
// 	    Light& first = Light::getNew();
// 	    first.e_color() = {1.0f, 0.0f, 0.0f}; // цвет
// 	    first.e_position() = {0.3f, 0.0f, 0.6f}; // Позиция
// 	    first.e_angle() = 100.0f;
// 	    Light& second = Light::getNew();
// 	    second.e_color() = {0.0f, 0.0f, 1.0f}; // цвет
// 	    second.e_position() = {-0.3f, 0.3f, 0.5f}; // Позиция
	
// 	    // Uniform-буферы
// 	    UBO cameraUB(sizeof(CameraData), 0);
// 	    UBO material_data(sizeof(Material), 1);
// 	    UBO light_data(Light::getUBOsize(), 2);
// 	    UBO sun_data(sizeof(Sun), 3);
	
// 	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Использование уменьшенных версий mipmap  
	
//     // Создадим G-буфер с данными о используемых привязках
// 	    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
// 	    FBO gbuffer(attachments, sizeof(attachments) / sizeof(GLuint));
// 	    // Создадим текстуры для буфера кадра
// 	    Texture gPosition(WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_ATTACHMENT0, 0, GL_RGB32F, GL_RGB); // Позиция вершины
// 	    Texture gNormal(WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_ATTACHMENT1, 1, GL_RGB16F, GL_RGB); // Нормали
// 	    Texture gDiffuseP(WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_ATTACHMENT2, 2, GL_RGBA16F); // Диффузная составляющая и коэф. глянцевости
// 	    Texture gAmbientSpecular(WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_ATTACHMENT3, 3); // Фоновая составляющая и один канал зеркальной
//     // Создадим буфер рендера под буфер глубины и привяжем его
// 	    RBO grbo(WINDOW_WIDTH, WINDOW_HEIGHT);
// 	    gbuffer.assignRenderBuffer(grbo.getHandler());
// 	    // Активируем базовый буфер кадра
// 	    FBO::useDefault();
	
// 	    // Сохраним указатели на текстуры для изменения размеров окна
// 	    pgPosition = &gPosition;
// 	    pgNormal = &gNormal;
// 	    pgDiffuseP = &gDiffuseP;
// 	    pgAmbientSpecular = &gAmbientSpecular;
// 	    pgrbo = &grbo;
	
// 	    // Шейдер для расчета освещенности
// 	    ShaderProgram lightShader;
// 	    // Загрузка и компиляция шейдеров
// 	    lightShader.load(GL_VERTEX_SHADER, "shaders/quad.vert");
// 	    lightShader.load(GL_FRAGMENT_SHADER, "shaders/lighting.frag");
// 	    lightShader.link();
// 	    // Привязка текстур
// 	    const char* gtextures_shader_names[]  = {"gPosition", "gNormal", "gDiffuseP", "gAmbientSpecular", "sunShadowDepth", "pointShadowDepth", "ssao"};
// 	    lightShader.bindTextures(gtextures_shader_names, sizeof(gtextures_shader_names)/sizeof(const char*));
//     // Загрузка данных о границах каскадов
// 	    glUniform1fv(lightShader.getUniformLoc("camera_cascade_distances"), CAMERA_CASCADE_COUNT, &camera_cascade_distances[1]);
	
// 	    glm::vec3 quadVertices[] = { {-1.0f,  1.0f, 0.0f}
// 	                               , {-1.0f, -1.0f, 0.0f}
// 	                               , { 1.0f,  1.0f, 0.0f}
// 	                               , { 1.0f, -1.0f, 0.0f}
// 	                               };
	
// 	    GLuint quadIndices[] = {0,1,2,2,1,3};
	
// 	    Model quadModel;
// 	    quadModel.load_verteces(quadVertices, 4);
// 	    quadModel.load_indices(quadIndices, 6);
	
// 	    // Размер текстуры тени от солнца
// 	    const GLuint sunShadow_resolution = 1024;
// 	    // Создадим буфер кадра для рендера теней
// 	    FBO sunShadowBuffer;
// 	    // Создадим текстуры для буфера кадра
// 	    TextureArray sunShadowDepth(CAMERA_CASCADE_COUNT, sunShadow_resolution, sunShadow_resolution, GL_DEPTH_ATTACHMENT, 4, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
// 	    // Правка фантомных теней
// 	    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
// 	    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
// 	    float shadowBorderColor[] = { 1.0, 1.0, 1.0, 1.0 };
// 	    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, shadowBorderColor);
//     // Отключим работу с цветом
// 	    glDrawBuffer(GL_NONE);
// 	    glReadBuffer(GL_NONE);
// 	    // Активируем базовый буфер кадра
// 	    FBO::useDefault();
	
// 	    // Шейдер для расчета теней
// 	    ShaderProgram sunShadowShader;
// 	    // Загрузим шейдер
// 	    sunShadowShader.load(GL_VERTEX_SHADER, "shaders/sun_shadow.vert");
// 	    sunShadowShader.load(GL_GEOMETRY_SHADER, "shaders/sun_shadow.geom");
// 	    sunShadowShader.load(GL_FRAGMENT_SHADER, "shaders/empty.frag");
// 	    sunShadowShader.link();
	    
// 	    // Размер одной стороны кубической карты
// 	    const GLuint pointShadow_resolution = 500;
// 	    // Создадим буфер кадра для рендера теней от источников света
// 	    FBO pointShadowBuffer;
// 	    // Создадим текстуры для буфера кадра
// 	    TextureCubeArray pointShadowDepth(MAX_LIGHTS, pointShadow_resolution, pointShadow_resolution, GL_DEPTH_ATTACHMENT, 5, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
//     // Отключим работу с цветом
// 	    glDrawBuffer(GL_NONE);
// 	    glReadBuffer(GL_NONE);
// 	    // Активируем базовый буфер кадра
// 	    FBO::useDefault();
	
// 	    // Шейдер для расчета теней от точечных источников
// 	    ShaderProgram pointShadowShader;
// 	    // Загрузим шейдер
// 	    pointShadowShader.load(GL_VERTEX_SHADER, "shaders/sun_shadow.vert");
// 	    pointShadowShader.load(GL_GEOMETRY_SHADER, "shaders/point_shadow.geom");
// 	    pointShadowShader.load(GL_FRAGMENT_SHADER, "shaders/point_shadow.frag");
// 	    pointShadowShader.link();
	
// 	    // Создадим буфер для вычисления SSAO
// 	    GLuint attachments_ssao[] = { GL_COLOR_ATTACHMENT0 };
// 	    FBO ssaoBuffer(attachments_ssao, sizeof(attachments_ssao) / sizeof(GLuint));
// 	    // Создадим текстуры для буфера кадра
// 	    Texture ssaoTexture_raw(WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_ATTACHMENT0, 0, GL_RED, GL_RED);
// 	    pssaoTexture_raw = &ssaoTexture_raw;
// 	    // Активируем базовый буфер кадра
// 	    FBO::useDefault();
	
// 	    // Стандартные параметры SSAO
// 	    SSAO_data ssao_data;
// 	    // Расчет масштабирования текстуры шума
// 	    ssao_data.scale = {WINDOW_WIDTH/4,WINDOW_HEIGHT/4};
// 	    // Генерируем случайные векторы
// 	    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // Генерирует случайные вещественные числа в заданном диапазоне
// 	    std::default_random_engine generator;
// 	    glm::vec3 sample; // Выборка
// 	    for (int i = 0; i < ssao_data.size; i++)
// 	    {
// 	        sample = {  randomFloats(generator) * 2.0 - 1.0
// 	                  , randomFloats(generator) * 2.0 - 1.0
// 	                  , randomFloats(generator)
// 	                 };
// 	        sample = glm::normalize(sample);
// 	        sample *= randomFloats(generator);
	        
// 	        // Отмасштабируем выборку
// 	        sample *= 0.1 + 0.9 * (i / (float)ssao_data.size) * (i / (float)ssao_data.size);
// 	        ssao_data.samples[i] = sample;
// 	    }
// 	    // Загрузка данных в uniform-буфер
// 	    UBO ssaoUB(&ssao_data, sizeof(SSAO_data), 4);
	
// 	    // Текстура шума
// 	    glm::vec3 noise_vecs[16];
// 	    for (int i = 0; i < 16; i++)
// 	        noise_vecs[i] = { randomFloats(generator) * 2.0 - 1.0
// 	                        , randomFloats(generator) * 2.0 - 1.0
// 	                        , 0.0f
// 	                        };
// 	    Texture noiseTexture(4,4, noise_vecs, 2, GL_RGBA32F, GL_RGB);
	
// 	    // Шейдер для расчета SSAO
// 	    ShaderProgram ssaoShader;
// 	    // Загрузим шейдер
// 	    ssaoShader.load(GL_VERTEX_SHADER, "shaders/quad.vert");
// 	    ssaoShader.load(GL_FRAGMENT_SHADER, "shaders/ssao.frag");
// 	    ssaoShader.link();
// 	    // Текстуры, используемые в шейдере
// 	    const char* ssaoShader_names[]  = {"gPosition", "gNormal", "noise"};
// 	    ssaoShader.bindTextures(ssaoShader_names, sizeof(ssaoShader_names)/sizeof(const char*));
	
// 	    // Создадим буфер для размытия SSAO
// 	    FBO ssaoBlurBuffer(attachments_ssao, sizeof(attachments_ssao) / sizeof(GLuint));  
// 	    // Создадим текстуры для буфера кадра
// 	    Texture ssaoTexture(WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_ATTACHMENT0, 6, GL_RED, GL_RED); 
// 	    pssaoTexture = &ssaoTexture;
// 	    // Активируем базовый буфер кадра
// 	    FBO::useDefault();
	
// 	    // Шейдер для размытия SSAO
// 	    ShaderProgram ssaoBlurShader;
// 	    // Загрузим шейдер
// 	    ssaoBlurShader.load(GL_VERTEX_SHADER, "shaders/quad.vert");
// 	    ssaoBlurShader.load(GL_FRAGMENT_SHADER, "shaders/ssaoBlur.frag");
// 	    ssaoBlurShader.link();
	
// 	    // Модель прямоугольника
// 	    Model rectangle; 
	
// 	    // Вершины прямоугольника
// 	    glm::vec3 rectangle_verticies[] = {  {-0.5f, 0.0f, -0.5f}
// 	                                       , { 0.5f, 0.0f, -0.5f}
// 	                                       , { 0.5f, 0.0f,  0.5f}
// 	                                       , {-0.5f, 0.0f,  0.5f}
// 	                                    };
// 	    // Загрузка вершин модели
// 	    rectangle.load_verteces(rectangle_verticies, sizeof(rectangle_verticies)/sizeof(glm::vec3));
	
// 	    // индексы вершин
// 	    GLuint rectangle_indices[] = {0, 1, 2, 2, 3, 0}; 
// 	    // Загрузка индексов модели
// 	    rectangle.load_indices(rectangle_indices, sizeof(rectangle_indices)/sizeof(GLuint));
	
// 	    // Нормали
// 	    glm::vec3 rectangle_normals[] = {  {0.0f, 1.0f, 0.0f}
// 	                                     , {0.0f, 1.0f, 0.0f}
// 	                                     , {0.0f, 1.0f, 0.0f}
// 	                                     , {0.0f, 1.0f, 0.0f}
// 	                                    };
// 	    // Загрузка нормалей модели
// 	    rectangle.load_normals(rectangle_normals, sizeof(rectangle_normals)/sizeof(glm::vec3));
	
// 	    // Зададим горизонтальное положение перед камерой
// 	    rectangle.e_position().y = -1;
// 	    rectangle.e_position().z = 2;
// 	    rectangle.e_scale() = glm::vec3(4);
	
// 	    // Параметры материала
// 	    rectangle.material.ka = {0.05, 0.05, 0.05};
// 	    rectangle.material.kd = {1, 1, 1};
	
// 	    // Шейдер для рисования отладочных лампочек
// 	    ShaderProgram bulbShader;
// 	    // Загрузка и компиляция шейдеров
// 	    bulbShader.load(GL_VERTEX_SHADER, "shaders/bulb.vert");
// 	    bulbShader.load(GL_FRAGMENT_SHADER, "shaders/bulb.frag");
// 	    bulbShader.link();
	    
// 	    // Вершины для скайбокса
// 	    glm::vec3 skybox_verticies[] = {        
// 	        {-1.0f,  1.0f, -1.0f},
// 	        {-1.0f, -1.0f, -1.0f},
// 	        { 1.0f, -1.0f, -1.0f},
// 	        { 1.0f, -1.0f, -1.0f},
// 	        { 1.0f,  1.0f, -1.0f},
// 	        {-1.0f,  1.0f, -1.0f},
	
// 	        {-1.0f, -1.0f,  1.0f},
// 	        {-1.0f, -1.0f, -1.0f},
// 	        {-1.0f,  1.0f, -1.0f},
// 	        {-1.0f,  1.0f, -1.0f},
// 	        {-1.0f,  1.0f,  1.0f},
// 	        {-1.0f, -1.0f,  1.0f},
	
// 	        { 1.0f, -1.0f, -1.0f},
// 	        { 1.0f, -1.0f,  1.0f},
// 	        { 1.0f,  1.0f,  1.0f},
// 	        { 1.0f,  1.0f,  1.0f},
// 	        { 1.0f,  1.0f, -1.0f},
// 	        { 1.0f, -1.0f, -1.0f},
	
// 	        {-1.0f, -1.0f,  1.0f},
// 	        {-1.0f,  1.0f,  1.0f},
// 	        { 1.0f,  1.0f,  1.0f},
// 	        { 1.0f,  1.0f,  1.0f},
// 	        { 1.0f, -1.0f,  1.0f},
// 	        {-1.0f, -1.0f,  1.0f},
	
// 	        {-1.0f,  1.0f, -1.0f},
// 	        { 1.0f,  1.0f, -1.0f},
// 	        { 1.0f,  1.0f,  1.0f},
// 	        { 1.0f,  1.0f,  1.0f},
// 	        {-1.0f,  1.0f,  1.0f},
// 	        {-1.0f,  1.0f, -1.0f},
	
// 	        {-1.0f, -1.0f, -1.0f},
// 	        {-1.0f, -1.0f,  1.0f},
// 	        { 1.0f, -1.0f, -1.0f},
// 	        { 1.0f, -1.0f, -1.0f},
// 	        {-1.0f, -1.0f,  1.0f},
// 	        { 1.0f, -1.0f,  1.0f}
// 	    };
// 	    // Модель скайбокса
// 	    Model skybox;
// 	    skybox.load_verteces(skybox_verticies, sizeof(skybox_verticies)/sizeof(glm::vec3));
// 	    TextureCube skybox_texture(TEX_DIFFUSE, {  "../resources/textures/skybox/px.jpg"
// 	                                             , "../resources/textures/skybox/nx.jpg"
// 	                                             , "../resources/textures/skybox/py.jpg"
// 	                                             , "../resources/textures/skybox/ny.jpg"
// 	                                             , "../resources/textures/skybox/pz.jpg"
// 	                                             , "../resources/textures/skybox/nz.jpg"
// 	                                            });
	
// 	    // Шейдер для скайбокса
// 	    ShaderProgram skyboxShader;
// 	    // Загрузим шейдеры
// 	    skyboxShader.load(GL_VERTEX_SHADER, "shaders/skybox.vert");
// 	    skyboxShader.load(GL_FRAGMENT_SHADER, "shaders/skybox.frag");
// 	    skyboxShader.link();
// 	    // Привязка текстуры скайбокса
// 	    const char* skybox_shader_names[]  = {"skybox"};
// 	    skyboxShader.bindTextures(skybox_shader_names, sizeof(skybox_shader_names)/sizeof(const char*));
	    
// 	    // Значение гамма-коррекции
// 	    UBO gamma(&inv_gamma, sizeof(inv_gamma), 4);



//         /* MAIN LOOP */
//         while (!m_bCloseWindow)
//         {
//             //Renderer_OpenGL::set_clear_color(255, 255, 255, 0);
//         //Renderer_OpenGL::clear();

//         // Загрузка данных о камере
// 	        cameraUB.loadSub(&Camera::current().getData(), sizeof(CameraData));
//         // Загрузим информацию об источниках света
// 	        Light::upload(light_data);
//         // Загружаем информацию о направленном источнике
// 	        Sun::upload(sun_data);
	
// 	        // Активируем G-кадра
// 	        gbuffer.use();
//         // Используем шейдер с освещением
// 	        gShader.use();
// 	        // Очистка буфера цвета и глубины
// 	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
// 	        // Тут производится рендер
// 	        scene.render(gShader, material_data);
// 	        rectangle.render(gShader, material_data);
	
// 	        // Активируем буфер SSAO
// 	        ssaoBuffer.use();
// 	        // Используем шейдер для расчета SSAO
// 	        ssaoShader.use();
// 	        // Очистка буфера цвета
// 	        glClear(GL_COLOR_BUFFER_BIT);
// 	        // Подключаем текстуры G-буфера
// 	        gPosition.use();
// 	        gNormal.use();
// 	        // Подключаем текстуру шума для SSAO
// 	        noiseTexture.use();
// 	        // Рендерим прямоугольник
// 	        quadModel.render();
	
// 	        // Активируем буфер размытия SSAO
// 	        ssaoBlurBuffer.use();
// 	        // Используем шейдер для размытия SSAO
// 	        ssaoBlurShader.use();
// 	        // Очистка буфера цвета
// 	        glClear(GL_COLOR_BUFFER_BIT);
// 	        // Подключаем текстуру сырого SSAO
// 	        ssaoTexture_raw.use();
// 	        // Рендерим прямоугольник
// 	        quadModel.render();
	
// 	        // Изменим размер вывода для тени
// 	        glViewport(0, 0, sunShadow_resolution, sunShadow_resolution);
// 	        // Активируем буфер кадра для теней от солнца
// 	        sunShadowBuffer.use();
// 	        // Подключим шейдер для расчета теней
// 	        sunShadowShader.use();
// 	        // Очистка буфера глубины
// 	        glClear(GL_DEPTH_BUFFER_BIT);
// 	        // Рендерим геометрию в буфер глубины
// 	        scene.render(sunShadowShader, material_data);
// 	        rectangle.render(sunShadowShader, material_data);
	
// 	        // Изменим размер вывода для стороны кубической карты точечного источника
// 	        glViewport(0, 0, pointShadow_resolution, pointShadow_resolution);
// 	        // Активируем буфер кадра для теней от солнца
// 	        pointShadowBuffer.use();
// 	        // Подключим шейдер для расчета теней
// 	        pointShadowShader.use();
// 	        // Очистка буфера глубины
// 	        glClear(GL_DEPTH_BUFFER_BIT);
// 	        // Для каждого источника вызывается рендер сцены
// 	        for (int i = 0; i < Light::getCount(); i++)
// 	        {
// 	            glUniform1i(pointShadowShader.getUniformLoc("light_i"), i);
// 	            // Рендерим геометрию в буфер глубины
// 	            scene.render(pointShadowShader, material_data);  
// 	            rectangle.render(pointShadowShader, material_data);
// 	        }
	
// 	        // Изменим размер вывода для окна
// 	        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
// 	        // Активируем базовый буфер кадра
// 	        FBO::useDefault();
// 	        // Подключаем шейдер для прямоугольника
// 	        lightShader.use();
// 	        // Очистка буфера цвета и глубины
// 	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	        // Подключаем текстуры G-буфера
// 	        gPosition.use();
// 	        gNormal.use();
// 	        gDiffuseP.use();
// 	        gAmbientSpecular.use();
// 	        // Подключаем текстуры теней
// 	        sunShadowDepth.use();
// 	        pointShadowDepth.use();
// 	        // Подключим текстуру SSAO
// 	        ssaoTexture.use();
//         // Рендерим прямоугольник с расчетом освещения
// 	        quadModel.render();
	
// 	        // Перенос буфера глубины
// 	        FBO::useDefault(GL_DRAW_FRAMEBUFFER); // Базовый в режиме записи
// 	        gbuffer.use(GL_READ_FRAMEBUFFER); // Буфер геометрии в режиме чтения
// 	        // Копирование значений глубины 
// 	        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
// 	        FBO::useDefault(); // Использование базового буфера для дальнейших работ
	
// 	        // Отрисовка скайбокса без записи глубины
// 	        glDepthMask(GL_FALSE);
// 	        // Используем шейдер для скайбокса
// 	        skyboxShader.use();
// 	        // Подключаем текстуру скайбокса
// 	        skybox_texture.use();
// 	        // Рендерим куб
// 	        skybox.render();
// 	        // Возвращаем запись глубины
// 	        glDepthMask(GL_TRUE);
	        
//         // Отрисовка отладочных лампочек со специальным шейдером
// 	        bulbShader.use();
// 	        Light::render(bulbShader, material_data);
//             draw();
//         }
//         m_pWindow = nullptr;

//         return 0;
//     }



//     void NodeApplication::draw()
//     {
        


//         UIModule::on_ui_draw_begin();

//         on_ui_draw();
//         UIModule::on_ui_draw_end();

//         m_pWindow->on_update();
//         on_update();
//     }
// }