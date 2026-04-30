#pragma once

#include <SFML/Graphics.hpp>

class ProjectileSim {
public:
    ProjectileSim(sf::Vector2f origin, float groundY, float radius);

    void reset(float speed, float angleDeg);
    void update(float dt);
    void stop();

    const sf::Vector2f& position() const;
    bool isActive() const;

    void setGravity(float gravity);
    float gravity() const;
    float radius() const;

private:
    sf::Vector2f m_origin;
    float m_groundY;
    float m_radius;
    float m_gravity;

    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
    bool m_active;
};
