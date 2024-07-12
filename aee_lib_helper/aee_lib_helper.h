#ifdef AEELIBHELPER_EXPORTS
#define AEE_LIB_HELPER_API __declspec(dllexport)
#else
#define AEE_LIB_HELPER_API __declspec(dllimport)
#endif

//
// SDK 相关能力定义
//

// 语音唤醒
#define AEE_LIB_AWAKEN_BY_VOICE					0x00000001

// 命令词识别
#define AEE_LIB_COMMAND_WORD_RECOGNITION		0x00000002

// 所有能力， 语音唤醒 + 命令词识别
#define AEE_LIB_ALL_ABILITY						(AEE_LIB_AWAKEN_BY_VOICE | AEE_LIB_COMMAND_WORD_RECOGNITION)

// 无限等待
#define AEE_LIB_INFINITE						(-1)

//
// SDK 3个回调函数定义 Callback Function Definition
//
// 输出回调
typedef void (*AEE_lib_AIKIT_OnOutput)(const char* ability_id, const char* key, const char* value);
// 事件回调
typedef void (*AEE_lib_AIKIT_OnEvent)(int event_type, const char* key, const char* value);
// 错误回调
typedef void (*AEE_lib_AIKIT_OnError)(int err, const char* desc);

/****************************************************SDK通用接口*************************************************************/

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
	int ability,
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

///// <summary>
///// 初始化指定能力引擎
///// </summary>
///// <param name="ability"></param>
///// <returns>错误码 0=成功，其他表示失败</returns>
//extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineInit(const char* ability);
//
///// <summary>
///// 能力引擎逆初始化,释放能力及对应引擎占用所有资源
///// </summary>
///// <param name="ability"></param>
///// <returns>错误码 0=成功，其他表示失败</returns>
//extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineUnInit(const char* ability);

/****************************************************语音唤醒相关接口*************************************************************/
/// <summary>
/// 初始化语音唤醒能力引擎
/// </summary>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_EngineInit();

/// <summary>
/// 语音唤醒能力引擎逆初始化,释放能力及对应引擎占用所有资源
/// </summary>
/// <param name="ability"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_EngineUnInit();

/// <summary>
/// 
/// </summary>
/// <param name="filepath"></param>
/// <returns></returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_SetKeywordData(const char* filepath);

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


/****************************************************命令词识别相关接口*************************************************************/
/// <summary>
/// 命令词识别能力引擎初始化
/// </summary>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Command_Word_EngineInit();

/// <summary>
/// 命令词识别你呢里引擎逆初始化,释放能力及对应引擎占用所有资源
/// </summary>
/// <param name="ability"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_Command_Word_EngineUnInit();

/// <summary>
/// 设置命令词的配置文件路径
/// </summary>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_SetCommandWordData(const char* filepath);

/// <summary>
/// 从语音文件识别命令词
/// </summary>
/// <param name="file_path"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_CommandFromFile(const char* file_path);

/// <summary>
/// 从麦克风识别命令词,调用这个接口口，默认开始监听命令词，会阻塞调用线程
/// 注意:这个接口只能在当前进程里调用一次，不能多线程同时调用
/// </summary>
/// <param name="milli_seconds">超时时间可以自定义</param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_CommandFromMicrophone(const int milli_seconds);

/// <summary>
/// 停止从麦克风语音识别命令词
/// 注意:超时之后，不建议再调用此接口
/// 调用此接口之后，会提前结束当前回话，即从麦克风命令词识别
/// </summary>
/// <param name="file_path"></param>
/// <returns>错误码 0=成功，其他表示失败</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_StopCommandWordRecognition();