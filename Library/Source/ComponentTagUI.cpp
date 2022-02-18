#include "../Header/Base.h"
#include "../Header/ComponentTagUI.h"
#include "../Header/Input.h"
#include "../Header/AIGUI.h"
#include "../Header/Font.h"
#include <memory>


/// <summary>
/// Constructer
/// </summary>
ComponentTagListTab::ComponentTagListTab()
{
	tabName = "ComponentTagListTab";
	myData = std::make_shared<Data>();
	base = std::make_unique<Base>();
	/*component = std::make_unique<Font>(26);
	searchText = std::make_unique<Font>(26);*/
	componentData = std::make_shared<Data>();
	searchTextData = std::make_shared<Data>();
	componentTagList = std::make_unique<ComponentTagList>();

}

/// <summary>
/// TagListUIの初期化処理
/// Data Set
/// 文字列 Component
/// 文字列 Search Text Here
/// </summary>
void ComponentTagListTab::Initialize()
{
	myData->SetPosition(Vector2(0.0f, 50.0f));
	myData->SetSize(Vector2(350, 1080));
	myData->SetColor(Vector4(0.8f, 0.8f, 0.8f, 1.0f));
	myData->SetName(tabName);
	base->Initialize(myData);

	componentData->SetPosition(Vector2(10.0f, 60.0f));
	componentData->SetSize(Vector2(11, 20.0f));
	componentData->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	componentData->SetName("Component");

	searchTextData->SetPosition(Vector2(155.0f, 60.0f));
	searchTextData->SetSize(Vector2(11.0f, 20.0f));
	searchTextData->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
	searchTextData->SetName("Search Text Here");

	component->Initialize(componentData);
	searchText->Initialize(searchTextData);

	componentTagList->Initialize();
	componentTagList->CreateComponentTag("test", "Test");
	componentTagList->CreateComponentTag("comp", "Test");
	componentTagList->CreateComponentTag("asdas", "Test");
	componentTagList->CreateComponentTag("asdfaaaaaaaaaaaa", "aa");
	componentTagList->CreateComponentTag("1234567890123456789012534567890", "math");
	componentTagList->CreateComponentTag("f", "Test");

	isSearch = false;
}

/// <summary>
/// TagListUIの更新処理
/// 検索処理
/// </summary>
/// <returns> bool isSearch 検索状態の判別 </returns>
void ComponentTagListTab::Update()
{

	SearchTag();

	componentTagList->Update(searchTextData->GetName());

}

/// <summary>
/// TagListUIの描画処理
/// </summary>
void ComponentTagListTab::Render()
{
	base->Render();

	// 文字列 > Componentの描画
	component->Render();

	// 文字列 > 入力文字 or 定型文
	// 検索時、入力した文字、検索してない時、定型文
	searchText->Render();

	componentTagList->Render();

}

/// <summary>
/// 入力した文字と合う名前を持つComponentTagが
/// ComponentTagListのcomponentTagsの中にあるかを検索
/// </summary>
/// <param name="tags"> ComponentTagListのcomponentTags </param>
/// <returns></returns>
void ComponentTagListTab::SearchTag()
{
	// 左クリック
	if (Input::GetMouseButtonDown(0))
	{
		// テキストボックスをクリック
		if (Input::GetMousePos().x >= searchTextData->GetPosition().x && Input::GetMousePos().x <= searchTextData->GetPosition().x + 185 &&
			Input::GetMousePos().y >= searchTextData->GetPosition().y && Input::GetMousePos().y <= searchTextData->GetPosition().y + searchTextData->GetSize().y)
		{
			searchTextData->SetName("|");
			searchTextData->SetSize(Vector2(13, 25.0f));
			isSearch = true;
		}
		else
		{
			//　何も入力されていないとき元に戻す
			searchTextData->GetName().clear();
			if (searchTextData->GetName() == "|")
			{
				searchTextData->SetName("Search Text Here");
				isSearch = false;

			}
		}
	}
	// 検索boxをクリックしてるか
	if (isSearch)
	{
		std::string input = Input::GetALLKeyDown();
		// BackSpaceが押されていないか
		if (input != "BackSpace")
		{
			// まだ何も押されてなかったとき"|"を消す
			if (searchTextData->GetName() == "|")
			{
				searchTextData->popBack();

			}

			// 今回入力した文字をセット
			searchTextData->SetName(searchTextData->GetName() + Input::GetALLKeyDown());
		}
		else
		{
			// BackSpaceを押したとき
			// 文字が入力されていた時消す
			if (!searchTextData->GetName().empty())
			{
				searchTextData->popBack();
			}

		}

		// 何も入力されていなかったときに消されてしまっているので
		// 改めて"|"を入力
		if (searchTextData->GetName().empty())
		{
			searchTextData->SetName("|");
		}
	}
}
