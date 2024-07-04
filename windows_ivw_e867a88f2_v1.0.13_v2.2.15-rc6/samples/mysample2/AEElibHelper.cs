using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace mysample2
{
    internal class AEElibHelper
    {
        //// 输出回调
        //typedef void (* AEE_lib_AIKIT_OnOutput) (const char* ability_id, const char* key, const char* value);
        //// 事件回调
        //typedef void (* AEE_lib_AIKIT_OnEvent) (int event_type, const char* key, const char* value);
        //// 错误回调
        //typedef void (* AEE_lib_AIKIT_OnError) (int err, const char* desc);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void AEE_lib_AIKIT_OnOutput(string abilityId, string key, string value);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void AEE_lib_AIKIT_OnEvent(int eventType, string key, string value);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void AEE_lib_AIKIT_OnError(int err, string desc);

        /// <summary>
        /// 初始化SDK
        /// </summary>
        /// <param name="appId"></param>
        /// <param name="apiSecret"></param>
        /// <param name="apiKey"></param>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_Init", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_Init(int ability, string appId, string apiSecret, string apiKey, AEE_lib_AIKIT_OnOutput OnOutput, AEE_lib_AIKIT_OnEvent OnEvent, AEE_lib_AIKIT_OnError OnError);

        /// <summary>
        /// 反初始化SDK
        /// </summary>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_UnInit", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_UnInit();

        /********************************************************************************************************************************/
        /****************************************************语音唤醒相关接口*************************************************************/
        /********************************************************************************************************************************/

        /// <summary>
        /// 语音唤醒能力引擎
        /// </summary>
        /// <param name="ability"></param>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_AIKIT_Awaken_EngineInit", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_AIKIT_Awaken_EngineInit();

        /// <summary>
        /// 语音唤醒能力关闭/反初始化引擎
        /// </summary>
        /// <param name="ability"></param>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_AIKIT_Awaken_EngineUnInit", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_AIKIT_Awaken_EngineUnInit();

        /// <summary>
        /// 设置语音唤醒的keyword文件路径
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_AIKIT_SetKeywordData", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_SetKeywordData(string filePath);

        /// <summary>
        /// 从语音文件唤醒
        /// </summary>
        /// <param name="filePath"></param>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_WakeFromFile", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_WakeFromFile(string filePath);

        /// <summary>
        /// 从麦克风语音唤醒
        /// </summary>
        /// <returns></returns>
        [DllImport("aee_lib_helper.dll", EntryPoint = "AEE_lib_WakeFromMicrophone", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int AEE_lib_WakeFromMicrophone();
    }
}
