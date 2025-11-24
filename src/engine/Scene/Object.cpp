/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Object.h"

#include "Managers/cLayer_Manager.h"

void sk::Object::iObject::SetLayer( const uint64_t _layer )
{
    auto& layerManager = Scene::cLayer_Manager::get();
    m_layer_ = _layer;
    layerManager.AddObject( get_shared() );
}
