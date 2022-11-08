#include "EditorLayer.h"

#include "Vox/Scene/SceneSerializer.h"
#include "Vox/Tools/PlatformTools.h"
#include "Vox/Math/Math.h"
#include "Vox/Scripting/ScriptEngine.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ImGuizmo.h"
#include <imgui/imgui.cpp>

namespace Vox 
{
	extern const std::filesystem::path m_AssetPath;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		VOX_PROFILE_FUNCTION();

		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
		m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");

		FramebufferSpec fbSpec;
		fbSpec.Attachments =
		{
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::Depth
		};

		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpec().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			OpenScene(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		Renderer2D::SetLineWidth(0.99f);
	}

	void EditorLayer::OnDetach()
	{
		VOX_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		VOX_PROFILE_FUNCTION();

		if (FramebufferSpec spec = m_Framebuffer->GetSpec();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
				m_EditorCamera.OnUpdate(ts);
				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			case SceneState::Play:
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			case SceneState::Simulate:
				m_EditorCamera.OnUpdate(ts);
				m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
				break;
		}

		MousePicking();
		OnOverlayRender();
		m_Framebuffer->Unbind();
	}

	void EditorLayer::RenderGrid()
	{
		static ImVector<ImVec2> points;
		static ImVec2 scrolling(0.0f, 0.0f);

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Draw border and background color
		ImGuiIO& io = ImGui::GetIO();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		// Draw grid + all lines in the canvas
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);

		if (m_EnableGrid)
		{
			const float GRID_STEP = 64.0f;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
			{
				draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
			}
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
			{
				draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
			}
		}

		draw_list->PopClipRect();
	}

	void EditorLayer::OnImGuiRender()
	{
		VOX_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("VoxGL Editor", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 350.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save Scene...", "Ctrl+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scripting"))
			{
				if (ImGui::MenuItem("Reload Assembly", "Ctrl+R"))
				{
					ScriptEngine::ReloadAssembly();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit", m_SceneHierarchyPanel.InValidContext(m_ActiveScene)))
			{
				m_SceneHierarchyPanel.DrawSelectedEntityActions();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::MenuItem("Theme Settings"))
				{
					m_ShowStyleDemo = true;
				}

				if (ImGui::MenuItem("Help"))
				{
					m_ShowImGuiDrawDemo = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		if (m_ShowStyleDemo)
		{
			ImGui::Begin("Style Editor");
			ImGui::ShowStyleEditor();
			ImGui::End();
		}

		if (m_ShowImGuiDrawDemo)
		{
			ImGui::ShowDemoWindow();
		}

		m_SceneHierarchyPanel.OnRender();
		m_ContentBrowserPanel.OnRender();
		m_EditorConsolePanel.OnRender();

		UI_Settings();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererId();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(m_AssetPath) / path);
			}

			ImGui::EndDragDropTarget();
		}

		DrawGizmos();
		//RenderGrid();
		
		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_SceneState == SceneState::Edit)
		{
			m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VOX_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(VOX_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));

		m_EditorConsolePanel.OnEvent(e);
	}

	void EditorLayer::MousePicking()
	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			if (pixelData == -1)
			{
				m_HoveredEntity = Entity();
			}
			else
			{
				m_HoveredEntity = Entity((entt::entity)pixelData, m_ActiveScene.get());
			}

			//LOG_CORE_WARN("Pixel data = {0}", pixelData);
		}
	}

	void EditorLayer::DrawGizmos()
	{
		if (m_ActiveScene->IsRunning() && !m_EnableGizmosInRuntime)
		{
			return;
		}

		Entity selectedEntity = m_ActiveScene->GetEntityWithId(SelectionManager::GetSelection(0));
		if (selectedEntity && m_GizmosType >= 0)
		{
			
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			TransformComponent& entityTransform = selectedEntity.Transform();
			glm::mat4 transform = m_ActiveScene->GetWorldSpaceTransformMatrix(selectedEntity);

			glm::mat4 projectionMatrix, viewMatrix;
			if (m_ActiveScene->IsRunning() && m_UseRuntimeCameraToGizmos)
			{
				Entity cameraEntity = m_ActiveScene->GetMainCameraEntity();
				if (!cameraEntity)
				{
					return;
				}

				SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
				projectionMatrix = camera.GetProjection();
				viewMatrix = glm::inverse(m_ActiveScene->GetWorldSpaceTransformMatrix(cameraEntity));
			}
			else
			{
				projectionMatrix = m_EditorCamera.GetProjection();
				viewMatrix = m_EditorCamera.GetViewMatrix();
			}

			float snapValue = m_SnapValue;
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			if (m_GizmosType == ImGuizmo::OPERATION::ROTATE)
			{
				snapValue = m_RotationSnap;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
				(ImGuizmo::OPERATION)m_GizmosType, (ImGuizmo::MODE)m_GizmosSpace, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				Entity parent = m_ActiveScene->FindEntityWithId(selectedEntity.GetParentId());
				if (parent)
				{
					glm::mat4 parentTransform = m_ActiveScene->GetWorldSpaceTransformMatrix(parent);
					transform = glm::inverse(parentTransform) * transform;
				}

				glm::vec3 position, scale;
				glm::quat rotation;
				Math::DecomposeTransform(transform, position, rotation, scale);

				switch (m_GizmosType)
				{
					case ImGuizmo::TRANSLATE:
						entityTransform.Position = position;
						break;
					case ImGuizmo::ROTATE:
						glm::vec3 originalRotationEuler = entityTransform.GetRotationEuler();

						originalRotationEuler.x = fmodf(originalRotationEuler.x + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
						originalRotationEuler.y = fmodf(originalRotationEuler.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
						originalRotationEuler.z = fmodf(originalRotationEuler.z + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();

						glm::vec3 deltaRotationEuler = glm::eulerAngles(rotation) - originalRotationEuler;

						if (fabs(deltaRotationEuler.x) < 0.001) deltaRotationEuler.x = 0.0f;
						if (fabs(deltaRotationEuler.y) < 0.001) deltaRotationEuler.y = 0.0f;
						if (fabs(deltaRotationEuler.z) < 0.001) deltaRotationEuler.z = 0.0f;

						glm::vec3 targetEulerRot = entityTransform.GetRotationEuler() += deltaRotationEuler;
						entityTransform.SetRotationEuler(targetEulerRot);
						break;
					case ImGuizmo::SCALE:
						entityTransform.Scale = scale;
						break;
				}
			}
		}
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | 
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
		{
			tintColor.w = 0.5f;
		}
		const float baseIconSize = 32.0f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
		bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
		bool hasPauseButton = m_SceneState != SceneState::Edit;

		if(hasPlayButton)
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererId(), ImVec2(baseIconSize, baseIconSize),
				ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		if(hasSimulateButton)
		{
			if (hasPlayButton)
			{
				ImGui::SameLine();
			}

			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererId(), ImVec2(baseIconSize, baseIconSize),
				ImVec2(0, 1), ImVec2(1, 0), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}

		if (hasPauseButton)
		{
			bool isPaused = m_ActiveScene->IsPaused();
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = m_IconPause;
				if (ImGui::ImageButton((ImTextureID)icon->GetRendererId(), ImVec2(baseIconSize, baseIconSize), ImVec2(0, 0),
					ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					m_ActiveScene->SetPaused(!isPaused);
				}
			}

			// Step button
			if (isPaused)
			{
				ImGui::SameLine();
				{
					Ref<Texture2D> icon = m_IconStep;
					bool isPaused = m_ActiveScene->IsPaused();
					if (ImGui::ImageButton((ImTextureID)icon->GetRendererId(), ImVec2(baseIconSize, baseIconSize), ImVec2(0, 0),
						ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
					{
						m_ActiveScene->Step();
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::UI_Settings()
	{
		ImGui::Begin("Settings");

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		bool gizmosOpen = ImGui::TreeNodeEx((void*)"##GizmosSettings", treeNodeFlags, "Gizmos Settings");
		ImGui::PopStyleVar();

		if (gizmosOpen)
		{
			ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);
			ImGui::Checkbox("Enable Gizmos In Runtime", &m_EnableGizmosInRuntime);
			ImGui::Checkbox("Use Runtime Camera To Gizmos", &m_UseRuntimeCameraToGizmos);

			ImGui::TreePop();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

		bool gridOpen = ImGui::TreeNodeEx((void*)"##GridSettings", treeNodeFlags, "Grid Settings");
		ImGui::PopStyleVar();

		if (gridOpen)
		{
			ImGui::Checkbox("Enable Grid", &m_EnableGrid);
			ImGui::DragFloat("SnapAmount", &m_SnapValue, 0.1f, 0.0f, 0.0f, "%.2f");

			ImGui::TreePop();
		}

		ImGui::End();
	}

	void EditorLayer::UI_GizmosToolbar()
	{
		if (!m_EnableGizmosInRuntime && m_ActiveScene->IsRunning())
		{
			return;
		}

		ImGui::PushID(0);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		const float buttonSize = 18.0f;
		const float edgeOffset = 4.0f;
		const float windowHeight = 32.0f;
		const float numberOfButtons = 4.0f;
		const float backgroundWidth = edgeOffset * 6.0f + buttonSize * numberOfButtons + edgeOffset * (numberOfButtons - 1.0f) * 2.0f;

		ImGui::SetNextWindowPos(ImVec2(m_ViewportBounds[0].x + 14, m_ViewportBounds[0].y + edgeOffset));
		ImGui::SetNextWindowSize(ImVec2(backgroundWidth, windowHeight));
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::Begin("##viewport_tools", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);

		// A hack to make icon panel appear smaller than minimum allowed by ImGui size
		// Filling the background for the desired 26px height
		const float desiredHeight = 26.0f;

		ImRect background = ImGui::GetCurrentWindow()->Rect();
		background.Min.x -= 0.0f;
		background.Min.y -= -(windowHeight - desiredHeight) / 2.0f;
		background.Max.x += 0.0f;
		background.Max.y += -(windowHeight - desiredHeight) / 2.0f;

		ImGui::GetWindowDrawList()->AddRectFilled(background.Min, background.Max, IM_COL32(15, 15, 15, 127), 4.0f);

		//Draw Gizmos Toolbar

		ImGui::End();

		ImGui::PopID();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
		{
			return false;
		}

		bool control = Input::IsKeyPressed(Key::LeftControl) ||
			Input::IsKeyPressed(Key::RightControl);

		bool shift = Input::IsKeyPressed(Key::LeftShift) ||
			Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
				if (control)
				{
					NewScene();
				}
				break;
			case Key::O:
				if (control)
				{
					OpenScene();
				}
				break;
			case Key::S:
				if (control)
				{
					if (shift)
					{
						SaveSceneAs();
					}
					else
					{
						SaveScene();
					}
				}
				break;

			//Scene Commands
			case Key::D:
				if (control)
				{
					OnDuplicateEntity();
				}
				break;

			//Gizmos
			case Key::W:
				if(!ImGuizmo::IsUsing())
					m_GizmosType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Key::E:
				if (!ImGuizmo::IsUsing())
					m_GizmosType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Key::R:
				if (control)
				{
					ScriptEngine::ReloadAssembly();
				}
				else
				{
					if (!ImGuizmo::IsUsing())
						m_GizmosType = ImGuizmo::OPERATION::SCALE;
				}
				break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				if (m_HoveredEntity)
				{
					SelectionManager::Select(m_HoveredEntity.GetId());
				}
				else
				{
					SelectionManager::DeselectAll();
				}
			}

			return false;
		}

		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetMainCameraEntity();
			if (!camera)
			{
				LOG_WARN("Scene does not have a main camera to render");
				CONSOLE_WARN("Scene does not have a main camera to render");
				return;
			}

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, 
				camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		if (m_ActiveScene->IsRunning() && !m_EnableGizmosInRuntime)
		{
			return;
		}

		if (m_ShowPhysicsColliders)
		{
			// Box Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 position = tc.Position + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
						* glm::rotate(glm::mat4(1.0f), tc.GetRotationEuler().z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle Colliders
			{
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Position + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.125f);
				}
			}
		}

		RenderOutline();
		Renderer2D::EndScene();
	}

	void EditorLayer::RenderOutline()
	{
		if (m_ActiveScene->IsRunning() && !m_EnableGizmosInRuntime)
		{
			return;
		}

		Entity selectedEntity = m_ActiveScene->GetEntityWithId(SelectionManager::GetSelection(0));
		if (selectedEntity && selectedEntity.IsActive())
		{
			const glm::mat4 transform = m_ActiveScene->GetWorldSpaceTransformMatrix(selectedEntity);
			Renderer2D::DrawRect(transform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>("New Scene", true);
		m_EditorScene = m_ActiveScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		if (path.extension().string() != ".scene")
		{
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			CONSOLE_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>("New Scene", true);
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
		{
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
		{
			OnSceneStop();
		}

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
		{
			OnSceneStop();
		}

		m_SceneState = SceneState::Simulate;
		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		VOX_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
		{
			m_ActiveScene->OnRuntimeStop();
		}
		else if (m_SceneState == SceneState::Simulate)
		{
			m_ActiveScene->OnSimulationStop();
		}

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
		{
			return;
		}

		m_ActiveScene->SetPaused(true);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			return;
		}

		Entity selectedEntity = m_ActiveScene->GetEntityWithId(SelectionManager::GetSelection(0));
		if (selectedEntity)
		{
			m_EditorScene->DuplicateEntity(selectedEntity);
		}
	}
}
