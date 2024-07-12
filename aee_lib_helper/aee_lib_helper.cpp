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
#include <string>
#include <vector>

#include "../include/aikit_biz_api.h"
#include "../include/aikit_constant.h"
#include "../include/aikit_biz_config.h"
#include "../include/aikit_biz_builder.h"

#include "speech_recognizer.h"

#include "global_define.h"

#pragma comment(lib, "winmm.lib")  

#define FRAME_LEN	640 //16k采样率的16bit音频，一帧的大小为640B, 时长20ms

using namespace std;
using namespace AIKIT;

enum {
	//EVT_START = 0,
	EVT_STOP,
	EVT_QUIT,
	EVT_TOTAL
};

static HANDLE gEvents[EVT_TOTAL] = { NULL,NULL};

//
// 能力id
//

// 语音唤醒
static const char* kABILITY_AWAKEN_BY_VOICE = "e867a88f2";

// 离线命令词识别
static const char* kABILITY_COMMAND_WORD_RECOGNITION = "e75f07b62";

// 命令词识别最大超时时间
static const DWORD kCOMMAND_WORD_RECOGNITION_TiMEOUT = 60 * 1000;

//
// call back
//
static AEE_lib_AIKIT_OnOutput OnOutputDelegate = nullptr;
static AEE_lib_AIKIT_OnEvent OnEventDelegate = nullptr;
static AEE_lib_AIKIT_OnError OnErrorDelegate = nullptr;

//
// call back
//
void OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output) {

	//system("chcp 65001");
	LOG_INFO("OnOutput abilityID :%s\n", handle->abilityID);
	LOG_INFO("OnOutput key:%s\n", output->node->key);
	LOG_INFO("OnOutput value:%s\n", (char*)output->node->value);

	if (OnOutputDelegate) {
		OnOutputDelegate(handle->abilityID, output->node->key, (char*)output->node->value);
	}
}

void OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue) {
	LOG_INFO("OnEvent:%d\n", eventType);

	if (OnEventDelegate) {
		OnEventDelegate((int)eventType, eventValue->node->key, (char*)eventValue->node->value);
	}
}

void OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc) {
	LOG_INFO("OnError:%d\n", err);

	if (OnErrorDelegate) {
		OnErrorDelegate(err, desc);
	}
}
//
// sdk method
//
static int esr_mic(DWORD dwMilliseconds);
static int esr_file(AIKIT_ParamBuilder* paramBuilder, const char* audio_path, int fsa_count, long* readLen);

//
// internel or static private method
//
std::string ToAbilityString(int ability);
std::string Ability2AbilityString(int ability);

// This is an example of an exported function.
AEE_LIB_HELPER_API int AEE_lib_Init(
	int ability,
	const char* app_id,
	const char* api_secret,
	const char* api_key,
	AEE_lib_AIKIT_OnOutput on_output,
	AEE_lib_AIKIT_OnEvent on_event,
	AEE_lib_AIKIT_OnError on_error)
{
	LOG_INFO("ability : %d, app_id : %s, api_secret : %s, api_key : %s \r\n", ability, app_id, api_secret, api_key);

	std::string real_ability = ToAbilityString(ability);

	AIKIT_Configurator::builder()
		.app()
		.appID(app_id)
		.apiSecret(api_secret)
		.apiKey(api_key)
		.workDir(".\\")
		.auth()
		.authType(0)
		.ability(real_ability.c_str())
		.log()
		.logMode(2)
		.logPath(".\\aikit.log");
	int ret = AIKIT_Init();
	if (ret != 0) {
		LOG_INFO("AIKIT_Init failed:%d\n", ret);
		return ret;
	}

	AIKIT_Callbacks cbs = { OnOutput,OnEvent,OnError };
	ret = AIKIT_RegisterAbilityCallback(real_ability.c_str(), cbs);
	if (ret != 0) {
		LOG_INFO("AIKIT_RegisterAbilityCallback failed:%d\n", ret);
		return ret;
	}

	//
	// set callback
	//
	OnOutputDelegate = on_output;
	OnEventDelegate = on_event;
	OnErrorDelegate = on_error;

	return 0;
}

AEE_LIB_HELPER_API int AEE_lib_UnInit() {
	return AIKIT_UnInit();
}

int AEE_lib_AIKIT_EngineInit(const char* ability) {
	int ret = 0;
	AIKIT_BizParam* param = nullptr;
	AIKIT_ParamBuilder* engine_paramBuilder = nullptr;

	// 语音唤醒
	if (strcmp(ability, kABILITY_AWAKEN_BY_VOICE) == 0) {
		// do nothing
	}
	else if (strcmp(ability, kABILITY_COMMAND_WORD_RECOGNITION) == 0) {
		// 命令词识别
		engine_paramBuilder = AIKIT_ParamBuilder::create();
		engine_paramBuilder->clear();
		engine_paramBuilder->param("decNetType", "fsa", (uint32_t)strlen("fsa"));
		engine_paramBuilder->param("punishCoefficient", 0.0);
		engine_paramBuilder->param("wfst_addType", 0);		//0-中文，1-英文

		param = AIKIT_Builder::build(engine_paramBuilder);
	}

	ret = AIKIT_EngineInit(ability, param);
	if (ret != 0) {
		LOG_INFO("AIKIT_EngineInit failed:%d\n", ret);
		goto exit;
	}
exit:
	if (engine_paramBuilder != nullptr) {
		delete engine_paramBuilder;
		engine_paramBuilder = nullptr;
	}

	return ret;
}

int AEE_lib_AIKIT_EngineUnInit(const char* ability) {
	return AIKIT_EngineUnInit(ability);
}

/********************************************************************************************************************************/
/****************************************************语音唤醒相关接口*************************************************************/
/********************************************************************************************************************************/
AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_EngineInit() {
	return AEE_lib_AIKIT_EngineInit(kABILITY_AWAKEN_BY_VOICE);
}

AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_EngineUnInit() {
	return AEE_lib_AIKIT_EngineUnInit(kABILITY_AWAKEN_BY_VOICE);
}

AEE_LIB_HELPER_API int AEE_lib_AIKIT_SetKeywordData(const char* filepath) {
	int ret = 0;

	//设置唤醒词
	AIKIT_CustomData customData = { 0 };
	customData.key = "key_word";
	customData.index = 0;
	customData.from = AIKIT_DATA_PTR_PATH;
	// .\\resource\\ivw70\\xbxb.txt
	customData.value = (void*)filepath;
	customData.len = (int32_t)strlen(filepath);
	customData.next = nullptr;
	customData.reserved = nullptr;
	LOG_INFO("AIKIT_LoadData start!\n");
	ret = AIKIT_LoadData(kABILITY_AWAKEN_BY_VOICE, &customData);
	LOG_INFO("AIKIT_LoadData end!\n");
	LOG_INFO("AIKIT_LoadData:%d\n", ret);

	return ret;
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
	ret = AIKIT_SpecifyDataSet(kABILITY_AWAKEN_BY_VOICE, "key_word", index, 1);
	LOG_INFO("AIKIT_SpecifyDataSet:%d\n", ret);
	if (ret != 0) {
		LOG_INFO("AIKIT_SpecifyDataSet failed \n");
		goto  exit;
	}

	// 通过 start 方法开启会话，传入当前能力所需要指定的参数。
	paramBuilder = AIKIT_ParamBuilder::create();
	paramBuilder->param("wdec_param_nCmThreshold", "0 0:999", (uint32_t)strlen("0 0:999"));
	paramBuilder->param("gramLoad", true);

	ret = AIKIT_Start(kABILITY_AWAKEN_BY_VOICE, AIKIT_Builder::build(paramBuilder), nullptr, &handle);
	LOG_INFO("AIKIT_Start:%d\n", ret);
	if (ret != 0) {
		LOG_INFO("AIKIT_Start failed\n");
		goto  exit;
	}

	file = fopen(file_path, "rb");
	if (file == nullptr) {
		LOG_INFO("fopen failed\n");
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
			LOG_INFO("AIKIT_Write:%d failed\n", ret);
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
	ret = AIKIT_SpecifyDataSet(kABILITY_AWAKEN_BY_VOICE, "key_word", index, 1);
	LOG_INFO("AIKIT_SpecifyDataSet:%d\n", ret);
	if (ret != 0) {
		goto exit;
	}
	paramBuilder->param("wdec_param_nCmThreshold", "0 0:999", (uint32_t)strlen("0 0:999"));
	paramBuilder->param("gramLoad", true);

	ret = AIKIT_Start(kABILITY_AWAKEN_BY_VOICE, AIKIT_Builder::build(paramBuilder), nullptr, &handle);
	LOG_INFO("AIKIT_Start:%d\n", ret);
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

		LOG_INFO(">>>>>>>>count=%d\n", count++);
		dataBuilder->clear();
		aiAudio_raw = AiAudio::get("wav")->data((const char*)&pBuffer[audio_count], len)->valid();
		dataBuilder->payload(aiAudio_raw);
		ret = AIKIT_Write(handle, AIKIT_Builder::build(dataBuilder));
		if (ret != 0) {
			LOG_INFO("AIKIT_Write:%d\n", ret);
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

/********************************************************************************************************************************/
/****************************************************命令词识别相关接口***********************************************************/
/********************************************************************************************************************************/

AEE_LIB_HELPER_API int AEE_lib_AIKIT_Command_Word_EngineInit() {
	return AEE_lib_AIKIT_EngineInit(kABILITY_COMMAND_WORD_RECOGNITION);
}

AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_Command_Word_EngineUnInit() {
	AIKIT_UnLoadData(kABILITY_COMMAND_WORD_RECOGNITION, "FSA", 0);

	return AEE_lib_AIKIT_EngineUnInit(kABILITY_COMMAND_WORD_RECOGNITION);
}

AEE_LIB_HELPER_API int AEE_lib_AIKIT_SetCommandWordData(const char* filepath) {
	AIKIT_CustomBuilder* customBuilder = nullptr;

	customBuilder = AIKIT_CustomBuilder::create();
	customBuilder->clear();
	//// 插词
	//customBuilder->textPath("FSA", ".\\resource\\cnenesr\\fsa\\en_fsa.txt", 0);
	customBuilder->textPath("FSA", filepath, 0);
	int ret = AIKIT_LoadData(kABILITY_COMMAND_WORD_RECOGNITION, AIKIT_Builder::build(customBuilder));

	if (ret != 0) {
		LOG_INFO("AIKIT_LoadData failed:%d\n", ret);
		goto exit;
	}

exit:
	if (customBuilder != nullptr) {
		delete customBuilder;
		customBuilder = nullptr;
	}

	return ret;
}

AEE_LIB_HELPER_API int AEE_lib_CommandFromFile(const char* file_path) {
	AIKIT_ParamBuilder* paramBuilder = nullptr;
	long readLen = 0;
	int ret = 0;

	paramBuilder = AIKIT_ParamBuilder::create();
	paramBuilder->clear();
	paramBuilder->param("languageType", 0);		//0-中文 1-英文
	paramBuilder->param("vadEndGap", 60);
	paramBuilder->param("vadOn", true);
	paramBuilder->param("beamThreshold", 20);
	paramBuilder->param("hisGramThreshold", 3000);
	paramBuilder->param("postprocOn", false);
	paramBuilder->param("vadResponsetime", 1000);
	paramBuilder->param("vadLinkOn", true);
	paramBuilder->param("vadSpeechEnd", 80);

	while (readLen >= 0)
	{
		// ".\\testAudio\\cn_test.pcm"
		ret = esr_file(paramBuilder, file_path, 1, &readLen);
		if (ret != 0 && ret != ESR_HAS_RESULT)
			goto exit;
	}

exit:
	if (paramBuilder != nullptr) {
		delete paramBuilder;
		paramBuilder = nullptr;
	}

	return ret;
}

AEE_LIB_HELPER_API int AEE_lib_CommandFromMicrophone(const int milli_seconds) {
	DWORD timeout = 0;

	if (milli_seconds == AEE_LIB_INFINITE) {
		timeout = INFINITE;
	}
	else {
		timeout = (DWORD)milli_seconds;
	}

	return esr_mic(timeout);
}

AEE_LIB_HELPER_API int AEE_lib_StopCommandWordRecognition() {
	SetEvent(gEvents[EVT_QUIT]);
	return 0;
}

/********************************************************************************************************************************/
/********************************************internel or static private method***************************************************/
/********************************************************************************************************************************/
static int esr_mic(DWORD dwMilliseconds)
{
	int errcode = 0;
	int i = 0;

	struct speech_rec esr;
	DWORD waitres = 0;
	bool isquit = false;

	errcode = sr_init(&esr, SR_MIC, DEFAULT_INPUT_DEVID);
	if (errcode) {
		LOG_INFO("speech recognizer init failed\n");
		return errcode;
	}

	for (i = 0; i < EVT_TOTAL; ++i) {
		gEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	// 开始录音
	if (errcode = sr_start_listening(&esr)) {
		LOG_INFO("start listen failed %d\n", errcode);
		goto exit;
	}

	while (true) {
		waitres = WaitForMultipleObjects(EVT_TOTAL, gEvents, FALSE, dwMilliseconds);
		switch (waitres) {
		case WAIT_FAILED:
			LOG_INFO("WaitForMultipleObjects failed \r\n");
		case WAIT_TIMEOUT:
			LOG_INFO("WAIT_TIMEOUT\n");
			sr_stop_listening(&esr);
			isquit = true;
			break;
		case WAIT_OBJECT_0 + EVT_STOP:
			if (errcode = sr_stop_listening(&esr)) {
				LOG_INFO("stop listening failed %d\n", errcode);
				isquit = true;
			}
			break;
		case WAIT_OBJECT_0 + EVT_QUIT:
			sr_stop_listening(&esr);
			isquit = true;
			break;
		default:
			break;
		}
		if (isquit)
			break;
	}

exit:
	for (i = 0; i < EVT_TOTAL; ++i) {
		if (gEvents[i]) {
			CloseHandle(gEvents[i]);
			gEvents[i] = NULL;
		}
	}

	sr_uninit(&esr);

	return 0;
}
int esr_file(AIKIT_ParamBuilder* paramBuilder, const char* audio_path, int fsa_count, long* readLen)
{
	int ret = 0;
	FILE* file = nullptr;
	long fileSize = 0;
	long curLen = 0;
	char data[320] = { 0 };
	int* index = (int*)malloc(fsa_count * sizeof(int));

	AIKIT_DataStatus status = AIKIT_DataBegin;
	AIKIT_DataBuilder* dataBuilder = nullptr;
	AIKIT_HANDLE* handle = nullptr;
	AiAudio* aiAudio_raw = nullptr;

	for (int i = 0; i < fsa_count; ++i)
	{
		index[i] = i;
	}
	ret = AIKIT_SpecifyDataSet(kABILITY_COMMAND_WORD_RECOGNITION, "FSA", index, fsa_count);
	if (ret != 0)
	{
		LOG_INFO("AIKIT_SpecifyDataSet failed:%d\n", ret);
		goto exit;
	}

	ret = AIKIT_Start(kABILITY_COMMAND_WORD_RECOGNITION, AIKIT_Builder::build(paramBuilder), nullptr, &handle);
	if (ret != 0)
	{
		LOG_INFO("AIKIT_Start failed:%d\n", ret);
		goto exit;
	}

	//file = fopen(".\\testAudio\\cn_test.pcm", "rb");
	file = fopen(audio_path, "rb");
	if (file == nullptr)
	{
		LOG_INFO("fopen failed\n");
		goto exit;
	}

	//注意，如果写入音频是wav，请去掉wav的头 44字节
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	//wav文件
 	//fseek(file, 44, SEEK_SET);

	dataBuilder = AIKIT_DataBuilder::create();

	while (fileSize > *readLen) {
		curLen = (long)fread(data, 1, sizeof(data), file);
		*readLen += curLen;
		dataBuilder->clear();
		if (*readLen == 320) {
			status = AIKIT_DataBegin;
		}
		else
			status = AIKIT_DataContinue;

		aiAudio_raw = AiAudio::get("audio")->data(data, curLen)->status(status)->valid();
		dataBuilder->payload(aiAudio_raw);

		ret = ESRGetRlt(handle, dataBuilder);
		if (ret != 0)
			goto exit;

		Sleep(10);
	}

	*readLen = -1;
	dataBuilder->clear();
	status = AIKIT_DataEnd;
	aiAudio_raw = AiAudio::get("audio")->data(data, 0)->status(status)->valid();
	dataBuilder->payload(aiAudio_raw);

	ret = ESRGetRlt(handle, dataBuilder);
	if (ret != 0)
		goto exit;

	ret = AIKIT_End(handle);
	if (ret != 0)
	{
		LOG_INFO("AIKIT_End failed : %d\n", ret);
	}

exit:
	if (handle != nullptr)
		AIKIT_End(handle);

	if (dataBuilder != nullptr) {
		delete dataBuilder;
		dataBuilder = nullptr;
	}
	if (file != nullptr) {
		fclose(file);
		file = nullptr;
	}
	if (index != NULL)
	{
		free(index);
		index = NULL;
	}

	return ret;
}

std::string ToAbilityString(int ability) {
	std::vector<int> all_abilitis;

	if (FlagOn(ability, AEE_LIB_AWAKEN_BY_VOICE)) {
		all_abilitis.push_back(AEE_LIB_AWAKEN_BY_VOICE);
	}

	if (FlagOn(ability, AEE_LIB_COMMAND_WORD_RECOGNITION)) {
		all_abilitis.push_back(AEE_LIB_COMMAND_WORD_RECOGNITION);
	}

	if (all_abilitis.empty()) return "";

	std::string real_ability;
	int index = 0;
	for (auto ab : all_abilitis) {
		if (index != 0) real_ability.append(";");

		real_ability.append(Ability2AbilityString(ab));
		++index;
	}

	return real_ability;
}

std::string Ability2AbilityString(int ability) {
	std::string real_ability;
	switch (ability)
	{
	case AEE_LIB_AWAKEN_BY_VOICE:
		real_ability = kABILITY_AWAKEN_BY_VOICE;
		break;
	case AEE_LIB_COMMAND_WORD_RECOGNITION:
		real_ability = kABILITY_COMMAND_WORD_RECOGNITION;
		break;
	default:
		break;
	}

	return real_ability;
}