// mysample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

#include "../../../aee_lib_helper/aee_lib_helper.h"

// C++测试程序，调用aee_lib_helper.dll来跑通讯飞的SDK

std::wstring GetInstallExePath() {
	wchar_t file_name[MAX_PATH] = { 0 };
	DWORD size = GetModuleFileName(NULL, file_name, MAX_PATH);
	if (size == 0) {
		return L"";
	}

	std::wstring host_folder(file_name);
	auto pos = host_folder.find_last_of(L"\\");
	if (pos != std::wstring::npos) {
		host_folder = host_folder.substr(0, pos + 1);
	}

	return host_folder;
}


// 输出回调
void OnOutput(const char* ability_id, const char* key, const char* value) {

}

// 事件回调
void OnEvent(int event_type, const char* key, const char* value) {

}

// 错误回调
void OnError(int err, const char* desc) {

}

int main()
{
	SetCurrentDirectoryW(GetInstallExePath().c_str());

	std::string app_id = "fcde2f9b";
	std::string api_secret = "YTY1ZDM1OWU1OWM1YjYxNmRiNmZjYmMy";
	std::string api_key = "3813c0bf114b0221b61db7b012f40dba";

	// 初始化SDK
	AEE_lib_Init(AEE_LIB_COMMAND_WORD_RECOGNITION, app_id.c_str(), api_secret.c_str(), api_key.c_str(), OnOutput, OnEvent, OnError);

	int code = 0;

	do
	{
		// 初始命令词识别引擎
		code = AEE_lib_AIKIT_Command_Word_EngineInit();
		if (code != 0) {
			printf("AEE_lib_AIKIT_Command_Word_EngineInit fail \r\n");
			break;
		}

		// 设置命令词识别
		code = AEE_lib_AIKIT_SetCommandWordData(".\\resource\\cnenesr\\fsa\\cn_fsa.txt");
		if (code != 0) {
			printf("AEE_lib_AIKIT_SetCommandWordData fail \r\n");
			break;
		}

		printf("===========================\n");
		printf("where is audio data from? \n0: audio file\n1: microphone\n");
		printf("===========================\n");

		int aud_src = 0;
		scanf("%d", &aud_src);
		if (aud_src)
		{
			// 超时时间，这个宏表示不会超时，只要不出异常
			AEE_lib_CommandFromMicrophone(AEE_LIB_INFINITE);
		}
		else
		{
			AEE_lib_CommandFromFile(".\\testAudio\\cn_test.pcm");
		}


		// 关闭引擎
		code = AEE_lib_AIKIT_Awaken_Command_Word_EngineUnInit();
	} while (false);


	// 反初始化SDK
	AEE_lib_UnInit();

	system("pause");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
