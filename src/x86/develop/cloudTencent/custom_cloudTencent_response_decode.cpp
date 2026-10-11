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

    string temp = g_file_config.domain_name;

    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(root, "Response"), second);
    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(second, "DomainList"), third);
    if(third->type == tretap_CJson_Array){
        for( int i = 0; i < tretap_CJson_GetArraySize(third); i++ ){

            tretap_CJson *domain = tretap_CJson_GetArrayItem(third, i);
            // 判断 "Name" 字段是否和当前域名匹配
            FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Name"), fourth);
            if(fourth->type == tretap_CJson_String
                && fourth->valuestring
                && strcmp(fourth->valuestring, temp.c_str()) == 0)
            {
                    DEBUG_PRINT("domain: %s\n", g_file_config.domain_full.c_str());
                    // 保存 DomainID
                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "DomainId"), fourth);
                    dnspod->domain_id = static_cast<uint64_t>(fourth->valueint);
                    DEBUG_PRINT("DomainId=%llu\n", (unsigned long long)dnspod->domain_id);

                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Grade"), fourth);
                    dnspod->domain_grade = fourth->valuestring;
                    DEBUG_PRINT("Grade: %s\n", dnspod->domain_grade.data());
            }
        }
    }
    tretap_CJson_Delete(root);
    root = NULL;
    return true;
}



/**
 * @brief: JSON 解析函数，解析腾讯云返回的记录列表
 *
 */
bool tencentcloud_decode_record_list_filter(string src, cloudTencent_dnspod *dnspod)
{
    tretap_CJson *root;
    tretap_CJson *second;
    tretap_CJson *third;
    tretap_CJson *fourth;
    tretap_CJson *fifth;
    record_list_t record_id;

    root = tretap_CJson_Parse(src.c_str());
    if (!root) { fprintf(stderr, "[%s] JSON parse failed.\n", __func__); return false; }

    string temp = g_file_config.domain_subname.empty()? "@": g_file_config.domain_subname.c_str();

    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(root, "Response"), second);
    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(second, "RecordList"), third);
    if(third->type == tretap_CJson_Array){
        for( int i = 0; i < tretap_CJson_GetArraySize(third); i++ ){

            tretap_CJson *domain = tretap_CJson_GetArrayItem(third, i);
            // 判断 "Name" 字段是否和当前域名匹配
            FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Name"), fourth);
            if(fourth->type == tretap_CJson_String
                && fourth->valuestring
                && strcmp(fourth->valuestring, temp.c_str()) == 0)
            {
                // A 记录
                FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Type"), fourth);

                if(strcmp(fourth->valuestring, "A") == 0){
                    record_id.id = 0;
                    record_id.value.clear();
                    record_id.type = "A";
                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Value"), fifth);
                    record_id.value= fifth->valuestring;
                    DEBUG_PRINT("A record: %s\n", record_id.value.data());

                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "RecordId"), fifth);
                    record_id.id = static_cast<uint64_t>(fifth->valuedouble);
                    DEBUG_PRINT("A record ID: %llu\n", (unsigned long long)record_id.id);
                    dnspod->record_ipv4.push_back(record_id);
                }

                // AAAA 记录
                if(strcmp(fourth->valuestring, "AAAA") == 0){
                    record_id.id = 0;
                    record_id.value.clear();
                    record_id.type = "AAAA";
                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "Value"), fifth);
                    record_id.value = fifth->valuestring;
                    DEBUG_PRINT("AAAA record: %s\n", record_id.value.c_str());

                    FUNC_JSON_CHECK(tretap_CJson_GetObjectItem(domain, "RecordId"), fifth);
                    record_id.id = static_cast<uint64_t>(fifth->valuedouble);
                    DEBUG_PRINT("AAAA record ID: %llu\n", (unsigned long long)record_id.id);
                    dnspod->record_ipv6.push_back(record_id);
                }
            }
        }
    }

    for(auto& record : dnspod->record_ipv4){
        DEBUG_PRINT("type: %s, recordid: %llu, value: %s\n", record.type.c_str(), (unsigned long long)record.id, record.value.c_str());
    }

    for(auto& record : dnspod->record_ipv6){
        DEBUG_PRINT("type: %s, recordid: %llu, value: %s\n", record.type.c_str(), (unsigned long long)record.id, record.value.c_str());
    }

    tretap_CJson_Delete(root);
    root = NULL;
    return true;
}
