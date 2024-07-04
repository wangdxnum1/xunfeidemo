#ifndef AEE_LIB_HELPER_GLOBAL_DEFINE_H_
#define AEE_LIB_HELPER_GLOBAL_DEFINE_H_

#if _DEBUG
#define LOG_INFO  printf
#else
#define LOG_INFO
#endif

#ifndef FlagOn
#define FlagOn(_F,_SF)        ((_F) & (_SF))
#endif

#endif