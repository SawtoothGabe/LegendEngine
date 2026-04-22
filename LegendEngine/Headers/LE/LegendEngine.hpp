#pragma once

#include <LE/Application.hpp>

#ifndef LE_HEADLESS
#include <LE/TetherBindings.hpp>
#endif

#include <LE/Common/Assert.hpp>
#include <LE/Common/Defs.hpp>
#include <LE/Common/Platform.hpp>
#include <LE/Common/Stopwatch.hpp>
#include <LE/Common/UID.hpp>

#include <LE/Components/ActiveCamera.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Components/ComponentStorage.hpp>
#include <LE/Components/Light.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Components/Transform.hpp>

#include <LE/Events/EventBus.hpp>
#include <LE/Events/EventBusSubscriber.hpp>
#include <LE/Events/RenderEvent.hpp>
#include <LE/Events/UpdateEvent.hpp>

#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/Renderer.hpp>

#include <LE/IO/Logger.hpp>
#include <LE/IO/LoggerSinks.hpp>
#include <LE/IO/TextureData.hpp>

#include <LE/Math/Math.hpp>

#include <LE/Resources/Material.hpp>
#include <LE/Resources/Shader.hpp>
#include <LE/Resources/Texture2D.hpp>
#include <LE/Resources/Texture2DArray.hpp>

#include <LE/World/Archetype.hpp>
#include <LE/World/ECS.hpp>
#include <LE/World/Entity.hpp>
#include <LE/World/Scene.hpp>