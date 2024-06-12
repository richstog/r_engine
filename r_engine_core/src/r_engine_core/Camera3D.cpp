#include "r_engine_core/Camera3D.hpp"

#include <glm/mat3x3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace r_engine
{
    Camera3D::Camera3D(
        const glm::vec3& position,
        const glm::vec3& rotation,
        const ProjectionMode projection_mode
    ) : m_position(position), m_rotation(rotation), m_projection_mode(projection_mode)
    {
        update_view_matrix();
        update_projection_matrix();
    }

    const glm::mat4& Camera3D::get_view_matrix()
    {
        if (m_update_view_matrix)
        {
            update_view_matrix();
            m_update_view_matrix = false;
        }

        return m_view_matrix;
    }

    void Camera3D::update_view_matrix()
    {
        // Переопределение углов поворота в радиансы
        const float roll_in_radians = glm::radians(m_rotation.x);
        const float pitch_in_radians = glm::radians(m_rotation.y);
        const float yaw_in_radians = glm::radians(m_rotation.z);

        // Определение матриц поворота
        const glm::mat3 rotate_matrix_x = glm::mat3(
            1, 0, 0,
            0, cos(roll_in_radians), sin(roll_in_radians),
            0, -sin(roll_in_radians), cos(roll_in_radians)
        );

        const glm::mat3 rotate_matrix_y = glm::mat3(
            cos(pitch_in_radians), 0, -sin(pitch_in_radians),
            0, 1, 0,
            sin(pitch_in_radians), 0, cos(pitch_in_radians)
        );

        const glm::mat3 rotate_matrix_z = glm::mat3(
            cos(yaw_in_radians), sin(yaw_in_radians), 0,
            -sin(yaw_in_radians), cos(yaw_in_radians), 0,
            0, 0, 1
        );

        // Определение матрицы поворота объекта по углам Эйлера
        const glm::mat3 euler_rotate_matrix = rotate_matrix_z * rotate_matrix_y * rotate_matrix_x;

        // Определение поворота по осям объекта
        m_direction = glm::normalize(euler_rotate_matrix * s_world_forward);
        m_right = glm::normalize(euler_rotate_matrix * s_world_right);
        m_up = glm::cross(m_right, m_direction);

        // Определение матрицы обзора
        m_view_matrix = glm::lookAt(m_position, m_position + m_direction, m_up);
    }

    void Camera3D::update_projection_matrix()
    {
        // Настройка фрустума камеры
        if (m_projection_mode == ProjectionMode::Perspective)
        {
            // Определение перспективной проекции камеры
            m_projection_matrix = glm::perspective(glm::radians(m_field_of_view), m_viewport_width / m_viewport_height, m_near_clip_plane, m_far_clip_plane);

        } else
        {
            // Определение ортогональной проекции камеры
            float
                r = 2,
                t = 2,
                f = 100,
                n = 0.1f;
            
            m_projection_matrix = glm::mat4(
                1/r, 0, 0, 0,
                0, 1/t, 0, 0,
                0, 0, -2/(f-n), 0,
                0, 0, (-f-n)/(f-n), 1
            );
        }
    }

    void Camera3D::set_position(const glm::vec3& position)
    {
        m_position = position;
        m_update_view_matrix = true;
    }

    void Camera3D::set_rotation(const glm::vec3& rotation)
    {
        m_rotation = rotation;
        m_update_view_matrix = true;
    }

    void Camera3D::set_position_rotation(const glm::vec3& position, const glm::vec3& rotation)
    {
        m_position = position;
        m_rotation = rotation;
        m_update_view_matrix = true;
    }

    void Camera3D::set_projection_mode(const ProjectionMode projection_mode)
    {
        m_projection_mode = projection_mode;
        update_projection_matrix();
    }

    void Camera3D::set_far_clip_plane(const float far)
    {
        m_far_clip_plane = far;
        update_projection_matrix();
    }

    void Camera3D::set_near_clip_plane(const float near)
    {
        m_near_clip_plane = near;
        update_projection_matrix();
    }

    void Camera3D::set_viewport_size(const float width, const float height)
    {
        m_viewport_width = width;
        m_viewport_height = height;
        update_projection_matrix();
    }

    void Camera3D::set_field_of_view(const float fov)
    {
        m_field_of_view = fov;
        update_projection_matrix();
    }

    void Camera3D::move_forward(const float delta)
    {
        m_position += m_direction * delta;
        m_update_view_matrix = true;
    }

    void Camera3D::move_right(const float delta)
    {
        m_position += m_right * delta;
        m_update_view_matrix = true;
    }

    void Camera3D::move_up(const float delta)
    {
        m_position += s_world_up * delta;
        m_update_view_matrix = true;
    }

    void Camera3D::add_movement_and_rotation(const glm::vec3& movement_delta, const glm::vec3& rotation_delta)
    {
        m_position += m_direction * movement_delta.x;
        m_position += m_right * movement_delta.y;
        m_position += m_up * movement_delta.z;

        m_rotation += rotation_delta;

        m_update_view_matrix = true;
    }
}