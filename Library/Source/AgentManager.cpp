#include "../Header/AgentManager.h"
#include "../Header/SceneManager.h"

#include <imgui-docking/imgui.h>

#include "../Header/Transform.h"

void AgentManager::Initialize()
{
	
}

void AgentManager::Update()
{
	DebugUI();
}

void AgentManager::DebugUI()
{
	ImGui::Begin("AgentManager");
	{
		float position[3] = { createData.createTransform.GetPosition().x,createData.createTransform.GetPosition().y,createData.createTransform.GetPosition().z };
		float rotation[3] = { createData.createTransform.GetEulerAngles().x, createData.createTransform.GetEulerAngles().y,createData.createTransform.GetEulerAngles().z };
		float scale[3] = { createData.createTransform.GetScale().x,createData.createTransform.GetScale().y,createData.createTransform.GetScale().z };

		ImGui::DragFloat3("position", position, 0.05f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("rotation", rotation, 0.05f, -FLT_MAX, FLT_MAX);
		ImGui::DragFloat3("scale", scale, 0.05f, -FLT_MAX, FLT_MAX);

		createData.createTransform.SetPosition(position);
		createData.createTransform.SetEulerAngles(rotation);
		createData.createTransform.SetScale(scale);

		ImGui::InputInt("AgentTypes", &createData.agentTypeNumber);

		if (ImGui::Button(u8"SelectAgentType"))
		{
			ImGui::OpenPopup(u8"AgentTypes");
			ImGui::SetNextWindowSize(ImVec2(300, 225));
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetMousePosOnOpeningCurrentPopup().y + 10));
		}

		if (ImGui::BeginPopup(u8"AgentTypes"))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));

			if (ImGui::Button("Agent0", ImVec2(-FLT_MIN, 0.0f)))
			{
				createData.agentTypeNumber = 0;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Agent1", ImVec2(-FLT_MIN, 0.0f)))
			{
				createData.agentTypeNumber = 1;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Agent2", ImVec2(-FLT_MIN, 0.0f)))
			{
				createData.agentTypeNumber = 2;
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}

		if (ImGui::Button("CreateAgent"))
		{
			
			CreateAgent(createData);
		}
	}
	ImGui::End();
}

void AgentManager::CreateAgent(CreateData createData) const
{
	const auto existObject = SceneManager::GetActiveScene()->CreateObject();

	switch (createData.agentTypeNumber)
	{
	case 0:
		existObject->LoadObjectTemplate("Data/ObjectTemplate/Agent0");
		break;
	case 1:
		existObject->LoadObjectTemplate("Data/ObjectTemplate/Agent1");
		break;
	case 2:
		existObject->LoadObjectTemplate("Data/ObjectTemplate/Agent2");
		break;
	default:
		return;
	}
	existObject->GetTransform()->SetPosition(createData.createTransform.GetPosition());
	existObject->GetTransform()->SetRotation(createData.createTransform.GetRotation());
	existObject->GetTransform()->SetScale(createData.createTransform.GetScale());

}
