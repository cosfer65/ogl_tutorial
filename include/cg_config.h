#ifndef __cg_config_h__
#define __cg_config_h__

#if defined(_WIN64) || defined(WIN64)
#pragma message("64bit compilation")
#else
#pragma message("32bit compilation")
#endif

#define GL_DATASIZE GL_FLOAT

#endif //__cg_config_h__
