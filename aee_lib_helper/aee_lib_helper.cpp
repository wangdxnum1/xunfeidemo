#include "pch.h"
#include "aee_lib_helper.h"

#include "stdlib.h"
#include "stdio.h"
#include <fstream>
#include <assert.h>
#include <cstring>
#include <atomic>
// #include <unistd.h>

#include <Windows.h>
#include <mmsystem.h>

#include "../include/aikit_biz_api.h"
#include "../include/aikit_constant.h"
#include "../include/aikit_biz_config.h"
#include "../include/aikit_biz_builder.h"

#pragma comment(lib, "winmm.lib")  

#define FRAME_LEN	640 //16k�����ʵ�16bit��Ƶ��һ֡�Ĵ�СΪ640B, ʱ��20ms

using namespace std;
using namespace AIKIT;

static const char* ABILITY = "e867a88f2";

static AEE_lib_AIKIT_OnOutput OnOutputDelegate = nullptr;
static AEE_lib_AIKIT_OnEvent OnEventDelegate = nullptr;
static AEE_lib_AIKIT_OnError OnErrorDelegate = nullptr;

//
// call back
//
void OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output) {

	//system("chcp 65001");
	printf("OnOutput abilityID :%s\n", handle->abilityID);
	printf("OnOutput key:%s\n", output->node->key);
	printf("OnOutput value:%s\n", (char*)output->node->value);

	if (OnOutputDelegate) {
		OnOutputDelegate(handle->abilityID, output->node->key, (char*)output->node->value);
	}
}

void OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue) {
	printf("OnEvent:%d\n", eventType);

	if (OnEventDelegate) {
		OnEventDelegate((int)eventType, eventValue->node->key, (char*)eventValue->node->value);
	}
}

void OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc) {
	printf("OnError:%d\n", err);

	if (OnErrorDelegate) {
		OnErrorDelegate(err, desc);
	}
}

// This is an example of an exported function.
AEE_LIB_HELPER_API int AEE_lib_Init(
	const char* app_id,
	const char* api_secret,
	const char* api_key,
	AEE_lib_AIKIT_OnOutput on_output,
	AEE_lib_AIKIT_OnEvent on_event,
	AEE_lib_AIKIT_OnError on_error)
{
	printf("app_id : %s, api_secret : %s, api_key : %s \r\n", app_id, api_secret, api_key);

	AIKIT_Configurator::builder()
		.app()
		.appID(app_id)
		.apiSecret(api_secret)
		.apiKey(api_key)
		.workDir(".\\")
		.auth()
		.authType(0)
		.ability(ABILITY)
		.log()
		.logMode(2)
		.logPath(".\\aikit.log");
	int ret = AIKIT_Init();
	if (ret != 0) {
		printf("AIKIT_Init failed:%d\n", ret);
		return ret;
	}

	AIKIT_Callbacks cbs = { OnOutput,OnEvent,OnError };
	ret = AIKIT_RegisterAbilityCallback(ABILITY, cbs);
	if (ret != 0) {
		printf("AIKIT_RegisterAbilityCallback failed:%d\n", ret);
		return ret;
	}

	//
	// set callback
	//
	OnOutputDelegate = on_output;
	OnEventDelegate = on_event;
	OnErrorDelegate = on_error;

	system("chcp 65001");

	return 0;
}

AEE_LIB_HELPER_API int AEE_lib_UnInit() {
	return AIKIT_UnInit();
}

AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineInit(const char* ability, const char* key_word_file_path) {
	AIKIT_OutputData* output = nullptr;
	int ret = 0;
	int aud_src = 0;
	int index[] = { 0 };

	ret = AIKIT_EngineInit(ABILITY, nullptr);
	if (ret != 0) {
		printf("AIKIT_EngineInit failed:%d\n", ret);
		return ret;

	}

	//���û��Ѵ�
	AIKIT_CustomData customData;
	customData.key = "key_word";
	customData.index = 0;
	customData.from = AIKIT_DATA_PTR_PATH;
	// .\\resource\\ivw70\\xbxb.txt
	customData.value = (void*)key_word_file_path;
	customData.len = (int32_t)strlen(key_word_file_path);
	customData.next = nullptr;
	customData.reserved = nullptr;
	printf("AIKIT_LoadData start!\n");
	ret = AIKIT_LoadData(ABILITY, &customData);
	printf("AIKIT_LoadData end!\n");
	printf("AIKIT_LoadData:%d\n", ret);

	// �������ʧ�ܣ�����������Ҫ����ʼ����
	if (ret != 0) {
		AIKIT_EngineUnInit(ability);
	}

	return ret;
}

AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineUnInit(const char* ability) {
	return AIKIT_EngineUnInit(ability);
}

AEE_LIB_HELPER_API int AEE_lib_WakeFromFile(const char* file_path) {
	int ret = 0;
	AIKIT_DataBuilder* dataBuilder = nullptr;
	AIKIT_ParamBuilder* paramBuilder = nullptr;
	AIKIT_HANDLE* handle = nullptr;
	AiAudio* aiAudio_raw = nullptr;
	FILE* file = nullptr;
	char data[320] = { 0 };
	size_t fileSize = 0;
	size_t readLen = 0;
	int index[] = { 0 };

	// ָ��Ҫʹ�õĻ��Ѵ��ļ����ݼ��ϣ�ÿ�λỰ����ǰ��Ҫ���á�
	ret = AIKIT_SpecifyDataSet(ABILITY, "key_word", index, 1);
	printf("AIKIT_SpecifyDataSet:%d\n", ret);
	if (ret != 0) {
		printf("AIKIT_SpecifyDataSet failed \n");
		goto  exit;
	}

	// ͨ�� start ���������Ự�����뵱ǰ��������Ҫָ���Ĳ�����
	paramBuilder = AIKIT_ParamBuilder::create();
	paramBuilder->param("wdec_param_nCmThreshold", "0 0:999", (uint32_t)strlen("0 0:999"));
	paramBuilder->param("gramLoad", true);

	ret = AIKIT_Start(ABILITY, AIKIT_Builder::build(paramBuilder), nullptr, &handle);
	printf("AIKIT_Start:%d\n", ret);
	if (ret != 0) {
		printf("AIKIT_Start failed\n");
		goto  exit;
	}

	file = fopen(file_path, "rb");
	if (file == nullptr) {
		printf("fopen failed\n");
		goto exit;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	dataBuilder = AIKIT_DataBuilder::create();

	while (fileSize > 0) {
		readLen = fread(data, 1, sizeof(data), file);
		dataBuilder->clear();

		aiAudio_raw = AiAudio::get("wav")->data(data, 320)->valid();
		dataBuilder->payload(aiAudio_raw);
		ret = AIKIT_Write(handle, AIKIT_Builder::build(dataBuilder));
		if (ret != 0) {
			printf("AIKIT_Write:%d failed\n", ret);
			goto  exit;
		}
		fileSize -= readLen;
	}

	// �ݴ�����ϣ�����AIKIT_End���������Ự
	ret = AIKIT_End(handle);

exit:
	if (handle != nullptr)
		AIKIT_End(handle);

	if (file != nullptr) {
		fclose(file);
		file = nullptr;
	}

	if (dataBuilder != nullptr) {
		delete dataBuilder;
		dataBuilder = nullptr;
	}

	if (paramBuilder != nullptr) {
		delete paramBuilder;
		paramBuilder = nullptr;
	}

	return ret;
}

AEE_LIB_HELPER_API int AEE_lib_WakeFromMicrophone() {
	int ret = 0;
	AIKIT_DataBuilder* dataBuilder = nullptr;
	AIKIT_ParamBuilder* paramBuilder = nullptr;
	AIKIT_HANDLE* handle = nullptr;
	AiAudio* aiAudio_raw = nullptr;
	DWORD bufsize;
	int len = 0;
	DWORD audio_count = 0;
	int count = 0;
	int index[] = { 0 };

	HWAVEIN hWaveIn = nullptr;  //�����豸
	WAVEFORMATEX waveform = { 0 }; //�ɼ���Ƶ�ĸ�ʽ���ṹ��
	BYTE* pBuffer = nullptr;//�ɼ���Ƶʱ�����ݻ���
	WAVEHDR wHdr = { 0 }; //�ɼ���Ƶʱ�������ݻ���Ľṹ��
	HANDLE          wait = NULL;

	waveform.wFormatTag = WAVE_FORMAT_PCM;//������ʽΪPCM
	waveform.nSamplesPerSec = 16000;//��Ƶ������
	waveform.wBitsPerSample = 16;//��������
	waveform.nChannels = 1;//����������
	waveform.nAvgBytesPerSec = 16000;//ÿ���������
	waveform.nBlockAlign = 2;//һ����Ĵ�С������bit���ֽ�������������
	waveform.cbSize = 0;//һ��Ϊ0

	wait = CreateEvent(NULL, 0, 0, NULL);
	//ʹ��waveInOpen����������Ƶ�ɼ�
	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	bufsize = 1024 * 500;//�����ʵ���С���ڴ�洢��Ƶ���ݣ����ʵ������ڴ��С������¼��ʱ�䣬���ȡ�������ڴ������

	paramBuilder = AIKIT_ParamBuilder::create();
	ret = AIKIT_SpecifyDataSet(ABILITY, "key_word", index, 1);
	printf("AIKIT_SpecifyDataSet:%d\n", ret);
	if (ret != 0) {
		goto exit;
	}
	paramBuilder->param("wdec_param_nCmThreshold", "0 0:999", (uint32_t)strlen("0 0:999"));
	paramBuilder->param("gramLoad", true);

	ret = AIKIT_Start(ABILITY, AIKIT_Builder::build(paramBuilder), nullptr, &handle);
	printf("AIKIT_Start:%d\n", ret);
	if (ret != 0) {
		return ret;
	}

	pBuffer = (BYTE*)malloc(bufsize);
	wHdr.lpData = (LPSTR)pBuffer;
	wHdr.dwBufferLength = bufsize;
	wHdr.dwBytesRecorded = 0;
	wHdr.dwUser = 0;
	wHdr.dwFlags = 0;
	wHdr.dwLoops = 1;
	waveInPrepareHeader(hWaveIn, &wHdr, sizeof(WAVEHDR));//׼��һ���������ݿ�ͷ����¼��
	waveInAddBuffer(hWaveIn, &wHdr, sizeof(WAVEHDR));//ָ���������ݿ�Ϊ¼�����뻺��
	waveInStart(hWaveIn);//��ʼ¼��


	dataBuilder = AIKIT_DataBuilder::create();
	//while (audio_count< bufsize && wakeupFlage!=1)//���λ���
	while (audio_count < bufsize)//��������
	{
		Sleep(200);//�ȴ�����¼��5s
		len = 10 * FRAME_LEN; //16k��Ƶ��10֡ ��ʱ��200ms��

		if (audio_count >= wHdr.dwBytesRecorded)
		{
			len = 0;
		}

		printf(">>>>>>>>count=%d\n", count++);
		dataBuilder->clear();
		aiAudio_raw = AiAudio::get("wav")->data((const char*)&pBuffer[audio_count], len)->valid();
		dataBuilder->payload(aiAudio_raw);
		ret = AIKIT_Write(handle, AIKIT_Builder::build(dataBuilder));
		if (ret != 0) {
			printf("AIKIT_Write:%d\n", ret);
			goto  exit;
		}
		audio_count += len;

	}

exit:
	if (handle != nullptr)
		AIKIT_End(handle);
	if (hWaveIn != NULL)
	{
		waveInStop(hWaveIn);
		waveInReset(hWaveIn);//ֹͣ¼��
		waveInClose(hWaveIn);
		hWaveIn = nullptr;
	}
	if (NULL != pBuffer)
	{
		free(pBuffer);
	}
	if (dataBuilder != nullptr) {
		delete dataBuilder;
		dataBuilder = nullptr;
	}

	return ret;
}