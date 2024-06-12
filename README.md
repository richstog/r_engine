# 3D движок с физикой
## Описание проекта
Проект представляет собой фреймворк r_engine_core и редактор r_engine_editor для работы с памятью видеокарты и трехмерным пространством средствами спецификации OpenGL. Проект реализован на языке C++, с инструментом сборки CMake и компилятором gcc.

## Файловая структура

Точка входа (функция main) описана в файле [main.cpp](https://github.com/richstog/OGL_engine/blob/master/r_engine_editor/src/main.cpp).

Проект содержит следующую файловую структуру:
+ Папка [external](https://github.com/richstog/OGL_engine/tree/master/external) содержит библиотеки проекта
+ Папка [r_engine_core](https://github.com/richstog/OGL_engine/tree/master/r_engine_core) содержит папки и файлы фреймворка:
  + Папка [includes/r_engine_core](https://github.com/richstog/OGL_engine/tree/master/r_engine_core/includes/r_engine_core) содержит заголовочные файлы фреймворка;
  + Папка [src/r_engine_core](https://github.com/richstog/OGL_engine/tree/master/r_engine_core/src/r_engine_core) содержит исходные файлы фреймворка:
    + Папка [Modules](https://github.com/richstog/OGL_engine/tree/master/r_engine_core/src/r_engine_core/Modules) содержит модули, используемые фреймворком;
    + Папка [Rendering](https://github.com/richstog/OGL_engine/tree/master/r_engine_core/src/r_engine_core/Rendering) содержит необходимые классы для рендеринга OpenGL:
      + Папка [OpenGL](https://github.com/richstog/OGL_engine/tree/master/r_engine_core/src/r_engine_core/Rendering/OpenGL) содержит исходные и заголовочные файлы рендеринга OpenGL;
      + Папка [NodeOpenGL]() содержит исходные и заголовочные файлы рендеринга OpenGL с использованием узлов.
+ Папка [r_engine_editor](https://github.com/richstog/OGL_engine/tree/master/r_engine_editor) содержит папки и файлы редактора:
  + Папка [src](https://github.com/richstog/OGL_engine/tree/master/r_engine_editor/src) содержит исходные файлы редактора движка.

## Библиотеки
Большая часть библиотек реализована как сабмодули GitHub ```git submodule add <ссылка> <путь>```, которые расположены в папке [external](https://github.com/richstog/OGL_engine/tree/master/external).

Библиотеки, используемые в проекте:
+ [GLAD](https://glad.dav1d.de/);
+ [GLFW](https://github.com/glfw/glfw);
+ [IMGUI](https://github.com/ocornut/imgui);
+ [SPDLOG](https://github.com/gabime/spdlog);
+ [STB](https://github.com/nothings/stb);
+ [TYNIGLTF](https://github.com/syoyo/tinygltf);
+ [TYNIOBJLOADER](https://github.com/tinyobjloader/tinyobjloader).

## Сборка CMake
В проекте описаны три файла сбоки:
+ [Файл сборки фреймворка](https://github.com/richstog/OGL_engine/blob/master/r_engine_core/CMakeLists.txt) описывает линковку библиотек, исходных и заголовочных файлов фреймворка;
+ [Файл сборки редактора](https://github.com/richstog/OGL_engine/blob/master/r_engine_editor/CMakeLists.txt) описывает линковку уже описанных во фреймворке библиотеками с редактором;
+ [Файл сборки объединения](https://github.com/richstog/OGL_engine/blob/master/CMakeLists.txt) описывает линковку фреймворка и редактора.

## Установка
Проект реализован на операционной системе Ubuntu версии 23.10. Она по умолчанию устанавливает необходимые графические драйвера для вашего устройства.

Для установки проекта необходимо выполнить следующие пункты:
+ Установить библиотеку GLFW ```sudo apt install libglfw3 libglfw3-dev```;
+ Склонировать данный репозиторий с рекурсией на сабмодули ```git clone https://github.com/richstog/OGL_engine.git --recursive```;
+ Перейти в папку проекта ```cd OGL_engine```;
+ Создать и открыть папку build ```mkdir build && cd build```;
+ Скомпилировать исходные файлы в объектные ```cmake ..```;
+ Собрать объектные файлы в один исполняемый ```cmake --build .```;
+ Запустить исполняемый файл ```./bin/r_engine_editor```.

## Для опытных пользователей
### Редактор движка
В файле [main.cpp](https://github.com/richstog/OGL_engine/blob/master/r_engine_editor/src/main.cpp) предоставлен код использования фреймворка 3D движка:
+ Создание класса, унаследованного от класса [Application](https://github.com/richstog/OGL_engine/blob/master/r_engine_core/includes/r_engine_core/Application.hpp);
+ Переопределение методов класса [Application](https://github.com/richstog/OGL_engine/blob/master/r_engine_core/includes/r_engine_core/Application.hpp):
+ + on_update;
+ + on_mouse_button_pressed_event;
+ + on_ui_draw.
+ Создание объекта, созданного ранее класса;
+ Определить размеры окна и наименование окна.

### Использование классов фреймворка движка
Для использования классов рендеринга, необходимо изменить настройку CMake сборки.
Инструкция для опытных пользователей, которые желают использовать движок и классы рендеринга:
+ Перейти в CMake файл движка, находящйися в папке "r_engine_core/CMakeLists.txt";
+ Изменить в 75 строке привязку PRIVATE на PUBLIC;
+ Если появились ошибки, то необходимо подключить библиотеки для разработки в редактор:
+ + перейти в CMake файл редактора, находящийся в папке "r_engine_editor/CMakeLists.txt";
+ + дописать необходимые библиотеки в 10 строке (библиотеки описаны в 9 строке).
 
### Примечания
Во вемя работы с классами рендеринга, необходимо учитывать следующие пункты:
+ Нельзя создать несколько приложений (некоторые другие классы также относятся к некопируемым), во избежание ошибок с памятью необходимо использовать умные указатели, к примеру: make_unique;
+ В проекте используется namespace под наименованием r_engine, только через него можно получить доступ к классам движка;
+ Рендеринги OpenGL и NodeOpenGL в папке [Rendering](https://github.com/richstog/OGL_engine/tree/master/r_engine_core/src/r_engine_core/Rendering) не взаимосвязаны друг с другом.