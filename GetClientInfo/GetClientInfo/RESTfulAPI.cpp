#include "stdafx.h"
#include "RESTfulAPI.h"
#include "Protection.h"

#include "../../Util/curl/curl.h"

CRESTfulAPI gRESTfulAPI;

CRESTfulAPI::CRESTfulAPI()
{
}

size_t CRESTfulAPI::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

string CRESTfulAPI::Request(API_METHOD m_method, string m_URL, string m_Data, bool m_isToken)
{
    VMProtectBeginUltra("CRESTfulAPI: Request");
    // INIT URL
    CURL *curl;
    CURLcode res;
    string m_Res;
    // GET API
    curl = curl_easy_init();
    if (curl)
    {

        curl_easy_setopt(curl, CURLOPT_URL, m_URL.c_str());
        if (m_isToken)
        {
            // Authorization
            curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, gProtection.m_AccessToken);
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
        }

        switch (m_method)
        {
        case GET_METHOD:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
            break;
        case POST_METHOD:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
            break;
        case PUT_METHOD:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            break;
        case DELETE_METHOD:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        default:
            break;
        }

        if (m_Data.length() > 0)
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_Data.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_Res);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    VMProtectEnd();

    return m_Res;
}