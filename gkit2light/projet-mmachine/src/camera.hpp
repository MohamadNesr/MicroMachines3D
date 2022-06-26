//
// Created by joris on 19/05/22.
//

#ifndef PROJET_MMACHINE_CAMERA_H
#define PROJET_MMACHINE_CAMERA_H

#include "mat.h"


class Camera {
public:
    Point m_from;
    Point m_to;
    Vector m_up;

    Camera();

};


#endif //PROJET_MMACHINE_CAMERA_H
