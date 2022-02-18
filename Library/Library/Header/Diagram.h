#pragma once


#include "AIGUI.h"
#include "Shader.h"
#include "Renderer.h"
#include "Data.h"
#include "MyMath.h"


class Diagram final : public Renderer
{
public:
	Diagram();
	~Diagram() = default;

	void Initialize(std::shared_ptr<Data> _data) override;;

	void Render() override;

public:

private:

	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord;
		Vector4 color;
	};
};