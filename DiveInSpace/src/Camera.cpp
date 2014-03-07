#include "Camera.h"

void Camera::camera_compute()
{
    m_eye.x = cos(m_theta) * sin(m_phi) * m_radius + m_o.x;   
    m_eye.y = cos(m_phi) * m_radius + m_o.y ;
    m_eye.z = sin(m_theta) * sin(m_phi) * m_radius + m_o.z;   
    m_up = glm::vec3(0.f, m_phi < M_PI ?1.f:-1.f, 0.f);
}

void Camera::camera_defaults()
{
    m_phi = 3.14/2.f;
    m_theta = 3.14/2.f;
    m_radius = 10.f;
    camera_compute();
}

void Camera::camera_zoom(float factor)
{
    m_radius += factor * m_radius ;
    if (m_radius < 0.1)
    {
        m_radius = 10.f;
        m_o = m_eye + glm::normalize(m_o - m_eye) * m_radius;
    }
    camera_compute();
}

void Camera::camera_turn(float phi, float theta)
{
    m_theta += 1.f * theta;
    m_phi   -= 1.f * phi;
    if (m_phi >= (2 * M_PI) - 0.1 )
        m_phi = 0.00001;
    else if (m_phi <= 0 )
        m_phi = 2 * M_PI - 0.1;
    camera_compute();
}

void Camera::camera_pan(float x, float y)
{
    glm::vec3 up(0.f, m_phi < M_PI ?1.f:-1.f, 0.f);
    glm::vec3 fwd = glm::normalize(m_o - m_eye);
    glm::vec3 side = glm::normalize(glm::cross(fwd, m_up));
    m_up = glm::normalize(glm::cross(side, fwd));
    m_o[0] += m_up[0] * y * m_radius * 2;
    m_o[1] += m_up[1] * y * m_radius * 2;
    m_o[2] += m_up[2] * y * m_radius * 2;
    m_o[0] -= side[0] * x * m_radius * 2;
    m_o[1] -= side[1] * x * m_radius * 2;
    m_o[2] -= side[2] * x * m_radius * 2;       
    camera_compute();
}