#pragma once

#include <vector>
#include <string>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include "MyMath.h"
#include "Shader.h"
#include "Renderer.h"
#include "Data.h"

class Font : public Renderer
{
public:
	Font(int maxInstance);
	~Font() {}

	void Initialize(std::shared_ptr<Data> _data) override { data = _data;  }

	void Begin();
	void Render();
	void End();
	std::string GetName() { return data->GetName(); }

	Data* GetData() { return data.get(); }

	void SetPosition(Vector2 pos) { data->SetPosition(pos); }
	void SetSize(Vector2 size) { data->SetSize(size); }

private:
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> shaderResourceViews;
	Data beforeData;

	struct Vertex
	{
		Vector3 position;
		Vector2 texcoord;
		Vector4 colour;

	};

	struct CharacterInfo
	{
		static const WORD NONE  = 0;		// None Code
		static const WORD END   = 0xFFFF;	// END Code
		static const WORD EOL   = 0xFFFE;	// End of Line
		static const WORD TAB   = 0xFFFD;	// Tab Code
		static const WORD SPACE = 0xFFFC;	// Space Code

		float texTop;			// Texture top coordinate
		float texBottom;		// Texture bottom coordinate
		float texLeft;			// Texture left coordinate
		float texRight;			// Texture right coordinate
		float texWidth;			// Texture Width
		float texHeight;		// Texture Hight
		float Xoffset;			// X offset
		float Yoffset;			// Y offset
		float proportional;		// proportional width
		int   texNum;			// Texture Number
		bool  isAscii;			// Ascii characters
	};

	float                      fontWidth;
	float                      fontHeight;
	int                        texCount;
	int                        characterCount;
	std::vector<CharacterInfo> characterInfos;
	std::vector<WORD>          characterIndices;

	struct Subset
	{
		ID3D11ShaderResourceView* SRView;
		UINT startIndex;
		UINT indexCount;
	};

	std::vector<Subset> subsets;
	Vertex* currentVertex = nullptr;
	UINT currentIndexCount;
	int currentPage;

	float screenWidth;
	float screenHeight;

};