#include "DataManager.h"
#include <fstream>
#include "GameInstance.h"
#include "GameObject.h"

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Save_Date(const vector<tagObjectInfo>& objectinfo) // 여기에 그럼 info가 아니라 list그냥 받아와서
{
	// 반복문으로 리스트에  그게 맞지않나
	json myData;

	myData["Level"] = "Tool_Scene";
	myData["Count"] = objectinfo.size();

	for (auto& iter : objectinfo)
	{
		json object;

		object["type"] = iter.type;
		object["name"] = iter.Name;
		object["PrototypeName"] = iter.PrototypeName;
		object["Scale"] = { iter.Scale.x, iter.Scale.y, iter.Scale.z };
		object["Rotation"] = { iter.Rotation.x, iter.Rotation.y, iter.Rotation.z };
		object["Translation"] = { iter.Translation.x, iter.Translation.y, iter.Translation.z };
		object["data"] =  iter.data ;

		myData["GameObject"].push_back(object);
	}

	

	std::ofstream out("../Test_Save.json");

	out << myData.dump(4);

	out.close();

	return S_OK;
}



HRESULT CDataManager::Load_Date(const _tchar* filePath, vector<tagObjectInfo>& objectinfo)
{
	// 파일 주소에서 데이터를 읽어와서 적용
	
	ifstream in(filePath);

	if (!in.is_open())
		return E_FAIL;

	json myData; 
	in >> myData;

	for (auto& iter : myData["GameObject"])
	{
		OBJECTINFO object{};
		object.Name = iter["name"].get<string>();
		object.Scale = { iter["Scale"][0], iter["Scale"][1], iter["Scale"][2] };
		object.Rotation = { iter["Rotation"][0], iter["Rotation"][1], iter["Rotation"][2] };
		object.Translation = { iter["Translation"][0], iter["Translation"][1], iter["Translation"][2] };
		object.type = iter["type"];
		object.PrototypeName = iter["PrototypeName"].get<string>();
		object.data = iter["data"];

		objectinfo.push_back(object);
	}

	in.close();

	
	return S_OK;
}

CDataManager* CDataManager::Create()
{
	return new CDataManager;
}

void CDataManager::Free()
{
	__super::Free();
}
