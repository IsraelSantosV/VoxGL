#include "SceneHierarchyPanel.h"

#include "Vox/Scene/Components.h"
#include "Vox/Scripting/ScriptEngine.h"
#include "Vox/ImGui/ImGuiWidgets.h"
#include "Vox/Core/Input.h"
#include "Vox/Tools/StringUtils.h"

#include "Vox/Editor/UITools.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#include "Vox/Tools/ThemePalette.h"

using namespace std::string_literals;

#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Vox
{
	extern const std::filesystem::path m_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		SelectionManager::DeselectAll();
		m_DrawedChildren.clear();
	}

	bool SceneHierarchyPanel::InValidContext(Ref<Scene> compareContext)
	{
		if (m_Context == nullptr)
		{
			return false;
		}

		return m_Context == compareContext;
	}

	void SceneHierarchyPanel::OnRender()
	{
		ImGui::Begin("Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityId)
			{
				Entity entity{ entityId, m_Context.get() };
				if (!m_Context->EntityIsScene(entity))
				{
					bool hasValidParent = entity.GetParent() && !HasDrawedChild(entity);
					DrawEntityNode(entity, hasValidParent);
				}
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				SelectionManager::DeselectAll();
			}

			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				DrawEntityCreateMenu(ICON_FA_SQUARE_PLUS, "Create", {});
				ImGui::EndPopup();
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (SelectionManager::GetSelectionCount() > 0)
		{
			Entity entity = m_Context->GetEntityWithId(SelectionManager::GetSelection(0));
			DrawComponents(entity);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawSelectedEntityActions()
	{
		Entity entity = m_Context->GetEntityWithId(SelectionManager::GetSelection(0));
		if (entity)
		{
			ImGui::Text(entity.GetName().c_str());
			ImGui::Separator();
		}

		DrawEntityCreateMenu(ICON_FA_SQUARE_PLUS, "Create", entity);

		if (SelectionManager::GetSelectionCount() > 0)
		{
			ImGui::Separator();

			if (ImGui::MenuItem(UI::GetText(ICON_FA_PERSON_WALKING_ARROW_LOOP_LEFT, "Unparent Entity").c_str()))
			{
				m_Context->UnparentEntity(entity);
				OnUnparentEntity(entity);
			}

			ImGui::Separator();

			/*if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}*/

			if (ImGui::MenuItem(UI::GetText(ICON_FA_COPY, "Duplicate Entity").c_str()))
			{
				m_Context->DuplicateEntity(entity);
			}
		}
	}

	void SceneHierarchyPanel::OnUnparentEntity(Entity entity)
	{
		if (HasDrawedChild(entity))
		{
			m_DrawedChildren.erase(entity.GetId());
		}
	}

	bool SceneHierarchyPanel::HasDrawedChild(Entity entity)
	{
		auto it = m_DrawedChildren.find(entity.GetId());
		return it != m_DrawedChildren.end();
	}

	const std::string SceneHierarchyPanel::GetEntityNameWithIcon(Entity entity)
	{
		if (!entity) return UI::GetText(ICON_FA_CUBE, "New Entity");
		if (entity.HasComponent<CameraComponent>()) return UI::GetText(ICON_FA_VIDEO, entity.GetName().c_str());
		if (entity.HasComponent<BehaviourComponent>()) return UI::GetText(ICON_FA_SCROLL, entity.GetName().c_str());
		if (entity.HasComponent<ScriptComponent>()) return UI::GetText(ICON_FA_SCROLL, entity.GetName().c_str());
		if (entity.HasComponent<Rigidbody2DComponent>()) return UI::GetText(ICON_FA_BOLT, entity.GetName().c_str());

		return UI::GetText(ICON_FA_CUBE, entity.GetName().c_str());
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity, bool isChild)
	{
		if (isChild)
		{
			if (!HasDrawedChild(entity))
			{
				m_DrawedChildren.emplace(entity.GetId(), entity);
			}
		}
		else if(HasDrawedChild(entity))
		{
			return;
		}

		const std::string name = GetEntityNameWithIcon(entity);
		const uint32_t maxSearchDepth = 10;
		const float rowHeight = 25.0f;

		auto* window = ImGui::GetCurrentWindow();
		window->DC.CurrLineSize.y = rowHeight;
		window->DC.CurrLineTextBaseOffset = 3.0f;

		const bool isSelected = SelectionManager::IsSelected(entity.GetId());
		ImGuiTreeNodeFlags flags = (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (entity.Children().empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		bool entityDisabled = !entity.IsActive();
		if (entityDisabled)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
		}

		const std::string strId = fmt::format("{0}{1}", name, (uint64_t)entity.GetId());
		bool opened = ImGui::TreeNodeEx(strId.c_str(), flags, name.c_str());

		if (entityDisabled)
		{
			ImGui::PopStyleColor();
		}

		if (ImGui::IsItemClicked())
		{
			bool shiftDown = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
			if (!shiftDown)
			{
				SelectionManager::DeselectAll();
				
			}
			
			SelectionManager::Select(entity.GetId());
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::Text(entity.GetName().c_str());
			ImGui::Separator();

			DrawEntityCreateMenu(ICON_FA_SQUARE_PLUS, "Create Child", entity);
			ImGui::Separator();

			if (ImGui::MenuItem(UI::GetText(ICON_FA_PERSON_WALKING_ARROW_LOOP_LEFT, "Unparent Entity").c_str(),
				"", false, entity.GetParent()))
			{
				m_Context->UnparentEntity(entity);
				OnUnparentEntity(entity);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(UI::GetText(ICON_FA_TRASH, "Delete Entity").c_str()))
			{
				entityDeleted = true;
			}

			if (ImGui::MenuItem(UI::GetText(ICON_FA_COPY, "Duplicate Entity").c_str()))
			{
				m_Context->DuplicateEntity(entity);
			}
			
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const auto& selectedEntities = SelectionManager::GetSelections();
			UUID entityID = entity.GetId();

			if (!SelectionManager::IsSelected(entityID))
			{
				const char* name = entity.GetName().c_str();
				ImGui::TextUnformatted(name);
				ImGui::SetDragDropPayload("scene_entity_hierarchy", &entityID, 1 * sizeof(UUID));
			}
			else
			{
				for (const auto& selectedEntity : selectedEntities)
				{
					Entity e = m_Context->GetEntityWithId(selectedEntity);
					const char* name = e.GetName().c_str();
					ImGui::TextUnformatted(name);
				}

				ImGui::SetDragDropPayload("scene_entity_hierarchy", selectedEntities.data(), selectedEntities.size() * sizeof(UUID));
			}

			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("scene_entity_hierarchy", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

			if (payload)
			{
				size_t count = payload->DataSize / sizeof(UUID);

				for (size_t i = 0; i < count; i++)
				{
					UUID droppedEntityID = *(((UUID*)payload->Data) + i);
					Entity droppedEntity = m_Context->GetEntityWithId(droppedEntityID);
					m_Context->SetParentEntity(droppedEntity, entity);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			for (auto child : entity.Children())
			{
				DrawEntityNode(m_Context->GetEntityWithId(child), true);
			}

			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			auto selectedEntities = SelectionManager::GetSelections();
			for (auto entityID : selectedEntities)
			{
				m_Context->DestroyEntity(m_Context->GetEntityWithId(entityID));
				if (SelectionManager::IsSelected(entityID))
				{
					SelectionManager::Deselect(entityID);
				}
			}
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const char* icon, const std::string& entryName, bool canAddMultiples) {
		Entity entity = m_Context->GetEntityWithId(SelectionManager::GetSelection(0));
		if (!entity.HasComponent<T>() || canAddMultiples)
		{
			const std::string componentName = UI::GetText(icon, entryName.c_str());
			if (ImGui::MenuItem(componentName.c_str()))
			{
				entity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, Vox::Theme::SecondaryButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Vox::Theme::SecondaryButtonHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Vox::Theme::SecondaryButtonActiveColor);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, Vox::Theme::SecondaryButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Vox::Theme::SecondaryButtonHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Vox::Theme::SecondaryButtonActiveColor);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, Vox::Theme::SecondaryButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Vox::Theme::SecondaryButtonHoveredColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Vox::Theme::SecondaryButtonActiveColor);
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawEntityCreateMenu(const char* icon, const std::string& title, Entity parent)
	{
		if (!ImGui::BeginMenu(UI::GetText(icon, title.c_str()).c_str()))
		{
			return;
		}

		Entity newEntity;

		if (ImGui::MenuItem(UI::GetText(ICON_FA_CUBE, "Empty Entity").c_str()))
		{
			newEntity = m_Context->CreateEntity("Empty Entity");
		}

		if (ImGui::MenuItem(UI::GetText(ICON_FA_VIDEO, "Camera").c_str()))
		{
			newEntity = m_Context->CreateEntity("Camera");
			newEntity.AddComponent<CameraComponent>();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu(UI::GetText(ICON_FA_IMAGE, "2D Renderer").c_str()))
		{
			if (ImGui::MenuItem(UI::GetText(ICON_FA_VECTOR_SQUARE, "Quad").c_str()))
			{
				newEntity = m_Context->CreateEntity("Quad");
				newEntity.AddComponent<SpriteRendererComponent>();
			}

			if (ImGui::MenuItem(UI::GetText(ICON_FA_CIRCLE_NOTCH, "Circle").c_str()))
			{
				newEntity = m_Context->CreateEntity("Circle");
				newEntity.AddComponent<CircleRendererComponent>();
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu(UI::GetText(ICON_FA_BOLT, "2D Physics").c_str()))
		{
			if (ImGui::MenuItem(UI::GetText(ICON_FA_PERSON, "Kinemactic Object").c_str()))
			{
				newEntity = m_Context->CreateEntity("Physics Object");
				newEntity.AddComponent<SpriteRendererComponent>();
				auto& rb = newEntity.AddComponent<Rigidbody2DComponent>();
				auto& col = newEntity.AddComponent<BoxCollider2DComponent>();
				rb.Type = Rigidbody2DComponent::BodyType::Kinematic;
			}

			if (ImGui::MenuItem(UI::GetText(ICON_FA_PERSON_WALKING, "Dynamic Object").c_str()))
			{
				newEntity = m_Context->CreateEntity("Physics Object");
				newEntity.AddComponent<SpriteRendererComponent>();
				auto& rb = newEntity.AddComponent<Rigidbody2DComponent>();
				auto& col = newEntity.AddComponent<BoxCollider2DComponent>();
				rb.Type = Rigidbody2DComponent::BodyType::Dynamic;
			}

			ImGui::EndMenu();
		}

		if (newEntity)
		{
			if (parent)
			{
				m_Context->SetParentEntity(newEntity, parent);
			}

			SelectionManager::DeselectAll();
			SelectionManager::Select(newEntity.GetId());
		}

		ImGui::EndMenu();
	}

	template<typename T, typename DrawFunction>
	static void DrawComponent(const char* icon, const std::string& title, Entity entity, DrawFunction drawFunction, bool canDelete = true)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();

			//ImGui::PushFont(boldFont);
			const std::string componentName = UI::GetText(icon, title.c_str());
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, componentName.c_str());
			ImGui::PopStyleVar();
			//ImGui::PopFont();

			if (canDelete)
			{
				ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
				if (ImGui::Button(ICON_FA_PEN_TO_SQUARE, ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem(UI::GetText(ICON_FA_TRASH, "Remove Component").c_str()))
				{
					removeComponent = true;
				}

				ImGui::EndPopup();
			}

			if (open)
			{
				drawFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<VisibilityComponent>())
		{
			auto& component = entity.GetComponent<VisibilityComponent>();
			ImGui::Checkbox("##IsActive", &component.IsActive);
		}
		
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::PopItemWidth();

		DrawComponent<LayerComponent>(ICON_FA_LAYER_GROUP, "Layer", entity, [](auto& component)
		{
			float width = ImGui::GetContentRegionAvailWidth();
			ImGui::PushItemWidth(width);
			ImGui::InputInt("##Layer", &component.Layer);
			if (component.Layer < 0)
			{
				component.Layer = 0;
			}

			ImGui::PopItemWidth();
		}, false);

		DrawComponent<TransformComponent>(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, "Transform", entity, [](auto& component)
		{
			DrawVec3Control("Position", component.Position);

			glm::vec3 rotation = glm::degrees(component.GetRotationEuler());
			DrawVec3Control("Rotation", rotation);
			component.SetRotationEuler(glm::radians(rotation));

			DrawVec3Control("Scale", component.Scale, 1.0f);
		}, false);

		DrawComponent<CameraComponent>(ICON_FA_VIDEO, "Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::Checkbox("Main Camera", &component.MainCamera);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjTypeStrings = projectionTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection Type", currentProjTypeStrings))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjTypeStrings == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjTypeStrings = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFov());
				if (ImGui::DragFloat("FOV", &verticalFov, 0.1f, 0.0f, 0.0f, "%.2f"))
					camera.SetPerspectiveVerticalFov(glm::radians(verticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear, 0.1f, 0.0f, 0.0f, "%.2f"))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar, 0.1f, 0.0f, 0.0f, "%.2f"))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}

			if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize, 0.1f, 0.0f, 0.0f, "%.2f"))
					camera.SetOrthographicSize(orthoSize);

				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear, 0.1f, 0.0f, 0.0f, "%.2f"))
					camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar, 0.1f, 0.0f, 0.0f, "%.2f"))
					camera.SetOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
			}
		});

		DrawComponent<ScriptComponent>(ICON_FA_SCROLL, "Script", entity, [entity, scene = m_Context](auto& component) mutable 
		{
			bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

			static char buffer[64];
			strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

			if (!scriptClassExists)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

			if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				component.ClassName = buffer;

			// Fields
			bool sceneRunning = scene->IsRunning();
			if (sceneRunning)
			{
				Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetId());
				if (scriptInstance)
				{
					const auto& fields = scriptInstance->GetScriptClass()->GetFields();
					for (const auto& [name, field] : fields)
					{
						if (field.Type == ScriptFieldType::Float)
						{
							float data = scriptInstance->GetFieldValue<float>(name);
							if (ImGui::DragFloat(name.c_str(), &data, 0.1f, 0.0f, 0.0f, "%.2f"))
							{
								scriptInstance->SetFieldValue(name, data);
							}
						}
					}
				}
			}
			else
			{
				if (scriptClassExists)
				{
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
					const auto& fields = entityClass->GetFields();

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					for (const auto& [name, field] : fields)
					{
						// Field has been set in editor
						if (entityFields.find(name) != entityFields.end())
						{
							ScriptFieldInstance& scriptField = entityFields.at(name);

							// Display control to set it maybe
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptField.GetValue<float>();
								if (ImGui::DragFloat(name.c_str(), &data, 0.1f, 0.0f, 0.0f, "%.2f"))
									scriptField.SetValue(data);
							}
						}
						else
						{
							// Display control to set it maybe
							if (field.Type == ScriptFieldType::Float)
							{
								float data = 0.0f;
								if (ImGui::DragFloat(name.c_str(), &data, 0.1f, 0.0f, 0.0f, "%.2f"))
								{
									ScriptFieldInstance& fieldInstance = entityFields[name];
									fieldInstance.Field = field;
									fieldInstance.SetValue(data);
								}
							}
						}
					}
				}
			}

			if (!scriptClassExists)
				ImGui::PopStyleColor();
		});

		DrawComponent<SpriteRendererComponent>(ICON_FA_IMAGE, "Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			if (component.Texture)
			{
				const bool clicked = ImGui::ImageButton((ImTextureID)component.Texture->GetRendererId(),
					ImVec2(64.0f, 64.0f), ImVec2(0, 1), ImVec2(1, 0));

				if (clicked)
				{
					component.SetSprite("");
				}
			}
			else
			{
				ImGui::Button("Sprite", ImVec2(64.0f, 64.0f));
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(m_AssetPath) / path;
					component.SetSprite(texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			if (component.Texture)
			{
				ImGui::SameLine();
				ImGui::DragFloat("Tiling", &component.TilingFactor, 0.1f, 0.0f, 100.0f, "%.2f");
			}
		});
	
		DrawComponent<CircleRendererComponent>(ICON_FA_CIRCLE_NOTCH, "Circle Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f, "%.2f");
		});

		DrawComponent<Rigidbody2DComponent>(ICON_FA_BOLT, "Rigidbody2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = (Rigidbody2DComponent::BodyType)i;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>(ICON_FA_BOX, "BoxCollider2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
		});
	
		DrawComponent<CircleCollider2DComponent>(ICON_FA_CIRCLE_NODES, "CircleCollider2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
		});
		
		DrawAddComponentPopup(entity);
	}

	void SceneHierarchyPanel::DrawAddComponentPopup(Entity entity)
	{
		ImGui::Separator();
		const float cornerSize = 100.0f;
		float currentY = ImGui::GetCursorPosY();
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowWidth() - cornerSize, 25.0f);
		ImGui::SetCursorPos(ImVec2(cornerSize / 2.0f, currentY + 25.0f));

		if (ImGui::Button(UI::GetText(ICON_FA_PLUS, "Add Component").c_str(), buttonSize))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			static ImGuiTextFilter filter;
			filter.Draw(ICON_FA_MAGNIFYING_GLASS);

			ImGui::Spacing();
			ImGui::Separator();

			if (filter.IsActive())
			{
				TryDrawComponentEntry<CameraComponent>(ICON_FA_VIDEO, "Camera", filter);
				TryDrawComponentEntry<ScriptComponent>(ICON_FA_SCROLL, "Script", filter, true);

				TryDrawComponentEntry<SpriteRendererComponent>(ICON_FA_IMAGE, "SpriteRenderer", filter);
				TryDrawComponentEntry<CircleRendererComponent>(ICON_FA_CIRCLE_NOTCH, "CircleRenderer", filter);


				TryDrawComponentEntry<Rigidbody2DComponent>(ICON_FA_BOLT, "Rigidbody2D", filter);
				TryDrawComponentEntry<BoxCollider2DComponent>(ICON_FA_BOX, "BoxCollider2D", filter, true);
				TryDrawComponentEntry<CircleCollider2DComponent>(ICON_FA_CIRCLE_NODES, "CircleCollider2D", filter, true);
			}
			else
			{
				if (ImGui::BeginMenu("Core"))
				{
					DisplayAddComponentEntry<CameraComponent>(ICON_FA_VIDEO, "Camera");
					DisplayAddComponentEntry<ScriptComponent>(ICON_FA_SCROLL, "Script", true);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Renderer"))
				{
					DisplayAddComponentEntry<SpriteRendererComponent>(ICON_FA_IMAGE, "SpriteRenderer");
					DisplayAddComponentEntry<CircleRendererComponent>(ICON_FA_CIRCLE_NOTCH, "CircleRenderer");

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Physics"))
				{
					DisplayAddComponentEntry<Rigidbody2DComponent>(ICON_FA_BOLT, "Rigidbody2D");
					DisplayAddComponentEntry<BoxCollider2DComponent>(ICON_FA_BOX, "BoxCollider2D", true);
					DisplayAddComponentEntry<CircleCollider2DComponent>(ICON_FA_CIRCLE_NODES, "CircleCollider2D", true);

					ImGui::EndMenu();
				}
			}
			
			ImGui::EndPopup();
		}
	}

	template<typename T>
	void SceneHierarchyPanel::TryDrawComponentEntry(const char* icon, const std::string& entryName, ImGuiTextFilter filter, bool canAddMultiples)
	{
		if (filter.PassFilter(entryName.c_str()))
		{
			DisplayAddComponentEntry<T>(icon, entryName, canAddMultiples);
		}
	}
}
