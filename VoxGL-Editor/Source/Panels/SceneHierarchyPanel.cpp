#include "SceneHierarchyPanel.h"

#include "Vox/Scene/Components.h"
#include "Vox/Scripting/ScriptEngine.h"
#include "Vox/ImGui/ImGuiWidgets.h"
#include "Vox/Core/Input.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#include "Vox/Tools/ThemePalette.h"

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
	}

	void SceneHierarchyPanel::OnRender()
	{
		ImGui::Begin("Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityId)
			{
				Entity entity{ entityId, m_Context.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				SelectionManager::DeselectAll();
			}

			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				Entity entity = m_Context->GetEntityWithId(SelectionManager::GetSelection(0));
				DrawEntityCreateMenu("Create", entity);
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
		DrawEntityCreateMenu("Create", entity);

		if (SelectionManager::GetSelectionCount() > 0)
		{
			ImGui::Separator();

			/*if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}*/

			if (ImGui::MenuItem("Duplicate Entity"))
			{
				m_Context->DuplicateEntity(entity);
			}
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity, const std::string& searchFilter)
	{
		const char* name = "New Entity";
		if (entity.HasComponent<TagComponent>())
		{
			name = entity.GetName().c_str();
		}

		const uint32_t maxSearchDepth = 10;
		bool hasChildMatchingSearch = TagSearchRecursive(entity, searchFilter, maxSearchDepth);

		if (!UI::IsMatchingSearch(name, searchFilter) && !hasChildMatchingSearch)
		{
			return;
		}

		const float rowHeight = 21.0f;

		auto* window = ImGui::GetCurrentWindow();
		window->DC.CurrLineSize.y = rowHeight;
		window->DC.CurrLineTextBaseOffset = 3.0f;

		const bool isSelected = SelectionManager::IsSelected(entity.GetId());
		ImGuiTreeNodeFlags flags = (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (hasChildMatchingSearch)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		if (entity.Children().empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		const std::string strId = fmt::format("{0}{1}", name, (uint64_t)entity.GetId());
		bool opened = ImGui::TreeNodeEx(strId.c_str(), flags, name);

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
			DrawEntityCreateMenu("Create", entity);
			ImGui::Separator();
			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}

			if (ImGui::MenuItem("Duplicate Entity"))
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
				DrawEntityNode(m_Context->GetEntityWithId(child), "");
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

	bool SceneHierarchyPanel::TagSearchRecursive(Entity entity, std::string_view searchFilter, uint32_t maxSearchDepth, uint32_t currentDepth)
	{
		if (searchFilter.empty())
		{
			return false;
		}

		for (auto child : entity.Children())
		{
			Entity e = m_Context->GetEntityWithId(child);
			if (e.HasComponent<TagComponent>())
			{
				if (UI::IsMatchingSearch(e.GetComponent<TagComponent>().Tag, searchFilter))
				{
					return true;
				}
			}

			bool found = TagSearchRecursive(e, searchFilter, maxSearchDepth, currentDepth + 1);
			if (found)
			{
				return true;
			}
		}

		return false;
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName, bool canAddMultiples) {
		Entity entity = m_Context->GetEntityWithId(SelectionManager::GetSelection(0));
		if (!entity.HasComponent<T>() || canAddMultiples)
		{
			if (ImGui::MenuItem(entryName.c_str()))
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

	void SceneHierarchyPanel::DrawEntityCreateMenu(const std::string& title, Entity parent)
	{
		if (!ImGui::BeginMenu(title.c_str()))
			return;

		Entity newEntity;

		if (ImGui::MenuItem("Empty Entity"))
		{
			newEntity = m_Context->CreateEntity("Empty Entity");
		}

		if (ImGui::MenuItem("Camera"))
		{
			newEntity = m_Context->CreateEntity("Camera");
			newEntity.AddComponent<CameraComponent>();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("2D Renderer"))
		{
			if (ImGui::MenuItem("Quad"))
			{
				newEntity = m_Context->CreateEntity("Quad");
				newEntity.AddComponent<SpriteRendererComponent>();
			}

			if (ImGui::MenuItem("Circle"))
			{
				newEntity = m_Context->CreateEntity("Circle");
				newEntity.AddComponent<CircleRendererComponent>();
			}

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("2D Physics"))
		{
			if (ImGui::MenuItem("Kinemactic Object"))
			{
				newEntity = m_Context->CreateEntity("Physics Object");
				newEntity.AddComponent<SpriteRendererComponent>();
				auto& rb = newEntity.AddComponent<Rigidbody2DComponent>();
				auto& col = newEntity.AddComponent<BoxCollider2DComponent>();
				rb.Type = Rigidbody2DComponent::BodyType::Kinematic;
			}

			if (ImGui::MenuItem("Dynamic Object"))
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
	static void DrawComponent(const std::string& title, Entity entity, DrawFunction drawFunction, bool canDelete = true)
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

			ImGui::PushFont(boldFont);
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, title.c_str());
			ImGui::PopStyleVar();
			ImGui::PopFont();

			if (canDelete)
			{
				ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
				if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
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
		bool isActive = entity.IsActive();
		entity.SetActive(ImGui::Checkbox("##Enable", &isActive));
		
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

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Position", component.Position);

			glm::vec3 rotation = glm::degrees(component.GetRotationEuler());
			DrawVec3Control("Rotation", rotation);
			component.SetRotationEuler(glm::radians(rotation));

			DrawVec3Control("Scale", component.Scale, 1.0f);
		}, false);

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
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

		DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable 
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

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::Button("Sprite", ImVec2(64.0f, 64.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(m_AssetPath) / path;
					
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
					if (texture->IsLoaded())
					{
						component.Texture = texture;
					}
					else
					{
						LOG_WARN("Could not load texture {0}", texturePath.filename().string());
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f, "%.2f");
		});
	
		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f, "%.2f");
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody2D", entity, [](auto& component)
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

		DrawComponent<BoxCollider2DComponent>("BoxCollider2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f, 0.0f, "%.2f");
		});
	
		DrawComponent<CircleCollider2DComponent>("CircleCollider2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f, 0.0f, "%.2f");
		});

		ImGui::Separator();
		const float cornerSize = 100.0f;
		float currentY = ImGui::GetCursorPosY();
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowWidth() - cornerSize, 25.0f);
		ImGui::SetCursorPos(ImVec2(cornerSize / 2.0f, currentY + 25.0f));
		
		if (ImGui::Button("Add Component", buttonSize))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::BeginMenu("Core"))
			{
				DisplayAddComponentEntry<CameraComponent>("Camera");
				DisplayAddComponentEntry<ScriptComponent>("Script", true);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Renderer"))
			{
				DisplayAddComponentEntry<SpriteRendererComponent>("SpriteRenderer");
				DisplayAddComponentEntry<CircleRendererComponent>("CircleRenderer");

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Physics"))
			{
				DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody2D");
				DisplayAddComponentEntry<BoxCollider2DComponent>("BoxCollider2D", true);
				DisplayAddComponentEntry<CircleCollider2DComponent>("CircleCollider2D", true);

				ImGui::EndMenu();
			}
			
			ImGui::EndPopup();
		}
	}
}
