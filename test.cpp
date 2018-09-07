#include <list>
#include <iostream>
#include "json_format.h"

class CObject
{
public:
	FORMAT_JSON_SUB_OBJECT_BEGIN
		FORMAT_JSON_NUMBER(a)
		FORMAT_JSON_NUMBER(b)
	FORMAT_JSON_SUB_OBJECT_END

	PRASE_JSON_SUB_OBJECT_BEGIN
		PRASE_JSON_INT(a)
		PRASE_JSON_STRING(b)
	PRASE_JSON_SUB_OBJECT_END

public:
	void SetValue(int var, const std::string &rStr)
	{
		m_a = var;
		m_b = rStr;
	}
public:
	int m_a;
	std::string m_b;
};


class CReq
{
public:
	FORMAT_JSON_OBJECT_BEGIN
		FORMAT_JSON_NUMBER(int1)
		FORMAT_JSON_NUMBER(double1)
		FORMAT_JSON_STRING(string1)
		FORMAT_JSON_BOOL(bool1)
		FORMAT_JSON_LIST(listint1)
		FORMAT_JSON_OBJECT(obj1)
		FORMAT_JSON_OBJECT_LIST(listObj, CObject)
	FORMAT_JSON_OBJECT_END

	PRASE_JSON_OBJECT_BEGIN
		PRASE_JSON_INT(int1)
		PRASE_JSON_DOUBLE(double1)
		PRASE_JSON_STRING(string1)
		PRASE_JSON_INT_LIST(listint1)
		PRASE_JSON_BOOL(bool1)
		PRASE_JSON_OBJECT(obj1)
		PRASE_JSON_OBJECT_LIST(listObj, CObject)
	PRASE_JSON_OBJECT_END
public:
	int m_int1;
	double m_double1;
	std::string m_string1;
	std::list<int> m_listint1;
	bool m_bool1;
	CObject m_obj1;
	std::list<CObject> m_listObj;
};

int main(int argc, char *argv[])
{
	CReq req;
	req.m_int1 = 69;
	req.m_bool1 = true;
	req.m_double1 = 3.14;
	req.m_string1 = "this is a json test.";
	req.m_listint1.push_back(2);
	req.m_listint1.push_back(3);
	req.m_obj1.SetValue(222, "this is a test.");
	CObject obj;
	obj.SetValue(1234, "first one");
	req.m_listObj.push_back(obj);
	obj.SetValue(2345, "second");
	req.m_listObj.push_back(obj);
	obj.SetValue(3456, "thisd");
	req.m_listObj.push_back(obj);
	std::string tt = req.FormatJsonToString();
	std::cout << "format json:\n" << tt.c_str() << std::endl;

	std::cout << "\nprase formatted json:\n" << tt.c_str() << std::endl;
	CReq rsp1;
	rsp1.PraseJsonString(tt);
	std::cout << "m_int1 = " << rsp1.m_int1 << std::endl;
	std::cout << "m_double1 = " << rsp1.m_double1 << std::endl;
	std::cout << "m_string1 = " << rsp1.m_string1.c_str() << std::endl;
	std::cout << "m_bool1 = " << rsp1.m_bool1 << std::endl;
	std::cout << "m_obj1.m_a = " << rsp1.m_obj1.m_a << std::endl;
	std::cout << "m_obj1.m_b = " << rsp1.m_obj1.m_b.c_str() << std::endl;
	std::cout << "list::\n\n\n";
	for (auto it = rsp1.m_listObj.begin(); it != rsp1.m_listObj.end(); ++it)
		std::cout << "m_obj1.m_a = " << it->m_a << ", m_obj1.m_b = " << it->m_b.c_str() << std::endl;

	CReq rsp;
	tt = "{\"int1\":69, \"double1\":3.1415926, \"string1\":\"test\", \"bool1\":true, \"listint1\":[2,3], \"obj1\":{\"a\":111, \"b\":\"test obj b\"}, \"listObj\":[{\"a\":111, \"b\":\"1111111\"}, {\"a\":222, \"b\":\"2222222222\"}, {\"a\":333, \"b\":\"33333333\"}]}";
	std::cout << "\n\n\n\nprase json:\n" << tt.c_str() << std::endl;
	rsp.PraseJsonString(tt);
	std::cout << "m_int1 = " << rsp.m_int1 << std::endl;
	std::cout << "m_double1 = " << rsp.m_double1 << std::endl;
	std::cout << "m_string1 = " << rsp.m_string1.c_str() << std::endl;
	std::cout << "m_bool1 = " << rsp.m_bool1 << std::endl;
	std::cout << "m_obj1.m_a = " << rsp.m_obj1.m_a << std::endl;
	std::cout << "m_obj1.m_b = " << rsp.m_obj1.m_b.c_str() << std::endl;
	std::cout << "list::\n\n\n";
	for (auto it = rsp.m_listObj.begin(); it != rsp.m_listObj.end(); ++it)
		std::cout << "m_obj1.m_a = " << it->m_a << ", m_obj1.m_b = " << it->m_b.c_str() << std::endl;
	return 0;
}

