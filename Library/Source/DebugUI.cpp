#include "../Header/DebugUI.h"
#include "../Header/StartingOver.h"

#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_win32.h"
#include "imgui-docking/imgui_impl_dx11.h"
#include "imgui-docking/imgui_stdlib.h"
#include <d3d11.h>
#include <wrl.h>
#include <Windows.h>
#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf2e0

#include "../../HideSeek/Header/BlockManager.h"
#include "../Header/SceneManager.h"
#include "../Header/RenderObjectmanager.h"
#include "../HideSeek/Header/GameScene.h"
#include "../Header/Object.h"
#include "../Header/Component.h"
#include "../Header/Transform.h"
#include "../Header/ExistObject.h"
#include "../../HideSeek/Header/Block.h"
#include "../Header/StaticMesh.h"
#include "../Header/SkinnedMesh.h"
#include "../Header/FileFunction.h"
#include "../Header/RigidBody.h"
#include "../HideSeek/Header/Agent.h"

void DebugUI::SelectCreateObject()
{
	if (ImGui::Button("Create Object"))
	{
		selectObject = SceneManager::GetActiveScene()->CreateObject();

	}

	if(ImGui::Button("Save"))
	{
		SceneManager::SaveScene();
	}

	if (ImGui::Button("Load"))
	{
		SceneManager::LoadScene();
	}
}

void DebugUI::DebugUpdate()
{
	SelectCreateObject();

	Inspector();
	Hierarchy();
}

void DebugUI::Inspector()
{
	ImGui::Begin("Inspector");
	{
		if (selectObject)
		{
			std::string name = selectObject->GetName();
			ImGui::InputText("name ", &name);
			selectObject->SetName(name);
			Transform* transform = selectObject->GetTransform().get();
			float position[3] = { transform->GetPosition().x,transform->GetPosition().y,transform->GetPosition().z };
			float rotation[3] = { transform->GetEulerAngles().x,transform->GetEulerAngles().y,transform->GetEulerAngles().z };
			float scale[3] = { transform->GetScale().x,transform->GetScale().y,transform->GetScale().z };

			ImGui::DragFloat3("position", position, 0.05f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat3("rotation", rotation, 0.05f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat3("scale", scale, 0.05f, -FLT_MAX, FLT_MAX);

			transform->SetPosition(position);
			transform->SetEulerAngles(rotation);
			transform->SetScale(scale);

			if (ImGui::Button(u8"template読み込み", ImVec2(-FLT_MIN, 0.0f)))
			{
				const std::string fileName = FileFunction::OpenExplorer();
				if (!fileName.empty())
				{
					selectObject->ClearComponents();
					selectObject->LoadObjectTemplate(fileName);
				}
			}

			if (ImGui::Button(u8"template保存", ImVec2(-FLT_MIN, 0.0f)))
			{
				
				selectObject->CreateObjectTemplate(selectObject->GetName());
				
			}

			for (auto& component : selectObject->GetComponents())
			{
				component->DebugUI();
			}
			ImGui::Separator();

			ImGui::Dummy(ImVec2(0.0f, 100.0f));

			if (ImGui::Button(u8"コンポーネントを追加", ImVec2(-FLT_MIN, 0.0f)))
			{
				ImGui::OpenPopup(u8"コンポーネントリスト");
				ImGui::SetNextWindowSize(ImVec2(300, 225));
				ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetMousePosOnOpeningCurrentPopup().y +10));
			}

			if (ImGui::BeginPopup(u8"コンポーネントリスト"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));

				if(ImGui::Button("Block", ImVec2(-FLT_MIN, 0.0f)))
				{
					selectObject->AddComponent<Block>();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Static Mesh", ImVec2(-FLT_MIN, 0.0f)))
				{
					selectObject->AddComponent<StaticMesh>();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Skinned Mesh", ImVec2(-FLT_MIN, 0.0f)))
				{
					selectObject->AddComponent<SkinnedMesh>();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Agent", ImVec2(-FLT_MIN, 0.0f)))
				{
					selectObject->AddComponent<Agent>();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("RigidBody", ImVec2(-FLT_MIN, 0.0f)))
				{
					selectObject->AddComponent<RigidBody>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleVar();
				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();
}



void DebugUI::Hierarchy()
{
	ImGui::Begin("Hierarchy");
	{
		for(auto& objects : SceneManager::GetActiveScene()->ObjectList())
		{
			for(auto& object : objects.second)
			{
				if(ImGui::Button( object->GetName().c_str()))
				{
					selectObject = object;
				}
			}
		}
	}
	ImGui::End();
}










void DebugUI::Initialize(HWND hWnd)
{
	ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	// setup imgui
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// setup platform/renderer
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device, context);

	ImGui::StyleColorsDark();

	ImGuiLoadJapaneseFont();

}

void DebugUI::ImGuiLoadJapaneseFont()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	//日本語フォントに対応
	const std::string fontPath = "Data/Font/mplus-1p-medium.ttf";
	constexpr float fontPixelSize = 30.0f;
	constexpr float fontSize = 0.6f;
	constexpr float uiSize = 0.6f;
	ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontPixelSize, nullptr, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);

	io.FontGlobalScale = fontSize; // フォントの大きさを一括で変更できます。
	style.ScaleAllSizes(uiSize); // UIの大きさを一括で変更できます。

	constexpr float iconSize = 20.0f; // 30.0fにすると大きすぎたので20.0fにしています。
	ImFontConfig config;
	config.MergeMode = true; // フォントテクスチャを合体させます。
	config.PixelSnapH = true;
	config.GlyphMinAdvanceX = iconSize; // アイコンを等幅にします。
	static constexpr ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("Data/Font/fontawesome-webfont.ttf", iconSize, &config, iconRanges);
	io.Fonts->Build();
}



void DebugUI::GameLoop()
{
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
	ID3D11RenderTargetView* RTView = StartingOver::DirectX11Manager::GetRenderTargetView();
	ID3D11DepthStencilView* DSView = StartingOver::DirectX11Manager::GetDepthStencilView();
	ImVec4 clearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	context->OMSetRenderTargets(1, &RTView, DSView);
	context->ClearRenderTargetView(RTView, reinterpret_cast<float*>(&clearColor));

	// imgui new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	DebugUpdate();

}

void DebugUI::Render()
{
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
	ID3D11RenderTargetView* RTView = StartingOver::DirectX11Manager::GetRenderTargetView();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	context->OMSetRenderTargets(1, &RTView, nullptr);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

}


void DebugUI::UnInitialize()
{
	// cleanup imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool DebugUI::WinProc(const HWND hWnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wparam, lparam)) { return true; }

	return false;
}
