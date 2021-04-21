/* =================================================================
 * Copyright (c) 2020 Botson Corp 
 * 
 * This program is proprietary and confidential information of Botson.
 * And may not be used unless duly authorized. 
 * 
 * Revision: 
 * Date: 5/18/20
 * Author: luc@sietium.com
 * Descriptions: 
 * 
 */
// =================================================================

#ifndef BOTSON_SDK_HPP
#define BOTSON_SDK_HPP

# if defined(_WIN32) || defined(_WIN64)
#  ifdef BOTSON_SDK_DLL
#   define BOTSON_SDK_API __declspec(dllexport)
#  else
#   define BOTSON_SDK_API __declspec(dllimport)
#  endif
# else // UNIX
#  ifdef BOTSON_SDK_DLL
#   define BOTSON_SDK_API __attribute__((visibility("default")))
#  else
#   define BOTSON_SDK_API
#  endif
# endif


#endif //BOTSON_SDK_HPP
