#include "../Header/Font.h"
#include "../Header/AIGUI.h"
#include "../Header/misc.h"
#include <WICTextureLoader/WICTextureLoader11.h>
#include <stdlib.h>

Font::Font(int maxInstance)
{
	ID3D11Device* device = AIGUI::GetDevice();

	static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOUR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	/*shader = std::make_unique<Shader>();
	shader->Create("Font_vs.cso", "Font_ps.cso", const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), _countof(inputElementDesc));*/
	D3D11_BUFFER_DESC VBDesc;
	ZeroMemory(&VBDesc, sizeof(VBDesc));
	VBDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * maxInstance * 4);
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = 0;

	device->CreateBuffer(&VBDesc, nullptr, vertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC PBDesc = {};
	ZeroMemory(&PBDesc, sizeof(PBDesc));


	std::unique_ptr<UINT[]> indices = std::make_unique<UINT[]>(maxInstance * 6);

	UINT* p = indices.get();
	for (int i = 0; i < maxInstance * 4; i += 4)
	{
		p[0] = i + 0;
		p[1] = i + 1;
		p[2] = i + 2;
		p[3] = i + 2;
		p[4] = i + 1;
		p[5] = i + 3;
		p += 6;
	}
	
	PBDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * maxInstance * 6);
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	PBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	PBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	PBDesc.CPUAccessFlags = 0;
	PBDesc.MiscFlags = 0;
	PBDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sub;
	ZeroMemory(&sub, sizeof(sub));
	sub.pSysMem = indices.get();
	sub.SysMemPitch = 0; //Not use for index buffers.
	sub.SysMemSlicePitch = 0; //Not use for index buffers.
	HRESULT hr = device->CreateBuffer(&PBDesc, &sub, indexBuffer.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = FALSE;
	/*depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;*/
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	{
		std::string filename = "Data/Font/MS Gothic.fnt";
		FILE* fp = nullptr;
		fopen_s(&fp, filename.c_str(), "rb");
		_ASSERT_EXPR_A(fp, "FNT File not found");

		fseek(fp, 0, SEEK_END);
		long fntSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::unique_ptr<char[]> fntData = std::make_unique<char[]>(fntSize);
		fread(fntData.get(), fntSize, 1, fp);
		fclose(fp);

		// (***.fnt)ヘッダ解析
		char* context = nullptr;	// 内部的に使用するので深く考えない
		const char* delimiter = " ,=\"\r\n";
		const char* delimiter2 = ",=\"\r\n";
		char* pToken = strtok_s(fntData.get(), delimiter, &context);
		_ASSERT_EXPR_A(strcmp(pToken, "info") == 0, "FNT Format");

		// face
		strtok_s(nullptr, delimiter, &context);
		const char* face = strtok_s(nullptr, delimiter2, &context);
		// size
		strtok_s(nullptr, delimiter, &context);
		int size = atoi(strtok_s(nullptr, delimiter, &context));
		// bold
		strtok_s(nullptr, delimiter, &context);
		int bold = atoi(strtok_s(nullptr, delimiter, &context));
		// italic
		strtok_s(nullptr, delimiter, &context);
		int italic = atoi(strtok_s(nullptr, delimiter, &context));
		// charset
		strtok_s(nullptr, delimiter, &context);
		const char* charset = strtok_s(nullptr, delimiter, &context);
		// unicode
		if (strcmp(charset, "unicode") == 0)
			charset = "";
		else
			strtok_s(nullptr, delimiter, &context);
		int unicode = atoi(strtok_s(nullptr, delimiter, &context));
		// stretchH
		strtok_s(nullptr, delimiter, &context);
		int stretchH = atoi(strtok_s(nullptr, delimiter, &context));
		// smooth
		strtok_s(nullptr, delimiter, &context);
		int smooth = atoi(strtok_s(nullptr, delimiter, &context));
		// aa
		strtok_s(nullptr, delimiter, &context);
		int aa = atoi(strtok_s(nullptr, delimiter, &context));
		// padding
		strtok_s(nullptr, delimiter, &context);
		int padding1 = atoi(strtok_s(nullptr, delimiter, &context));
		int padding2 = atoi(strtok_s(nullptr, delimiter, &context));
		int padding3 = atoi(strtok_s(nullptr, delimiter, &context));
		int padding4 = atoi(strtok_s(nullptr, delimiter, &context));
		// spacing
		strtok_s(nullptr, delimiter, &context);
		int spacing1 = atoi(strtok_s(nullptr, delimiter, &context));
		int spacing2 = atoi(strtok_s(nullptr, delimiter, &context));
		// outline
		strtok_s(nullptr, delimiter, &context);
		int outline = atoi(strtok_s(nullptr, delimiter, &context));

		// common
		strtok_s(nullptr, delimiter, &context);
		// lineHeight
		strtok_s(nullptr, delimiter, &context);
		int lineHeight = atoi(strtok_s(nullptr, delimiter, &context));
		// base
		strtok_s(nullptr, delimiter, &context);
		int base = atoi(strtok_s(nullptr, delimiter, &context));
		// scaleW
		strtok_s(nullptr, delimiter, &context);
		int scaleW = atoi(strtok_s(nullptr, delimiter, &context));
		// scaleH
		strtok_s(nullptr, delimiter, &context);
		int scaleH = atoi(strtok_s(nullptr, delimiter, &context));
		// pages
		strtok_s(nullptr, delimiter, &context);
		int pages = atoi(strtok_s(nullptr, delimiter, &context));
		// packed
		strtok_s(nullptr, delimiter, &context);
		int packed = atoi(strtok_s(nullptr, delimiter, &context));
		// alphaChnl
		strtok_s(nullptr, delimiter, &context);
		int alphaChnl = atoi(strtok_s(nullptr, delimiter, &context));
		// redChnl
		strtok_s(nullptr, delimiter, &context);
		int redChnl = atoi(strtok_s(nullptr, delimiter, &context));
		// greenChnl
		strtok_s(nullptr, delimiter, &context);
		int greenChnl = atoi(strtok_s(nullptr, delimiter, &context));
		// blueChnl
		strtok_s(nullptr, delimiter, &context);
		int blueChnl = atoi(strtok_s(nullptr, delimiter, &context));

		// ディレクトリパス取得
		char dirname[256];
	
		_splitpath_s(filename.c_str(), nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);
		shaderResourceViews.resize(pages);
		for (int i = 0; i < pages; i++)
		{
			// page
			strtok_s(nullptr, delimiter, &context);
			// id
			strtok_s(nullptr, delimiter, &context);
			int id = ::atoi(::strtok_s(nullptr, delimiter, &context));
			// file
			strtok_s(nullptr, delimiter, &context);
			const char* file = ::strtok_s(nullptr, delimiter2, &context);

			// 相対パスの解決
			char fname[256];
			_makepath_s(fname, 256, nullptr, dirname, file, nullptr);

			// マルチバイト文字からワイド文字へ変換
			size_t length;
			wchar_t wfname[256];
			mbstowcs_s(&length, wfname, 256, fname, _TRUNCATE);

			// テクスチャ読み込み
			Microsoft::WRL::ComPtr<ID3D11Resource> resource;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfname, resource.GetAddressOf(), shaderResourceViews.at(i).GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		// chars
		strtok_s(nullptr, delimiter, &context);
		// count
		strtok_s(nullptr, delimiter, &context);
		int count = atoi(strtok_s(nullptr, delimiter, &context));

		// データ格納
		fontWidth = static_cast<float>(size);
		fontHeight = static_cast<float>(lineHeight);
		texCount = pages;
		characterCount = count + 1;
		characterInfos.resize(characterCount);
		characterIndices.resize(0xFFFF);
		memset(characterIndices.data(), 0, sizeof(WORD) * characterIndices.size());

		characterIndices.at(0x00) = CharacterInfo::END;
		characterIndices.at(0x0a) = CharacterInfo::EOL;
		characterIndices.at(0x09) = CharacterInfo::TAB;
		characterIndices.at(0x20) = CharacterInfo::SPACE;

		// 文字情報解析
		int n = 1;
		for (int i = 0; i < count; i++)
		{
			// char
			strtok_s(nullptr, delimiter, &context);
			// id
			strtok_s(nullptr, delimiter, &context);
			int id = atoi(strtok_s(nullptr, delimiter, &context));
			// x
			strtok_s(nullptr, delimiter, &context);
			int x = atoi(strtok_s(nullptr, delimiter, &context));
			// y
			strtok_s(nullptr, delimiter, &context);
			int y = atoi(strtok_s(nullptr, delimiter, &context));
			// width
			strtok_s(nullptr, delimiter, &context);
			int width = atoi(strtok_s(nullptr, delimiter, &context));
			// height
			strtok_s(nullptr, delimiter, &context);
			int height = atoi(strtok_s(nullptr, delimiter, &context));
			// xoffset
			strtok_s(nullptr, delimiter, &context);
			int xoffset = atoi(strtok_s(nullptr, delimiter, &context));
			// yoffset
			strtok_s(nullptr, delimiter, &context);
			int yoffset = atoi(strtok_s(nullptr, delimiter, &context));
			// xadvance
			strtok_s(nullptr, delimiter, &context);
			int xadvance = atoi(strtok_s(nullptr, delimiter, &context));
			// page
			strtok_s(nullptr, delimiter, &context);
			int texNum = atoi(strtok_s(nullptr, delimiter, &context));
			// chnl
			strtok_s(nullptr, delimiter, &context);
			int chnl = atoi(strtok_s(nullptr, delimiter, &context));

			// 文字コード格納
			if (id >= 0x10000) continue;

			CharacterInfo& info = characterInfos.at(n);

			characterIndices.at(id) = static_cast<WORD>(i + 1);

			// 文字情報格納
			info.texLeft = static_cast<float>(x) / static_cast<float>(scaleW);
			info.texTop = static_cast<float>(y) / static_cast<float>(scaleH);
			info.texRight = static_cast<float>(x + width) / static_cast<float>(scaleW);
			info.texBottom = static_cast<float>(y + height) / static_cast<float>(scaleH);
			info.Xoffset = static_cast<float>(xoffset);
			info.Yoffset = static_cast<float>(yoffset);
			info.proportional = static_cast<float>(xadvance);
			info.texWidth = static_cast<float>(width);
			info.texHeight = static_cast<float>(height);
			info.texNum = texNum;
			info.isAscii = (id < 0x100);

			n++;
		}
	}

}


void Font::Begin()
{
	

}

void Font::Render()
{
	beforeData = *data;

	if (data->GetName().empty())
	{
		return ;
	}

	ID3D11DeviceContext* context = AIGUI::GetDeviceContext();

	StartingOver::SetBlendMode(StartingOver::Blender::BS_ALPHA);


	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	context->RSGetViewports(&numViewports, &viewport);
	screenWidth = viewport.Width;
	screenHeight = viewport.Height;
	//shader->Activate();

	D3D11_MAPPED_SUBRESOURCE mappedSub;
	context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSub);

	currentVertex = reinterpret_cast<Vertex*>(mappedSub.pData);
	currentIndexCount = 0;
	currentPage = -1;
	subsets.clear();


	size_t size = data->GetName().size();

	Vector2 startPos = beforeData.GetPosition();
	float space = fontWidth;

	for (size_t i = 0; i < size; ++i)
	{
		WORD word = static_cast<WORD>(data->GetName().at(i));
		WORD code = characterIndices.at(word);

		// 特殊制御用コード
		if (code == CharacterInfo::END)
		{
			break;
		}
		else if (code == CharacterInfo::EOL)
		{
			beforeData.SetPosition(Vector2(startPos.x, beforeData.GetPosition().y+fontHeight));
			continue;
		}
		else if (code == CharacterInfo::TAB)
		{
			beforeData.SetPosition(Vector2(startPos.x + space * 4, beforeData.GetPosition().y));
			continue;
		}
		else if (code == CharacterInfo::SPACE)
		{
			beforeData.SetPosition(Vector2(startPos.x + space, beforeData.GetPosition().y));

			continue;
		}

		const CharacterInfo& info = characterInfos.at(code);

		Vector2 pos = beforeData.GetPosition() +Vector2(info.Xoffset, info.Yoffset);
		Vector4 colour = beforeData.GetColor();
		currentVertex[0].position.x = pos.x;
		currentVertex[0].position.y = pos.y;
		currentVertex[0].position.z = 0.0f;
		currentVertex[0].texcoord.x = info.texLeft;
		currentVertex[0].texcoord.y = info.texTop;
		currentVertex[0].colour = colour;
		currentVertex[0].colour.w = 1.0f;

		currentVertex[1].position.x = pos.x + data->GetSize().x;
		currentVertex[1].position.y = pos.y;
		currentVertex[1].position.z = 0.0f;
		currentVertex[1].texcoord.x = info.texRight;
		currentVertex[1].texcoord.y = info.texTop;
		currentVertex[1].colour = colour;
		currentVertex[1].colour.w = 1.0f;

		currentVertex[2].position.x = pos.x;
		currentVertex[2].position.y = pos.y + data->GetSize().y;
		currentVertex[2].position.z = 0.0f;
		currentVertex[2].texcoord.x = info.texLeft;
		currentVertex[2].texcoord.y = info.texBottom;
		currentVertex[2].colour = colour;
		currentVertex[2].colour.w = 1.0f;

		currentVertex[3].position.x = pos.x + data->GetSize().x;
		currentVertex[3].position.y = pos.y + data->GetSize().y;
		currentVertex[3].position.z = 0.0f;
		currentVertex[3].texcoord.x = info.texRight;
		currentVertex[3].texcoord.y = info.texBottom;
		currentVertex[3].colour = colour;
		currentVertex[3].colour.w = 1.0f;

		// NDC座標変換
		for (int j = 0; j < 4; ++j)
		{
			currentVertex[j].position.x = 2.0f * currentVertex[j].position.x / screenWidth - 1.0f;
			currentVertex[j].position.y = 1.0f - 2.0f * currentVertex[j].position.y / screenHeight;
		}
		currentVertex += 4;

		beforeData.SetPosition(
			Vector2(
				beforeData.GetPosition().x + data->GetSize().x/* + info.proportional*/, beforeData.GetPosition().y));

		// テクスチャが切り替わる度に描画する情報を設定
		if (currentPage != info.texNum)
		{
			currentPage = info.texNum;

			Subset subset;
			subset.SRView = shaderResourceViews.at(info.texNum).Get();
			subset.startIndex = currentIndexCount;
			subset.indexCount = 0;
			subsets.emplace_back(subset);
		}
		currentIndexCount += 6;
	}

	context->Unmap(vertexBuffer.Get(), 0);

	size_t subsetSize = subsets.size();

	for (size_t i = 1; i < subsetSize; ++i)
	{
		Subset& prev = subsets.at(i - 1);
		Subset& next = subsets.at(i);
		prev.indexCount = next.startIndex - prev.startIndex;
	}
	Subset& last = subsets.back();
	last.indexCount = currentIndexCount - last.startIndex;


	context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	context->RSSetState(rasterizerState.Get());
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	// 頂点バッファ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	// 描画
	for (size_t i = 0; i < subsetSize; ++i)
	{
		Subset& subset = subsets.at(i);

		context->PSSetShaderResources(0, 1, &subset.SRView);
		context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
	}
	StartingOver::SetBlendMode(StartingOver::Blender::BS_NONE);

}

void Font::End()
{
	ID3D11DeviceContext* context = AIGUI::GetDeviceContext();
	
	
	
}