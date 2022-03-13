#include "InteractiveObject.h"
#include "../../AstarokGame.h"

void InteractiveObject::deactivate() {

    this->x = -1;
    this->y = -1;

}

ObjectTypes InteractiveObject::getType() {

    return this->type;

}