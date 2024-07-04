using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace mysample2
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // 设置工作目录
            string currentFolderPath = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location);
            System.IO.Directory.SetCurrentDirectory(currentFolderPath);

            // app 信息
            string appId = "fcde2f9b";
            string apiSecret = "YTY1ZDM1OWU1OWM1YjYxNmRiNmZjYmMy";
            string apiKey = "3813c0bf114b0221b61db7b012f40dba";

            // 模块能力-语音唤醒标识码
            int ability = 1;

            // 进程启动的时候，初始化下
            AEElibHelper.AEE_lib_Init(ability, appId, apiSecret, apiKey, OnOutput, OnEvent, OnError);

            // 开启语音唤醒引擎
            AEElibHelper.AEE_lib_AIKIT_Awaken_EngineInit();

            // 关键词所在路径
            string keywordFilePath = ".\\resource\\ivw70\\keyword.txt";
            AEElibHelper.AEE_lib_SetKeywordData(keywordFilePath);

            // 开始唤醒
            Console.WriteLine("===========================");
            Console.WriteLine("where is audio data from? \n0: audio file\n1: microphone");
            Console.WriteLine("===========================");
            string input = Console.ReadLine();
            if (input.Equals("0"))
            {
                // 使用语音文件唤醒
                AEElibHelper.AEE_lib_WakeFromFile(".\\testAudio\\xbxb.pcm");
            }
            else
            {
                // 使用麦克风语音唤醒
                AEElibHelper.AEE_lib_WakeFromMicrophone();
            }

            // 关闭语音唤醒引擎
            AEElibHelper.AEE_lib_AIKIT_Awaken_EngineUnInit();


            // 程序退出的时候，反初始化一下
            AEElibHelper.AEE_lib_UnInit();

            Console.ReadLine();
        }

        static void OnOutput(string abilityId, string key, string value)
        {
            Console.WriteLine("C# OnOutput abilityID :{0}", abilityId);
            Console.WriteLine("C# OnOutput key: {0} ", key);
            Console.WriteLine("C# OnOutput value: {0}", value);
        }

        static void OnEvent(int eventType, string key, string value)
        {
            Console.WriteLine("C# OnEvent: {0}", eventType);
        }

        static void OnError(int err, string desc)
        {
            Console.WriteLine("C# OnError: {0}", err);
        }
    }
}
