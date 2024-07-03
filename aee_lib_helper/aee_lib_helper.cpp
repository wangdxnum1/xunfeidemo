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

#define FRAME_LEN	640 //16k采样率的16bit音频，一帧的大小为640B, 时长20ms

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

	//设置唤醒词
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

	// 如果加载失败，能力引擎需要反初始化下
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

	// 指定要使用的唤醒词文件数据集合，每次会话开启前需要调用。
	ret = AIKIT_SpecifyDataSet(ABILITY, "key_word", index, 1);
	printf("AIKIT_SpecifyDataSet:%d\n", ret);
	if (ret != 0) {
		printf("AIKIT_SpecifyDataSet failed \n");
		goto  exit;
	}

	// 通过 start 方法开启会话，传入当前能力所需要指定的参数。
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

	// 据处理完毕，调用AIKIT_End方法结束会话
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

	HWAVEIN hWaveIn = nullptr;  //输入设备
	WAVEFORMATEX waveform = { 0 }; //采集音频的格式，结构体
	BYTE* pBuffer = nullptr;//采集音频时的数据缓存
	WAVEHDR wHdr = { 0 }; //采集音频时包含数据缓存的结构体
	HANDLE          wait = NULL;

	waveform.wFormatTag = WAVE_FORMAT_PCM;//声音格式为PCM
	waveform.nSamplesPerSec = 16000;//音频采样率
	waveform.wBitsPerSample = 16;//采样比特
	waveform.nChannels = 1;//采样声道数
	waveform.nAvgBytesPerSec = 16000;//每秒的数据率
	waveform.nBlockAlign = 2;//一个块的大小，采样bit的字节数乘以声道数
	waveform.cbSize = 0;//一般为0

	wait = CreateEvent(NULL, 0, 0, NULL);
	//使用waveInOpen函数开启音频采集
	waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	bufsize = 1024 * 500;//开辟适当大小的内存存储音频数据，可适当调整内存大小以增加录音时间，或采取其他的内存管理方案

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
	waveInPrepareHeader(hWaveIn, &wHdr, sizeof(WAVEHDR));//准备一个波形数据块头用于录音
	waveInAddBuffer(hWaveIn, &wHdr, sizeof(WAVEHDR));//指定波形数据块为录音输入缓存
	waveInStart(hWaveIn);//开始录音


	dataBuilder = AIKIT_DataBuilder::create();
	//while (audio_count< bufsize && wakeupFlage!=1)//单次唤醒
	while (audio_count < bufsize)//持续唤醒
	{
		Sleep(200);//等待声音录制5s
		len = 10 * FRAME_LEN; //16k音频，10帧 （时长200ms）

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
		waveInReset(hWaveIn);//停止录音
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