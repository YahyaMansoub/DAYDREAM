#include "ProjectileSim.h"

#include <cmath>

namespace {
constexpr float kPi = 3.14159265358979323846f;
}

ProjectileSim::ProjectileSim(sf::Vector2f origin, float groundY, float radius)
    : m_origin(origin),
      m_groundY(groundY),
      m_radius(radius),
      m_gravity(500.f),
      m_position(origin),
      m_velocity(0.f, 0.f),
      m_active(false) {}

void ProjectileSim::reset(float speed, float angleDeg) {
    const float angleRad = angleDeg * kPi / 180.f;
    m_velocity.x = speed * std::cos(angleRad);
    m_velocity.y = -speed * std::sin(angleRad);
    m_position = m_origin;
    m_active = true;
}

void ProjectileSim::update(float dt) {
    if (!m_active) {
        m_position = m_origin;
        return;
    }

    m_velocity.y += m_gravity * dt;
    m_position += m_velocity * dt;

    if (m_position.y >= m_groundY) {
        m_position.y = m_groundY;
        m_active = false;
    }
}

void ProjectileSim::stop() {
    m_active = false;
    m_position = m_origin;
    m_velocity = sf::Vector2f(0.f, 0.f);
}

const sf::Vector2f& ProjectileSim::position() const {
    return m_position;
}

bool ProjectileSim::isActive() const {
    return m_active;
}

void ProjectileSim::setGravity(float gravity) {
    m_gravity = gravity;
}

float ProjectileSim::gravity() const {
    return m_gravity;
}

float ProjectileSim::radius() const {
    return m_radius;
}
