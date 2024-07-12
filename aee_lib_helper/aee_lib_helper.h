#ifdef AEELIBHELPER_EXPORTS
#define AEE_LIB_HELPER_API __declspec(dllexport)
#else
#define AEE_LIB_HELPER_API __declspec(dllimport)
#endif

//
// SDK �����������
//

// ��������
#define AEE_LIB_AWAKEN_BY_VOICE					0x00000001

// �����ʶ��
#define AEE_LIB_COMMAND_WORD_RECOGNITION		0x00000002

// ���������� �������� + �����ʶ��
#define AEE_LIB_ALL_ABILITY						(AEE_LIB_AWAKEN_BY_VOICE | AEE_LIB_COMMAND_WORD_RECOGNITION)

// ���޵ȴ�
#define AEE_LIB_INFINITE						(-1)

//
// SDK 3���ص��������� Callback Function Definition
//
// ����ص�
typedef void (*AEE_lib_AIKIT_OnOutput)(const char* ability_id, const char* key, const char* value);
// �¼��ص�
typedef void (*AEE_lib_AIKIT_OnEvent)(int event_type, const char* key, const char* value);
// ����ص�
typedef void (*AEE_lib_AIKIT_OnError)(int err, const char* desc);

/****************************************************SDKͨ�ýӿ�*************************************************************/

/// <summary>
/// ��ʼ��SDK lib������������ʱ���ʼ��һ�μ���
/// </summary>
/// <param name="app_id"></param>
/// <param name="api_secret"></param>
/// <param name="api_key"></param>
/// /// <param name="on_output"></param>
/// <param name="on_event"></param>
/// <param name="on_error"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_Init(
	int ability,
	const char* app_id, 
	const char* api_secret, 
	const char* api_key, 
	AEE_lib_AIKIT_OnOutput on_output,
	AEE_lib_AIKIT_OnEvent on_event,
	AEE_lib_AIKIT_OnError on_error);

/// <summary>
/// ����ʼ��SDK�������˳���ʱ����ü���
/// </summary>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_UnInit();

///// <summary>
///// ��ʼ��ָ����������
///// </summary>
///// <param name="ability"></param>
///// <returns>������ 0=�ɹ���������ʾʧ��</returns>
//extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineInit(const char* ability);
//
///// <summary>
///// �����������ʼ��,�ͷ���������Ӧ����ռ��������Դ
///// </summary>
///// <param name="ability"></param>
///// <returns>������ 0=�ɹ���������ʾʧ��</returns>
//extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineUnInit(const char* ability);

/****************************************************����������ؽӿ�*************************************************************/
/// <summary>
/// ��ʼ������������������
/// </summary>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_EngineInit();

/// <summary>
/// �������������������ʼ��,�ͷ���������Ӧ����ռ��������Դ
/// </summary>
/// <param name="ability"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_EngineUnInit();

/// <summary>
/// 
/// </summary>
/// <param name="filepath"></param>
/// <returns></returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_SetKeywordData(const char* filepath);

/// <summary>
/// ����һ�������ļ�·�������ļ�����
/// </summary>
/// <param name="file_path"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_WakeFromFile(const char* file_path);

/// <summary>
/// ����������
/// </summary>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_WakeFromMicrophone();


/****************************************************�����ʶ����ؽӿ�*************************************************************/
/// <summary>
/// �����ʶ�����������ʼ��
/// </summary>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Command_Word_EngineInit();

/// <summary>
/// �����ʶ���������������ʼ��,�ͷ���������Ӧ����ռ��������Դ
/// </summary>
/// <param name="ability"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_Awaken_Command_Word_EngineUnInit();

/// <summary>
/// ��������ʵ������ļ�·��
/// </summary>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_SetCommandWordData(const char* filepath);

/// <summary>
/// �������ļ�ʶ�������
/// </summary>
/// <param name="file_path"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_CommandFromFile(const char* file_path);

/// <summary>
/// ����˷�ʶ�������,��������ӿڿڣ�Ĭ�Ͽ�ʼ��������ʣ������������߳�
/// ע��:����ӿ�ֻ���ڵ�ǰ���������һ�Σ����ܶ��߳�ͬʱ����
/// </summary>
/// <param name="milli_seconds">��ʱʱ������Զ���</param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_CommandFromMicrophone(const int milli_seconds);

/// <summary>
/// ֹͣ����˷�����ʶ�������
/// ע��:��ʱ֮�󣬲������ٵ��ô˽ӿ�
/// ���ô˽ӿ�֮�󣬻���ǰ������ǰ�ػ���������˷������ʶ��
/// </summary>
/// <param name="file_path"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_StopCommandWordRecognition();