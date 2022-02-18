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
/// TagListUI�̏���������
/// Data Set
/// ������ Component
/// ������ Search Text Here
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
/// TagListUI�̍X�V����
/// ��������
/// </summary>
/// <returns> bool isSearch ������Ԃ̔��� </returns>
void ComponentTagListTab::Update()
{

	SearchTag();

	componentTagList->Update(searchTextData->GetName());

}

/// <summary>
/// TagListUI�̕`�揈��
/// </summary>
void ComponentTagListTab::Render()
{
	base->Render();

	// ������ > Component�̕`��
	component->Render();

	// ������ > ���͕��� or ��^��
	// �������A���͂��������A�������ĂȂ����A��^��
	searchText->Render();

	componentTagList->Render();

}

/// <summary>
/// ���͂��������ƍ������O������ComponentTag��
/// ComponentTagList��componentTags�̒��ɂ��邩������
/// </summary>
/// <param name="tags"> ComponentTagList��componentTags </param>
/// <returns></returns>
void ComponentTagListTab::SearchTag()
{
	// ���N���b�N
	if (Input::GetMouseButtonDown(0))
	{
		// �e�L�X�g�{�b�N�X���N���b�N
		if (Input::GetMousePos().x >= searchTextData->GetPosition().x && Input::GetMousePos().x <= searchTextData->GetPosition().x + 185 &&
			Input::GetMousePos().y >= searchTextData->GetPosition().y && Input::GetMousePos().y <= searchTextData->GetPosition().y + searchTextData->GetSize().y)
		{
			searchTextData->SetName("|");
			searchTextData->SetSize(Vector2(13, 25.0f));
			isSearch = true;
		}
		else
		{
			//�@�������͂���Ă��Ȃ��Ƃ����ɖ߂�
			searchTextData->GetName().clear();
			if (searchTextData->GetName() == "|")
			{
				searchTextData->SetName("Search Text Here");
				isSearch = false;

			}
		}
	}
	// ����box���N���b�N���Ă邩
	if (isSearch)
	{
		std::string input = Input::GetALLKeyDown();
		// BackSpace��������Ă��Ȃ���
		if (input != "BackSpace")
		{
			// �܂�����������ĂȂ������Ƃ�"|"������
			if (searchTextData->GetName() == "|")
			{
				searchTextData->popBack();

			}

			// ������͂����������Z�b�g
			searchTextData->SetName(searchTextData->GetName() + Input::GetALLKeyDown());
		}
		else
		{
			// BackSpace���������Ƃ�
			// ���������͂���Ă���������
			if (!searchTextData->GetName().empty())
			{
				searchTextData->popBack();
			}

		}

		// �������͂���Ă��Ȃ������Ƃ��ɏ�����Ă��܂��Ă���̂�
		// ���߂�"|"�����
		if (searchTextData->GetName().empty())
		{
			searchTextData->SetName("|");
		}
	}
}
