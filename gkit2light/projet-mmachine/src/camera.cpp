//
// Created by joris on 19/05/22.
//

#include "camera.hpp"

Camera::Camera() {
    m_from = Point(0 ,0 ,0);
    m_to = Point(0, 1, 0);
    m_up = Vector(0, 0, 1);
}