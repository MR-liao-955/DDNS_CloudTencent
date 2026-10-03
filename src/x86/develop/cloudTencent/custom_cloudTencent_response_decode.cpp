#include "custom_cloudTencent_response_decode.hpp"
#include "tretap_cJSON.h"

#define FUNC_JSON_CHECK(func, r) \
    do{ \
        r = func; \
        if(!r) {\
            printf("[%s: %d] fail \n", #func, __LINE__);\
            return false; \
         }\
    }while(0);


/**
 * @brief: JSON 解析函数，解析腾讯云返回的域名列表
 *
 *
 */
bool tencentcloud_decode_domain_list(string src, cloudTencent_dnspod *dnspod)
{
    tretap_CJson *root;
    tretap_CJson *second;
    tretap_CJson *third;
    tretap_CJson *fourth;

    root = tretap_CJson_Parse(src.c_str());
    if (!root) { fprintf(stderr, "[%s] JSON parse failed.\n", __func__); return false; }

    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(root, "Response"), second);
    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(second, "DomainList"), third);
    if(third->type == tretap_CJson_Array){
        for( int i = 0; i < tretap_CJson_GetArraySize(third); i++ ){

            tretap_CJson *domain = tretap_CJson_GetArrayItem(third, i);
            // 判断 "Name" 字段是否和当前域名匹配
            FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Name"), fourth);
            if(fourth->type == tretap_CJson_String
                && fourth->valuestring
                && strcmp(fourth->valuestring, dnspod->domain_name.c_str()) == 0)
            {
                    printf("[%s] domain: %s\n", __func__, dnspod->domain_name.c_str());
                    // 保存 DomainID
                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "DomainId"), fourth);
                    dnspod->parsed_domain_id = fourth->valueint;
                    printf("[%s] DomainId=%u\n", __func__, dnspod->domain_name.c_str(), dnspod->parsed_domain_id);

                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Grade"), fourth);
                    dnspod->domain_grade = fourth->valuestring;
                    printf("[%s] Grade: %s\n", __func__, dnspod->domain_grade.data());
            }
        }
    }
    tretap_CJson_Delete(root);
    root = NULL;
    return true;
}
