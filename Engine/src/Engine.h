#pragma once

// application
#include <engine/application/application.h>
#include <engine/application/event.h>

// layers
#include <engine/layers/layer_set.h>

// input
#include <engine/input/input.h>

// sprites
#include <engine/sprite/sprite_atlas.h>

// prefabs
#include <engine/entity/prefabs/prefab_atlas.h>

// scenes and entities
#include <engine/entity/scene.h>
#include <engine/entity/entity.h>
#include <engine/entity/scriptable_entity.h>
#include <engine/entity/component/components.h>

// lua
#include <engine/lua/script_environment.h>

// debug logging
#include <engine/watchdog/watchdog.h>
#include <engine/watchdog/imgui/console/console_panel.h>
#include <engine/watchdog/imgui/explorer/explorer.h>

// renderer
#include <render/renderer.h>

#include <render/array/vertex_array.h>
#include <render/buffer/vertex.h>
#include <render/buffer/index.h>
#include <render/shader/shader.h>
#include <render/texture/texture.h>