
#include "../Header/DataBase.h"
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <fstream>

/// <summary>
/// Constructer
/// </summary>
DataBase::DataBase()
{
	UIDataList.clear();
	nodeDataLists.clear();
}

/// <summary>
/// DataBase‚Ì‰Šú‰»ˆ—
/// serialize‚µ‚½Data‚ğ“Ç‚İ‚Ş
/// Œ»İ‚ÍŒˆ‚ß‘Å‚¿
/// </summary>
void DataBase::Initialize()
{
	std::shared_ptr<Data> data;
	std::string str;

#if 1
	std::string name = "Base";
	std::vector<std::shared_ptr<Data>> dataList;
	//data = std::make_shared<Data>();
	//data->SetPosition(Vector2(0.0f, 75.0f));
	//data->SetSize(Vector2(300, 1080));
	//data->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	//dataList.push_back(data);

	data = std::make_shared<Data>();
	data->SetPosition(Vector2(303.0f, 75.0f));
	data->SetSize(Vector2(1315, 75));
	data->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	dataList.push_back(data);

	data = std::make_shared<Data>();

	data->SetPosition(Vector2(303.0f, 153.0f));
	data->SetSize(Vector2(1315, 75));
	data->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	dataList.push_back(data);

	data = std::make_shared<Data>();
	data->SetPosition(Vector2(303.0f, 230.0f));
	data->SetSize(Vector2(1315, 850));
	data->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	dataList.push_back(data);

	data = std::make_shared<Data>();
	data->SetPosition(Vector2(1620.0f, 75.0f));
	data->SetSize(Vector2(300, 1080));
	data->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	dataList.push_back(data);

	UIDataList.insert(std::make_pair(name, dataList));
	dataList.clear();

	
	const std::string n = "Components";

	data = std::make_shared<Data>();
	data->SetPosition(Vector2(15.0, 80));
	data->SetSize(Vector2(100 / n.size(), 20));
	data->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));
	dataList.push_back(data);
	UIDataList.insert(std::make_pair("Contents", dataList));

	//{ // save
	//	std::string name = "Default.json";
	//	std::ofstream os(name, std::ios::out);
	//	cereal::JSONOutputArchive archiveSave(os);

	//	archiveSave(cereal::make_nvp("UI", UIDataList));



	//}
#elif 1

	{ // load
		std::string name = "Default.json";
		std::ifstream is(name, std::ios::in);
		cereal::JSONInputArchive archiveLoad(is);
		archiveLoad(cereal::make_nvp("UI", UIDataList));

	}

#endif
}

/// <summary>
/// UIDataList‚ÉData‚ğ•Û‘¶
/// </summary>
/// <param name="_search"> UIDataList‚ÌKey </param>
/// <param name="_data"> UIDataList‚É•Û‘¶‚·‚éData </param>
void DataBase::PushBackData(std::string _search, Data _data)
{
	std::shared_ptr<Data> pData = std::make_shared<Data>();
	pData.reset(&_data);

	if (UIDataList.at(_search).empty())
	{
		UIDataList.at(_search).push_back(pData);
	}
	else
	{
		std::vector<std::shared_ptr<Data>> pvData;
		pvData.push_back(pData);
		UIDataList.insert(std::make_pair(_search, pvData));
	}
}

/// <summary>
/// UIDataList‚ÉData‚ğ•Û‘¶
/// </summary>
/// <param name="_search"> UIDataList‚ÌKey </param>
/// <param name="_data"> UIDataList‚É•Û‘¶‚·‚éData </param>
void DataBase::PushBackData(std::string _search, Data* _data)
{
	std::shared_ptr<Data> pData = std::make_shared<Data>();
	pData.reset(_data);

	if (UIDataList.at(_search).empty())
	{
		UIDataList.at(_search).push_back(pData);
	}
	else
	{
		std::vector<std::shared_ptr<Data>> pvData;
		pvData.push_back(pData);
		UIDataList.insert(std::make_pair(_search, pvData));
	}
}

/// <summary>
/// UIDataList‚ÉData‚ğ•Û‘¶
/// </summary>
/// <param name="_search"> UIDataList‚ÌKey </param>
/// <param name="_data"> UIDataList‚É•Û‘¶‚·‚éData </param>
void DataBase::PushBackData(std::string _search, std::shared_ptr<Data> _data)
{

	if (UIDataList.count(_search))
	{
		UIDataList.at(_search).push_back(_data);
	}
	else
	{
		std::vector<std::shared_ptr<Data>> pvData;
		pvData.push_back(_data);
		UIDataList.insert(std::make_pair(_search, pvData));
	}
}

/// <summary>
/// UIDataList‚ÉData‚ğ•Û‘¶
/// </summary>
/// <param name="_search"> UIDataList‚ÌKey </param>
/// <param name="_data"> UIDataList‚É•Û‘¶‚·‚éData </param>
void DataBase::PushBackDatas(std::string _search, std::vector<std::shared_ptr<Data>> _datas)
{
	std::vector<std::shared_ptr<Data>> pDatas;
	for (auto data : _datas)
	{
		pDatas.push_back(data);
	}


	if (UIDataList.at(_search).empty())
	{
		UIDataList.insert(std::make_pair(_search, pDatas));
	}
	else
	{

		for (auto data : pDatas)
		{
			UIDataList.at(_search).push_back(data);
		}
	}
}