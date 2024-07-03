#ifdef AEELIBHELPER_EXPORTS
#define AEE_LIB_HELPER_API __declspec(dllexport)
#else
#define AEE_LIB_HELPER_API __declspec(dllimport)
#endif

//typedef void (*AIKIT_OnOutput)(AIKIT_HANDLE* handle, const AIKIT_OutputData* output);
//typedef void (*AIKIT_OnEvent)(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue);
//typedef void (*AIKIT_OnError)(AIKIT_HANDLE* handle, int32_t err, const char* desc);
//typedef struct {
//	AIKIT_OnOutput outputCB;  //ݔ���ص�
//	AIKIT_OnEvent  eventCB;   //�¼��ص�
//	AIKIT_OnError  errorCB;   //����ص�
//} AIKIT_Callbacks;

//
// SDK 3���ص��������� Callback Function Definition
//
// ����ص�
typedef void (*AEE_lib_AIKIT_OnOutput)(const char* ability_id, const char* key, const char* value);
// �¼��ص�
typedef void (*AEE_lib_AIKIT_OnEvent)(int event_type, const char* key, const char* value);
// ����ص�
typedef void (*AEE_lib_AIKIT_OnError)(int err, const char* desc);

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

/// <summary>
/// ��ʼ������������������
/// </summary>
/// <param name="ability"></param>
/// /// <param name="key_word_file_path">�ؼ���·��</param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineInit(const char* ability, const char* key_word_file_path);

/// <summary>
/// �����������ʼ��,�ͷ���������Ӧ����ռ��������Դ
/// </summary>
/// <param name="ability"></param>
/// <returns>������ 0=�ɹ���������ʾʧ��</returns>
extern "C" AEE_LIB_HELPER_API int AEE_lib_AIKIT_EngineUnInit(const char* ability);

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