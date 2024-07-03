// mysample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "../../../aee_lib_helper/aee_lib_helper.h"

// C++���Գ��򣬵���aee_lib_helper.dll����ͨѶ�ɵ�SDK

static const char* ABILITY = "e867a88f2";


// ����ص�
void OnOutput(const char* ability_id, const char* key, const char* value) {

}

// �¼��ص�
void OnEvent(int event_type, const char* key, const char* value) {

}

// ����ص�
void OnError(int err, const char* desc) {

}

int main()
{
	std::string app_id = "fcde2f9b";
	std::string api_secret = "YTY1ZDM1OWU1OWM1YjYxNmRiNmZjYmMy";
	std::string api_key = "3813c0bf114b0221b61db7b012f40dba";

	// ��ʼ��SDK
	AEE_lib_Init(app_id.c_str(), api_secret.c_str(), api_key.c_str(), OnOutput, OnEvent, OnError);

	int code = 0;
	do
	{
		//// ��ʼ��������������
		//code = AEE_lib_AIKIT_EngineInit(ABILITY, ".\\resource\\ivw70\\kerword.txt");
		//if (code != 0) {
		//	printf("AEE_lib_AIKIT_EngineInit fail \r\n");
		//	break;
		//}

		//printf("===========================\n");
		//printf("where is audio data from? \n0: audio file\n1: microphone\n");
		//printf("===========================\n");

		//int aud_src = 0;
		//scanf("%d", &aud_src);
		//if (aud_src)
		//{
		//	AEE_lib_WakeFromMicrophone();
		//}
		//else
		//{
		//	AEE_lib_WakeFromFile(".\\testAudio\\xbxb.pcm");
		//}


		//// �ر�����
		//code = AEE_lib_AIKIT_EngineUnInit(ABILITY);
	} while (false);


	// ����ʼ��SDK
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
