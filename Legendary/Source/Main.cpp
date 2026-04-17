#include <chrono>
#include <random>

#include <LE/LegendEngine.hpp>

using namespace le;
using namespace std::literals::chrono_literals;

class EntityManager
{
public:
	explicit EntityManager(Scene* pScene, const Ref<MeshData>& mesh)
		:
		m_entity(pScene->CreateEntity())
	{
		m_entity.AddComponent<Transform>();
		m_entity.AddComponent<Mesh>();
		m_entity.QueryComponents<Transform, Mesh>([&](Transform& transform, Mesh& meshComp)
		{
			transform.SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(0, 1, 0)));
			meshComp.data = mesh;
		});
	}

	Entity m_entity;
};

class CameraManager final : public Input::InputListener
{
public:
	explicit CameraManager(Scene* pScene)
		:
		m_camera(pScene->CreateEntity()),
		m_Window(Application::Get().GetWindow()),
		m_Sub(Application::Get().GetEventBus())
	{
		m_camera.AddComponent<ActiveCamera>();
		m_camera.AddComponent<Camera>();
		m_camera.AddComponent<Transform>();

		m_camera.QueryComponents<Camera, Transform>([](Camera& camera, Transform& transform)
		{
			camera.SetNearZ(0.01f);
			transform.SetPosition(Vector3f(-1.0f, 2.0f, 4.0f));
		});

		m_Window.AddInputListener(*this, Input::InputType::KEY);
		m_Window.AddInputListener(*this, Input::InputType::RAW_MOUSE_MOVE);
		m_Window.AddInputListener(*this, Input::InputType::MOUSE_CLICK);

		m_Sub.AddEventHandler<UpdateEvent>([this](const UpdateEvent& e)
		{
			OnUpdate(e.GetDeltaTime());
		});
	}

	void OnUpdate(const float delta)
	{
		Vector2f moveDir;
		if (keys.w)
			moveDir += Vector2f(1, 0);
		if (keys.a)
			moveDir += Vector2f(0, -1);
		if (keys.s)
			moveDir += Vector2f(-1, 0);
		if (keys.d)
			moveDir += Vector2f(0, 1);

		Vector2f normMoveDir = Math::Normalize(moveDir);
		if (keys.ctrl)
			normMoveDir *= 3.0f;

		m_camera.QueryComponents<Camera, Transform>([&](Camera& camera, Transform& transform)
		{
			Vector3f forward = camera.GetForwardVector();
			forward.y = 0;
			forward = Math::Normalize(forward);

			transform.AddPosition(forward * normMoveDir.x * delta);
			transform.AddPosition(camera.GetRightVector() * normMoveDir.y * delta);

			// Vertical movement
			if (keys.space)
				transform.AddPosition(Vector3f(0, 1, 0) * delta);
			if (keys.shift)
				transform.AddPosition(Vector3f(0, -1, 0) * delta);
		});
	}

	void OnKey(Input::KeyInfo& info) override
	{
		const bool pressed = info.IsPressed();
		switch (info.GetKey())
		{
			case Keycodes::KEY_W: keys.w = pressed; break;
			case Keycodes::KEY_A: keys.a = pressed; break;
			case Keycodes::KEY_S: keys.s = pressed; break;
			case Keycodes::KEY_D: keys.d = pressed; break;

			case Keycodes::KEY_SPACE: keys.space = pressed; break;
			case Keycodes::KEY_LEFT_SHIFT: keys.shift = pressed; break;
			case Keycodes::KEY_LEFT_CONTROL: keys.ctrl = pressed; break;

			case Keycodes::KEY_ESCAPE:
			{
				m_Window.SetCursorMode(Tether::Window::CursorMode::NORMAL);
				m_CaptureMouse = false;
			}
			break;

			default: break;
		}
	}

	const float sense = 0.04f;
	void OnRawMouseMove(Input::RawMouseMoveInfo& info) override
	{
		if (!m_CaptureMouse)
			return;

		horizontal += static_cast<float>(info.GetRawX()) * sense;
		vertical += static_cast<float>(info.GetRawY()) * sense;

		if (vertical > 89.9f)
			vertical = 89.9f;
		if (vertical < -89.9f)
			vertical = -89.9f;

		Quaternion q = Math::AngleAxis(Math::Radians(vertical), Vector3f(1, 0, 0));
		q *= Math::AngleAxis(Math::Radians(horizontal), Vector3f(0, 1, 0));

		m_camera.QueryComponents<Transform>([&q](Transform& transform)
		{
			transform.SetRotation(q);
		});
	}

	void OnMouseClick(Input::MouseClickInfo& info) override
	{
		if (m_CaptureMouse)
			return;

		m_Window.SetCursorMode(Tether::Window::CursorMode::DISABLED);
		m_CaptureMouse = true;
	}

	Vector3f GetForwardVec()
	{
		return m_camera.GetComponentData<Camera>().GetForwardVector();
	}

	struct Keys
	{
		bool w = false;
		bool a = false;
		bool s = false;
		bool d = false;
		bool space = false;
		bool shift = false;
		bool ctrl = false;
	};
	Keys keys;

	float horizontal = 0.0f;
	float vertical = 0.0f;

	Entity m_camera;
private:
	bool m_CaptureMouse = true;

	Tether::Window& m_Window;
	EventBusSubscriber m_Sub;
};

struct Velocity : Component
{
	Vector3f direction;
};

class Legendary final : public Input::InputListener
{
public:
	explicit Legendary(Application& app)
		:
		m_App(app),
		m_Sub(m_App.GetEventBus()),
		m_mesh(CreateMesh()),
		camera(&testScene),
		cube1(&testScene, m_mesh),
		cube2(&testScene, m_mesh),
		floor(&testScene, m_mesh)
	{
		const Ref<Shader> shader = app.GetGraphicsContext().GetShaderManager().TryCreateFromId("builtin_textured");
		shader->SetCullMode(CullMode::NONE);

		CreateMaterials();
		CreateObjects();

		m_App.SetActiveScene(testScene);

		Window& window = m_App.GetWindow();
		window.SetCursorMode(Tether::Window::CursorMode::DISABLED);
		window.SetRawInputEnabled(true);
		window.AddInputListener(*this, InputType::MOUSE_CLICK);

		m_Sub.AddEventHandler<UpdateEvent>(
			[this](const UpdateEvent& e) { OnUpdate(e.GetDeltaTime()); });

		timer.Set();
	}

	void OnUpdate(float delta)
	{
		if (fpsTimer.GetElapsedMillis() >= 5000.0f)
		{
			LE_INFO("FPS: {}", m_Frames / 5);
			fpsTimer.Set();
			m_Frames = 0;
		}

		constexpr float drag = 0.1f;
		const float dragMult = std::pow(drag, delta);
		testScene.QueryComponents<Velocity, Transform>([&](Velocity& velocity, Transform& transform)
		{
			const Vector3f position = transform.GetPosition();

			if (m_mouseButton)
			{
				Vector3f cameraPos = camera.m_camera.GetComponentData<Transform>().GetPosition();
				Vector3f forward = camera.GetForwardVec();

				float distanceToPlane = -cameraPos.y / forward.y;
				Vector3f planePos = {
					cameraPos.x + distanceToPlane * forward.x,
					0.0f,
					cameraPos.z + distanceToPlane * forward.z
				};

				Vector3f directionToPlanePos = Math::Normalize(planePos - position);
				directionToPlanePos.y = 0.0f;

				const float distance = std::sqrt(
					std::pow(directionToPlanePos.x, 2.0f) +
					std::pow(directionToPlanePos.z, 2.0f)
				);
				const float speed = 1.0f / std::pow(distance, 2.0f);
				Vector3f acceleration = directionToPlanePos * speed * delta * 10.0f;

				if (m_mouseButton == 3)
					acceleration = -acceleration;

				velocity.direction += acceleration;
			}

			// Bounce off walls
			if (position.x > 5.0f || position.x < -5.0f)
				velocity.direction.x = -velocity.direction.x;
			if (position.z > 5.0f || position.z < -5.0f)
				velocity.direction.z = -velocity.direction.z;

			// Apply drag
			velocity.direction *= dragMult;

			// Apply velocity
			transform.AddPosition(velocity.direction * delta);
		});

		m_Frames++;
	}

	void OnMouseClick(MouseClickInfo& info) override
	{
		m_mouseButton = static_cast<uint32_t>(info.GetClickType()) * info.IsPressed() + info.IsPressed();
	}
private:
	static Ref<MeshData> CreateMesh()
	{
		MeshData::Vertex3 testVertices[] =
		{
			{0.5f, 0.5f, 0.0f, 1.0f, 1.0f},
			{0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
			{-0.5f, -0.5f, 0.0f, 0.0f, 0.0f},
			{-0.5f, 0.5f, 0.0f, 0.0f, 1.0f}
		};

		uint32_t indices[] =
		{
			0, 1, 3,
			1, 2, 3
		};

		return MeshData::Create(
			std::span<MeshData::Vertex3>(testVertices),
			std::span<uint32_t>(indices), MeshData::UpdateFrequency::UPDATES_ONCE);
	}

	void CreateMaterials()
	{
		material = Material::Create();
		material2 = Material::Create();

		std::future<TextureData> planksLoader = TextureData::FromFile("Assets/planks.png");
		std::future<TextureData> tilesLoader = TextureData::FromFile("Assets/tiles.png");

		texture = Texture2D::Create(planksLoader.get());
		texture2 = Texture2D::Create(tilesLoader.get());

		material->SetTexture(texture);
		material2->SetTexture(texture2);
	}

	void CreateObjects()
	{
		cube1.m_entity.QueryComponents<Transform, Mesh>([&](Transform& transform, Mesh& mesh)
		{
			transform.SetPosition(Vector3f(0.0f, 0.5f, 0.0f));
			mesh.material = material;
		});

		cube2.m_entity.QueryComponents<Transform, Mesh>([&](Transform& transform, Mesh& mesh)
		{
			transform.SetPosition(Vector3f(3.0f, 0.5f, 0.0f));
			mesh.material = material;
		});

		floor.m_entity.QueryComponents<Transform, Mesh>([&](Transform& transform, Mesh& mesh)
		{
			transform.SetScale(Vector3f(10));
			transform.SetRotation(Math::AngleAxis(Math::Radians(90.0f), Vector3f(1, 0, 0)));
			mesh.material = material2;
		});

		std::mt19937 g(1);
		std::uniform_real_distribution dist(0.0f, 1.0f);

		for (size_t i = 0; i < 100; i++)
		{
			Entity light = testScene.CreateEntity();
			light.AddComponent<Transform>();
			light.AddComponent<Light>();
			light.AddComponent<Velocity>();
			light.QueryComponents<Transform, Light, Velocity>([&](Transform& transform, Light& data, Velocity& dir)
			{
				transform.SetPosition(Vector3f(dist(g) * 10.0f - 5.0f, 0.1f, dist(g) * 10.0f - 5.0f));
				data.color = Vector4f(dist(g), dist(g), dist(g), 0.04f);
			});
		}
	}

	Application& m_App;
	EventBusSubscriber m_Sub;

	Stopwatch timer;
	Stopwatch fpsTimer;

	Scene testScene;

	Ref<MeshData> m_mesh;

	CameraManager camera;
	EntityManager cube1;
	EntityManager cube2;
	EntityManager floor;

	Ref<Material> material;
	Ref<Material> material2;
	Ref<Texture2D> texture;
	Ref<Texture2D> texture2;

	uint32_t m_mouseButton = 0;

	size_t m_Frames = 0;
};

#include <LE/Common/Entrypoint.hpp>
LEGENDENGINE_MAIN
{
	Application::Create(GraphicsAPI::VULKAN, "Legendary", 1280, 720);

	{
		Legendary legend(Application::Get());
		Application::Run();
	}

	Application::Destroy();
}
