#include "../Header/Player.h"

#include "imgui-docking/imgui.h"

#define skinn 1

Player::Player()
{
#if skinn

	{
		std::string anima = "model/Ferret_LOD0.fbx";

		//model = std::make_unique<SkinnedMesh>("model/animals/Ferret/Ferret_LOD1.fbx", 0);
	//	model->AddAnimations("model/NIC_export_20190304_v01.fbx", 0);

		skinnedMesh_vs = std::make_unique<StartingOver::VertexShader<SkinnedMesh::Vertex>>("Shader/SkinnedMesh_vs.cso");
		skinnedMesh_ps = std::make_unique<StartingOver::PixelShader>("Shader/SkinnedMesh_ps.cso");
		skinnedMeshShadow_vs = std::make_unique<StartingOver::VertexShader<SkinnedMesh::Vertex>>("Shader/SkinnedMeshShadowCast_vs.cso");

	}
#else 
	{

		staticMesh = std::make_unique<StaticMesh>("model/Ferret_LOD0.fbx");
		//staticMesh = std::make_unique<StaticMesh>("model/vilage.fbx");

		staticMesh_vs = std::make_unique<StartingOver::VertexShader<StaticMesh::Vertex>>("Shader/StaticMesh_vs.cso");
		staticMesh_ps = std::make_unique<StartingOver::PixelShader>("Shader/StaticMesh_ps.cso");
		staticMeshShadow_vs = std::make_unique<StartingOver::VertexShader<StaticMesh::Vertex>>("Shader/StaticMeshShadowCast_vs.cso");

	}
#endif


	/*shader = std::make_unique<Shader>();
	shader->Create("SkinnedMesh_vs.cso", "SkinnedMesh_ps.cso");*/
	position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//position = XMFLOAT4(79.56f, -19.35f + 2, 208.95f, 1.0f);

	float angle = 0;
	direction.x = sinf(angle * 0.01745f);
	direction.y = 0;
	direction.z = cosf(angle * 0.01745f);

	velocity = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

}

enum class COMMANDS { IDLE, RUN, ATTACK, TURN_LEFT, TURN_RIGHT, JUMP, DEATH };

void Player::Update()
{
	ImGui::Begin("player");
	for (auto& material : model->substance->materials)
	{
		if (ImGui::TreeNode(material.first.c_str()))
		{
			Vector4 color = material.second.properties.at(0).color;
			float outColor[4] = { color.x,color.y, color.z, color.w };
			ImGui::ColorEdit4("Color", outColor);
			color = Vector4(outColor[0], outColor[1], outColor[2], outColor[3]);
			material.second.properties.at(0).color = color;
			ImGui::TreePop();

		}
	}
	if (ImGui::Button("Save"))
	{
		model->substance->Save();
	}
	ImGui::End();
	DirectX::XMMATRIX S, R, T;
	float elapsed_time = 0.1f;
	DirectX::XMVECTOR X, Y, Z;
	Y = DirectX::XMVectorSet(0, 1, 0, 0);
	Z = DirectX::XMVector3Normalize(XMLoadFloat4(&direction));
	X = DirectX::XMVector3Cross(Y, Z);
	Y = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Z, X));
	std::deque<COMMANDS> command_queue;
	float speed = 0;
	float turning = 0;
	/*for (COMMANDS command : command_queue)
	{
		switch (command)
		{
		case COMMANDS::IDLE:
			break;
		case COMMANDS::RUN:
			speed = max_speed * 0;
			break;
		case COMMANDS::ATTACK:
			break;
		case COMMANDS::JUMP:
			break;
		case COMMANDS::TURN_LEFT:
		case COMMANDS::TURN_RIGHT:
			turning = max_turning * 0;
			break;
		case COMMANDS::DEATH:
			break;
		}
	}*/

	R = DirectX::XMMatrixRotationAxis(Y, turning * 0.01745f * elapsed_time);
	Z = XMVector4Transform(Z, R);
	XMStoreFloat4(&direction, Z);

	velocity.x = speed * direction.x;
	velocity.z = speed * direction.z;
	//velocity.y -= 9.8f * elapsed_time;
	velocity.y -= 0 * elapsed_time;

	position.x += velocity.x * elapsed_time;
	position.y += velocity.y * elapsed_time;
	position.z += velocity.z * elapsed_time;

	R = DirectX::XMMatrixIdentity();
	R.r[0] = X;
	R.r[1] = Y;
	R.r[2] = Z;
	S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	//S = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	XMStoreFloat4x4(&world_transform, S * R * T);

	Animate(StartingOver::DirectX11Manager::hrTimer.timeInterval());
}

void Player::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	//DirectX::XMMATRIX world_Matrix = GetWorldMatrix();

	
	//DirectX::XMStoreFloat4x4(&world_viewProjection, world_Matrix * view * projection);




	if (model)
	{

		skinnedMesh_vs->Activate();
		skinnedMesh_ps->Activate();
		//model->Render(w);
		skinnedMesh_vs->InActivate();
		skinnedMesh_ps->InActivate();
	}

	if(staticMesh)
	{
		staticMesh_vs->Activate();
		staticMesh_ps->Activate();
		staticMesh->Render();
		staticMesh_vs->InActivate();
		staticMesh_ps->InActivate();
	}


}

void Player::RenderShadow()
{
	world_transform = GetWorldMatrix();

	if (model)
	{

		skinnedMeshShadow_vs->Activate();
		//model->Render(world_transform, animation_clip, animation_frame);
		skinnedMeshShadow_vs->InActivate();
	}

	if (staticMesh)
	{
		staticMesh_vs->Activate();
		//staticMesh->Render(world_transform);
		staticMesh_vs->Activate();
	}
}


void Player::Animate(float elapsedTime)
{
	if (!model || model->substance->animationTakes.size() < 1)
	{
		return;
	}

	static ANIMATION_CLIP last_animation_clip;
	last_animation_clip = animation_clip;

	switch (state)
	{
	case STATE_IDLE:
		break;
	case STATE_AIM:
		break;
	}

	if (animation_clip != last_animation_clip)
	{
		animation_frame = 0;
		animation_tick = 0;
	}

	animation_clip = CLIP_AIM; //CLIP_IDLE CLIP_AIM
	//animation_frame = static_cast<size_t>(animation_tick / model->substance->animation_takes.at(animation_clip).sampling_time);
	animation_frame = static_cast<size_t>(animation_tick * model->substance->animationTakes.at(animation_clip).samplingRate);
	//animation_frame = 0;
	//animation_tick = 0;
	size_t last_of_frame = model->substance->animationTakes.at(animation_clip).keyframeSize - 1;
	if (animation_frame > last_of_frame) //end of frames
	{
		animation_frame = 0;
		animation_tick = 0;
		switch (animation_clip)
		{
		case CLIP_IDLE:
			animation_clip = CLIP_IDLE;
			state = STATE_IDLE;
			break;
		case CLIP_AIM:
			animation_clip = CLIP_AIM;
			state = STATE_AIM;
			break;
		}
	}
	animation_tick += elapsedTime * 1.0f;
}


DirectX::XMMATRIX Player::GetWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	world = DirectX::XMMatrixIdentity();

	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	Rx = DirectX::XMMatrixRotationX(angle.x);
	Ry = DirectX::XMMatrixRotationY(angle.y);
	Rz = DirectX::XMMatrixRotationZ(angle.z);
	R = Rz * Ry * Rx;

	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	world = S * R * T;

	return world;

}