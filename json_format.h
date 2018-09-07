/*************************************************************************************************
* purpose:  利用rapidjson进行json的编解码，方便业务使用
* author :  wisdom/yzw5164129@163.com.com
* date   :  2018-09-06
* readme:
* 若json中的key为var，则变量名必须定义为m_var,
* 解析的时候只有最外层为开始用PRASE_JSON_OBJECT_BEGIN与PRASE_JSON_OBJECT_END，
* 其余内层对象都使用PRASE_JSON_SUB_OBJECT_BEGIN与PRASE_JSON_SUB_OBJECT_END, 格式化的时候类似，注意区别SUB_OBJECT使用
*************************************************************************************************/
#include "rapidjson.h"
#include "document.h"
#include "filereadstream.h"
#include "filewritestream.h"
#include "stringbuffer.h"
#include "writer.h"

//将对象格式化为json格式
#define FORMAT_JSON_OBJECT_BEGIN \
    std::string FormatJsonToString() \
    { \
		rapidjson::Document jsonDoc; \
        rapidjson::Document::AllocatorType &allocator = jsonDoc.GetAllocator(); \
        jsonDoc.SetObject();

#define FORMAT_JSON_SUB_OBJECT_BEGIN \
    void FormatJsonToString(rapidjson::Value &jsonDoc, rapidjson::Document::AllocatorType &allocator) \
    {

#define FORMAT_JSON_BUNBER(name) \
        jsonDoc.AddMember(#name, m_##name, allocator); 

#define FORMAT_JSON_STRING(name) \
        jsonDoc.AddMember(#name, m_##name, allocator); 

#define FORMAT_JSON_BOOL(name) \
        if (m_##name) { \
            jsonDoc.AddMember(#name, true, allocator); \
        }\
        else { \
            jsonDoc.AddMember(#name, false, allocator); \
        }

#define FORMAT_JSON_LIST(name) \
        rapidjson::Value name##JsonArray(rapidjson::kArrayType); \
        for (auto it = m_##name.begin(); it != m_##name.end(); ++it) { \
            name##JsonArray.PushBack(*it, allocator); \
        } \
        jsonDoc.AddMember(#name, name##JsonArray, allocator);

#define FORMAT_JSON_OBJECT(name) \
		rapidjson::Value name##Obj(rapidjson::kObjectType); \
        m_##name.FormatJsonToString(name##Obj, allocator); \
        jsonDoc.AddMember(#name, name##Obj, allocator);

/*
* 这里用auto类型，是容器类型的可以通用，否则的话就需要分别定义list，vector，set等等容器类型了
*/
#define FORMAT_JSON_OBJECT_LIST(name, type) \
        rapidjson::Value name##ObjArray(rapidjson::kArrayType); \
        for (auto it = m_##name.begin(); it != m_##name.end(); ++it) { \
			rapidjson::Value name##Obj(rapidjson::kObjectType); \
			it->FormatJsonToString(name##Obj, allocator); \
            name##ObjArray.PushBack(name##Obj, allocator); \
        } \
        jsonDoc.AddMember(#name, name##ObjArray, allocator);

#define FORMAT_JSON_SUB_OBJECT_END \
	}

#define FORMAT_JSON_OBJECT_END \
        rapidjson::StringBuffer buffer; \
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer); \
        jsonDoc.Accept(writer); \
        std::string formatJsonStr = buffer.GetString(); \
        return formatJsonStr; \
    }



//解析json格式字符串到对象中
#define PRASE_JSON_OBJECT_BEGIN \
    bool PraseJsonString(std::string &jsonStr) \
    { \
        rapidjson::Document jsonDoc; \
        jsonDoc.Parse(jsonStr.c_str()); \
        if (jsonDoc.HasParseError()) { \
            std::cout << "Json Parse error:" << jsonDoc.GetParseError() \
                << ", Error json:" << jsonStr.c_str(); \
            return false; \
        }

#define PRASE_JSON_SUB_OBJECT_BEGIN \
    bool PraseJsonString(rapidjson::Value &jsonDoc) \
    { \

#define PRASE_JSON_INT(name) \
        if (jsonDoc.HasMember(#name)) { \
			m_##name = jsonDoc[#name].GetInt(); \
        }

#define PRASE_JSON_DOUBLE(name) \
        if (jsonDoc.HasMember(#name)) { \
            m_##name = jsonDoc[#name].GetDouble(); \
        } 


#define PRASE_JSON_STRING(name) \
        if (jsonDoc.HasMember(#name)) { \
            m_##name = jsonDoc[#name].GetString(); \
        }

#define PRASE_JSON_BOOL(name) \
        if (jsonDoc.HasMember(#name)) { \
            m_##name = jsonDoc[#name].GetBool(); \
        }

#define PRASE_JSON_INT_LIST(name) \
        if (jsonDoc.HasMember(#name)) { \
            rapidjson::Value name##Value; \
            name##Value = jsonDoc[#name]; \
            if (name##Value.IsArray() && !name##Value.Empty()) { \
                for (rapidjson::SizeType i = 0; i < name##Value.Size(); ++i) { \
                    m_##name.push_back(name##Value[i].GetInt()); \
                } \
            } \
        }

#define PRASE_JSON_DOUBLE_LIST(name) \
        if (jsonDoc.HasMember(#name)) { \
            rapidjson::Value name##Value; \
            name##Value = jsonDoc[#name]; \
            if (name##Value.IsArray() && !name##Value.Empty()) { \
                for (rapidjson::SizeType i = 0; i < name##Value.Size(); ++i) { \
                    m_##name.push_back(name##Value[i].GetDouble()); \
                } \
            } \
        }


#define PRASE_JSON_STRING_LIST(name) \
        if (jsonDoc.HasMember(#name)) { \
            rapidjson::Value name##Value; \
            name##Value = jsonDoc[#name]; \
            if (name##Value.IsArray() && !name##Value.Empty()) { \
                for (rapidjson::SizeType i = 0; i < name##Value.Size(); ++i) { \
                    m_##name.push_back(name##Value[i].GetString()); \
                } \
            } \
        }

#define PRASE_JSON_BOOL_LIST(name) \
        if (jsonDoc.HasMember(#name)) { \
            rapidjson::Value name##Value; \
            name##Value = jsonDoc[#name]; \
            if (name##Value.IsArray() && !name##Value.Empty()) { \
                for (rapidjson::SizeType i = 0; i < name##Value.Size(); ++i) { \
                    m_##name.push_back(name##Value[i].GetBool()); \
                } \
            } \
        }

/*
* 不能使用rapidjson::Value name##JsonObj = jsonDoc[#name];，这样会调用复制, 而复制构造函数是private的，这样会报错
* 只能分开写成两行: rapidjson::Value name##JsonObj;
name##JsonObj = jsonDoc[#name];
*/
#define PRASE_JSON_OBJECT(name) \
        if (jsonDoc.HasMember(#name)) { \
            rapidjson::Value name##JsonObj; \
			name##JsonObj = jsonDoc[#name]; \
            m_##name.PraseJsonString(name##JsonObj); \
        }

#define PRASE_JSON_OBJECT_LIST(name, type) \
        if (jsonDoc.HasMember(#name)) { \
            rapidjson::Value name##ObjArray; \
            name##ObjArray = jsonDoc[#name]; \
            if (name##ObjArray.IsArray() && !name##ObjArray.Empty()) { \
                for (rapidjson::SizeType i = 0; i < name##ObjArray.Size(); ++i) { \
                    rapidjson::Value name##Value; \
                    name##Value = name##ObjArray[i]; \
                    type name##Obj; \
                    name##Obj.PraseJsonString(name##Value); \
                    m_##name.push_back(name##Obj); \
                } \
            } \
        }


#define PRASE_JSON_SUB_OBJECT_END \
        return true; \
    }

#define PRASE_JSON_OBJECT_END \
        return true; \
    }


