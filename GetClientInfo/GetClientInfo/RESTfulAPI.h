#pragma once

enum API_METHOD
{
    GET_METHOD = 0,
    POST_METHOD = 1,
    PUT_METHOD = 2,
    DELETE_METHOD = 3,
};

class CRESTfulAPI
{
public:
    CRESTfulAPI();
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    string Request(API_METHOD m_method, string m_URL, string m_Data, bool m_isToken);
	//
private:
    string m_urlAPI;
};
extern CRESTfulAPI gRESTfulAPI;