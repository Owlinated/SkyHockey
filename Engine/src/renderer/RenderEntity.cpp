#include "RenderEntity.h"

int RenderEntity::id_counter = 0;

RenderEntity::RenderEntity(std::shared_ptr<Shape> &shape, std::shared_ptr<Texture> &texture) :
    shape(std::move(shape)), texture(std::move(texture)), id(RenderEntity::id_counter++) {
}
