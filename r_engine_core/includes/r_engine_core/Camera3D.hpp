#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

// Определение класса камеры
// Описание класса в файле Camera3D.cpp
namespace r_engine
{
    class Camera3D
    {
        public:

        // Перечисление типов проекции
        enum class ProjectionMode
        {
            Perspective,
            Orthographic
        };

        // Конструктор класса
        Camera3D(
            const glm::vec3& position = {0, 0, 0},
            const glm::vec3& rotation = {0, 0, 0},
            const ProjectionMode projection_mode = ProjectionMode::Perspective);

        // Сеттеры
        void set_position(const glm::vec3& position);
        void set_rotation(const glm::vec3& rotation);
        void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);
        void set_projection_mode(const ProjectionMode projection_mode);
        void set_far_clip_plane(const float far);
        void set_near_clip_plane(const float near);
        void set_viewport_size(const float width, const float height);
        void set_field_of_view(const float fov);

        // Геттеры
        const glm::mat4& get_view_matrix();
        const glm::mat4& get_projection_matrix() const { return m_projection_matrix; }
        float get_far_clip_plane() const { return m_far_clip_plane; }
        float get_near_clip_plane() const { return m_near_clip_plane; }
        float get_field_of_view() const { return m_field_of_view; }
        const glm::vec3& get_position() const { return m_position; };
        const glm::vec3& get_rotation() const { return m_rotation; };

        // Методы движения камеры
        void move_forward(const float delta);
        void move_right(const float delta);
        void move_up(const float delta);

        // Функция движения и поворота
        void add_movement_and_rotation(const glm::vec3& movement_delta, const glm::vec3& rotation_delta);

        private:

        // Функция обновления матрицы вида
        void update_view_matrix();
        // Функция обновления матрицы проекции
        void update_projection_matrix();

        glm::vec3 m_position;
        glm::vec3 m_rotation; // X - roll, Y - pitch, Z - yaw
        ProjectionMode m_projection_mode;

        // Данные перемещения камеры в собственной области координат
        glm::vec3 m_direction;
        glm::vec3 m_right;
        glm::vec3 m_up;
        // Данные фрустума
        float m_far_clip_plane {100.f};
        float m_near_clip_plane {0.1f};
        float m_viewport_width {800.f};
        float m_viewport_height {600.f};
        float m_field_of_view {60.f}; // Угол между гранями фрустума

        // Данные перемещения камеры в мировой области координат
        static constexpr glm::vec3 s_world_up { 0.f, 0.f, 1.f };
        static constexpr glm::vec3 s_world_right { 0.f, -1.f, 0.f };
        static constexpr glm::vec3 s_world_forward { 1.f, 0.f, 0.f };

        // Матрица вида
        glm::mat4 m_view_matrix;
        // Матрица проекции
        glm::mat4 m_projection_matrix;

        // Стоит ли пересчитывать матрицу вида?
        bool m_update_view_matrix = false;
    };
}