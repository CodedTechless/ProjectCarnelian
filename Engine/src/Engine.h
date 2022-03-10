#pragma once

// application
#include <engine/application/application.h>
#include <engine/application/event.h>

// layers
#include <engine/application/layers/layer_set.h>

// input
#include <engine/application/input/input.h>

// debug logging
#include <engine/application/watchdog/watchdog.h>
#include <engine/application/watchdog/imgui/console/console_panel.h>
#include <engine/application/watchdog/imgui/explorer/explorer.h>

// sprites
#include <engine/sprite/sprite_atlas.h>

// prefabs
#include <engine/entity/prefabs/prefab_atlas.h>

// scenes and entities
#include <engine/entity/scene.h>
#include <engine/entity/entity.h>
#include <engine/entity/scriptable_entity.h>
#include <engine/entity/components.h>

// lua
#include <engine/lua/script_environment.h>

// renderer
#include <render/renderer.h>

#include <render/array/vertex_array.h>
#include <render/buffer/vertex.h>
#include <render/buffer/index.h>
#include <render/shader/shader.h>
#include <render/texture/texture.h>