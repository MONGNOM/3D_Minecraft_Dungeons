#include "DataManager.h"
#include <fstream>
#include "GameInstance.h"

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Save_Date()
{
	// 파일 데이터를 저장
	json myData;

	myData["name"] = "If this shows up, let's go to bed.";
	myData["age"] = 30;
	myData["key"] = "1234";

	std::ofstream out("../Bin/DataFiles/Test_Save.json");
	out << myData.dump(4);
	

	return S_OK;
}



HRESULT CDataManager::Load_Date(const _tchar* filePath)
{
	// 파일 주소에서 데이터를 읽어와서 적용
	
	ifstream in(filePath);

	if (!in.is_open())
		return E_FAIL;

	json myData; 
	in >> myData;

	cout << "Name: " << myData["name"] << endl;

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
