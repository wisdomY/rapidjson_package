#! /usr/bin/env python
# -*- coding:utf-8 -*-
##############################################################################################
# * purpose:  生成json接口定义文件
# * author :  wisdom/zhiwei.yin@yingheying.com
# * date   :  2018-09-13
# * readme:
# * 将下面的jsonData字段改为需要生成接口定义的json串
# * 将generateFormat变量改为1或者2或者3， 1对应的是发送json串，需要format编码，2对应的是接受json串，需要prase解码, 3对应的编解码都需要
# * className对应的是生成接口定义的名字，即需要生成文件的类名，不需要以C作为前缀
##############################################################################################
import json

#需要生成代码的json串
jsonData = '''{"abc":"1123","bcd":[2],"cde":{"objc":3, "listcde":[{"sssss":"s"}]},"def":[{"qq":"test"}],"efg":true}'''
#生成的是format = 1 还是prase = 2
generateFormat = 3
#类名，前缀不需要加C，生成代码会自动添加
className = "Req"

#生成的字符串，全局变量
defStr = ""
#生成json格式化定义
def FormatJson(pyObj):
    formatStr = ""
    for key in pyObj.keys():
        if type(pyObj[key]) == unicode and type(pyObj[key]) == unicode:
            formatStr += "\n\t\tFORMAT_JSON_STRING(" + key + ")"
        elif type(pyObj[key]) == int:
            formatStr += "\n\t\tFORMAT_JSON_NUMBER(" + key + ")"
        elif type(pyObj[key]) == dict:
            formatStr += "\n\t\tFORMAT_JSON_OBJECT(" + key + ")"
        elif type(pyObj[key]) == list:
            if type(pyObj[key][0]) == dict:
                formatStr += "\n\t\tFORMAT_JSON_OBJECT_LIST(" + key + ", C" + key.capitalize() + ")"
            else:
                formatStr += "\n\t\tFORMAT_JSON_LIST(" + key + ")"
        elif type(pyObj[key]) == bool:
            formatStr += "\n\t\tFORMAT_JSON_NUMBER(" + key + ")"
    return formatStr


#解析json格式化定义
def PraseJson(pyObj):
    praseStr = ""
    for key in pyObj.keys():
        if type(pyObj[key]) == unicode and type(pyObj[key]) == unicode:
            praseStr += "\n\t\tPRASE_JSON_STRING(" + key + ")"
        elif type(pyObj[key]) == int:
            praseStr += "\n\t\tPRASE_JSON_INT(" + key + ")"
        elif type(pyObj[key]) == dict:
            praseStr += "\n\t\tPRASE_JSON_OBJECT(" + key + ")"
        elif type(pyObj[key]) == list:
            if type(pyObj[key][0]) == unicode and type(pyObj[key][0]) == unicode:
                praseStr += "\n\t\tPRASE_JSON_STRING_LIST(" + key + ")"
            elif type(pyObj[key][0]) == int:
                praseStr += "\n\t\tPRASE_JSON_INT_LIST(" + key + ")"
            elif type(pyObj[key][0]) == dict:
                praseStr += "\n\t\tPRASE_JSON_OBJECT_LIST(" + key + ", C" + key.capitalize() + ")"
            elif type(pyObj[key][0]) == True and type(pyObj[key][0]) == False:
                praseStr += "\n\t\tPRASE_JSON_BOOL_LIST(" + key + ", C" + key.capitalize() + ")"
        elif type(pyObj[key]) == bool:
            praseStr += "\n\t\tPRASE_JSON_BOOL(" + key + ")"
    return praseStr


#生成对象
def GenerateObj(name, pyObj):
    objClassName = "class C" + name.capitalize()
    #对象已经存在定义
    if defStr.find(objClassName) != -1:
        return ""

    obj = "class C" + name.capitalize() + "\n{\npublic:"
    # json编解码宏开始
    if generateFormat == 1:
        obj += "\n\tFORMAT_JSON_SUB_OBJECT_BEGIN"
        obj += FormatJson(pyObj)
        obj += "\n\t" + "FORMAT_JSON_SUB_OBJECT_END;"
    elif generateFormat == 2:
        obj += "\n\tPRASE_JSON_SUB_OBJECT_BEGIN"
        obj += PraseJson(pyObj)
        obj += "\n\t" + "PRASE_JSON_SUB_OBJECT_END;"
    elif generateFormat == 3:
        obj += "\n\tFORMAT_JSON_SUB_OBJECT_BEGIN"
        obj += FormatJson(pyObj)
        obj += "\n\t" + "FORMAT_JSON_SUB_OBJECT_END;\n"
        obj += "\n\tPRASE_JSON_SUB_OBJECT_BEGIN"
        obj += PraseJson(pyObj)
        obj += "\n\t" + "PRASE_JSON_SUB_OBJECT_END;"
    # json编解码宏结束

    obj += "\n\npublic:"
    for key in pyObj.keys():
        if type(pyObj[key]) == unicode and type(pyObj[key]) == unicode:
            obj += "\n\tQYString m_" + key + ";"
        elif type(pyObj[key]) == int:
            obj += "\n\tint m_" + key + ";"
        elif type(pyObj[key]) == dict:
            obj += "\n\tC" + key.capitalize() + " m_" + key + ";"
            objStr = GenerateObj(key, pyObj[key])
            obj = objStr + obj
        elif type(pyObj[key]) == list:
            name, objStr = GenerateList(key, pyObj[key])
            obj = objStr + obj
            obj += "\n\tstd::list<" + name + "> m_" + key + ";"
        elif type(pyObj[key]) == bool :
            obj += "\n\tbool m_" + key + ";"
    obj += "\n};\n\n"
    return obj

#列表生成
def GenerateList(name, listObj):
    listName = ""
    obj = ""
    if type(listObj[0]) == unicode and type(listObj[0]) == unicode:
        listName = "QYString"
    elif type(listObj[0]) == int:
        listName = "int"
    elif type(listObj[0]) == dict:
        listName = "C" + name.capitalize()
        objClassName = "class C" + name.capitalize()
        if defStr.find(objClassName) == -1:
            obj = GenerateObj(name, listObj[0])
    elif type(listObj[0]) == list:
        print "list in list is error!"
    elif type(listObj[0]) == bool :
        listName = "bool"

    return listName, obj




#生成代码
#name:类名，jsonStr:json串
def GenerateDefine(jsonStr):
    global defStr
    pyObj = json.loads(jsonStr)
    defStr = "class C" + className.capitalize() + "\n{\npublic:"

    #json编解码宏开始
    if generateFormat == 1:
        defStr += "\n\tFORMAT_JSON_OBJECT_BEGIN"
        defStr += FormatJson(pyObj)
        defStr += "\n\t" + "FORMAT_JSON_OBJECT_END;"
    elif generateFormat == 2:
        defStr += "\n\tPRASE_JSON_OBJECT_BEGIN"
        defStr += PraseJson(pyObj)
        defStr += "\n\t" + "PRASE_JSON_OBJECT_END;"
    elif generateFormat == 3:
        defStr += "\n\tFORMAT_JSON_SUB_OBJECT_BEGIN"
        defStr += FormatJson(pyObj)
        defStr += "\n\t" + "FORMAT_JSON_SUB_OBJECT_END;\n"
        defStr += "\n\tPRASE_JSON_SUB_OBJECT_BEGIN"
        defStr += PraseJson(pyObj)
        defStr += "\n\t" + "PRASE_JSON_SUB_OBJECT_END;"
    else:
        print "Error generateFormat:", generateFormat
        return
    #json编解码宏结束

    #定义成员变量
    defStr += "\n\npublic:"
    for key in pyObj.keys():
        if type(pyObj[key]) == unicode and type(pyObj[key]) == unicode:
            defStr += "\n\tQYString m_" + key + ";"
        elif type(pyObj[key]) == int:
            defStr += "\n\tint m_" + key + ";"
        elif type(pyObj[key]) == dict:
            defStr += "\n\tC" + key.capitalize() + " m_" + key + ";"
            objStr = GenerateObj(key, pyObj[key])
            defStr = objStr + defStr
        elif type(pyObj[key]) == list:
            name, objStr = GenerateList(key, pyObj[key])
            defStr = objStr + defStr
            defStr += "\n\tstd::list<" + name + "> m_" + key + ";"
        elif type(pyObj[key]) == bool:
            defStr += "\n\tbool m_" + key + ";"

    #定义结束符
    defStr += "\n}"
    return defStr


if __name__ == '__main__':
    GenerateDefine(jsonData)
    headerStr = "#pragma once\n\n#include \"sys/json_format.h\"\n#include \"sys/common.h\"\n\n"
    outputStr = headerStr + defStr
    print outputStr