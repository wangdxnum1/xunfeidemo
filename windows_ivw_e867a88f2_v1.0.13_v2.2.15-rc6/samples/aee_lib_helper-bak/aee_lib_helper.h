#ifdef AEELIBHELPER_EXPORTS
#define AEE_LIB_HELPER_API __declspec(dllexport)
#else
#define AEE_LIB_HELPER_API __declspec(dllimport)
#endif

//typedef void (*AIKIT_OnOutput)(AIKIT_HANDLE* handle, const AIKIT_OutputData* output);
//typedef void (*AIKIT_OnEvent)(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue);
//typedef void (*AIKIT_OnError)(AIKIT_HANDLE* handle, int32_t err, const char* desc);
//typedef struct {
//	AIKIT_OnOutput outputCB;  //輸出回调
//	AIKIT_OnEvent  eventCB;   //事件回调
//	AIKIT_OnError  errorCB;   //错误回调
//} AIKIT_Callbacks;

//
// SDK 3个回调函数定义 Callback Function Definition
//
// 输出回调
typedef void (*AEE_lib_AIKIT_OnOutput)(const char* ability_id, const char* key, const char* value);
// 事件回调
typedef void (*AEE_lib_AIKIT_OnEvent)(int event_type, const char* key, const char* value);
// 错误回调
typedef void (*AEE_lib_AIKIT_OnError)(int err, const char* desc);

/// <summary>
/// 初始化SDK lib，程序启动的时候初始化一次即可
/// </summary>
/// <param name="app_id"></param>
/// <param name="api_secret"></param>
/// <param name="api_key"></param>
/// /// <param name="on_output"></param>
/// <param name="on_event"></param>
/// <param name="on_error"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_Init(
	const char* app_id, 
	const char* api_secret, 
	const char* api_key, 
	AEE_lib_AIKIT_OnOutput on_output,
	AEE_lib_AIKIT_OnEvent on_event,
	AEE_lib_AIKIT_OnError on_error);

/// <summary>
/// 反初始化SDK，程序退出的时候调用即可
/// </summary>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_UnInit();

/// <summary>
/// 初始化语音唤醒能力引擎
/// </summary>
/// <param name="ability"></param>
/// /// <param name="key_word_file_path">关键词路径</param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineInit(const char* ability, const char* key_word_file_path);

/// <summary>
/// 能力引擎逆初始化,释放能力及对应引擎占用所有资源
/// </summary>
/// <param name="ability"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineUnInit(const char* ability);

/// <summary>
/// 设置一个语音文件路径，从文件唤醒
/// </summary>
/// <param name="file_path"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_WakeFromFile(const char* file_path);

/// <summary>
/// 从语音唤醒
/// </summary>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_WakeFromMicrophone();