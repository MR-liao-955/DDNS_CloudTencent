

// curl -X POST https://dnsapi.cn/Record.Info -d 'login_token=LOGIN_TOKEN&format=json&domain_id=2317346&record_id=16894439'
#include "custom_cloudTencent.hpp"


#include "config.h"
#include "tretap_cJSON.h"

using namespace std;

#pragma  once
// curl 写回调，用于接收服务器返回的数据
static size_t record_info_write_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    string *output = static_cast<string *>(userp);
    output->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

int cloudTencent_dnspod::set_ipv4_dns_record(string sub_domain)
{

}

int cloudTencent_dnspod::set_ipv6_dns_record(string sub_domain)
{

}

/*
 * 腾讯云 DNSPod 获取域名解析记录信息
 *
 * API 文档: https://cloud.tencent.com/document/product/1427/56163
 *
 * 请求参数:
 *   domain_id 或 domain  — 分别对应域名ID和域名, 提交其中一个即可
 *   record_id            — 记录ID
 *   login_token          — 登录凭证
 *   format               — 返回格式 (json/xml)
 *
 * 响应码:
 *   6   — 域名ID错误
 *   7   — 记录开始的偏移无效、非域名所有者
 *   8   — 域名无效
 *   13  — 当前域名有误，请返回重新操作
 *
 * 返回值:
 *   成功 — 返回服务器响应的 JSON 字符串（std::string，自动管理内存，无泄漏风险）
 *   失败 — 返回空字符串
 */

/*
curl -X POST 'https://dnsapi.cn/Record.Info' \
-d 'login_token=644952,cb861cd07897bca0d826f97a96f2b0b7&format=json&domain_id=2317346&record_id=644952'
*/

/*
查询 domain_id
curl -X POST 'https://dnsapi.cn/Domain.List' \
  -d 'login_token=644952,cb861cd07897bca0d826f97a96f2b0b7&format=json'
*/

string cloudTencent_dnspod::get_record_info()
{
    CURL *curl = NULL;
    CURLcode res;
    string response_data;          // 存放服务器返回的原始数据
    string post_fields;            // POST 请求体
    struct curl_slist *headers = NULL;

    // ---------- 1. 参数校验 ----------
    if (domain_id == 0 || record_id == 0)
    {
        fprintf(stderr, "[%s] ERROR: domain_id or record_id is zero, please set them first.\n", __func__);
        return "";
    }

    // ---------- 2. 拼接 POST 参数 ----------
    // 格式: login_token=LOGIN_TOKEN&format=json&domain_id=xxx&record_id=xxx
    char buf[512] = {0};
    snprintf(buf, sizeof(buf),
             "login_token=%s&format=json&domain_id=%u&record_id=%u",
             LOGIN_TOKEN,
             domain_id,
             record_id);
    post_fields = string(buf);

    printf("[%s] POST URL : %s\n", __func__, TENCENT_DNSPOD_URL);
    printf("[%s] POST DATA: %s\n", __func__, post_fields.c_str());

    // ---------- 3. 初始化 curl ----------
    curl = curl_easy_init();
    if (!curl)
    {
        fprintf(stderr, "[%s] ERROR: curl_easy_init() failed.\n", __func__);
        return "";
    }

    // ---------- 4. 设置 curl 选项 ----------
    curl_easy_setopt(curl, CURLOPT_URL, TENCENT_DNSPOD_URL);          // 请求 URL
    curl_easy_setopt(curl, CURLOPT_POST, 1L);                          // 使用 POST 方法
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());   // POST 请求体
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_fields.size()); // 请求体大小

    // 设置 HTTP 头
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // 设置写回调，接收响应数据
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, record_info_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // 设置超时（可选）
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);                      // 总超时 30 秒
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);               // 连接超时 10 秒

    // ---------- 5. 执行请求 ----------
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "[%s] ERROR: curl_easy_perform() failed: %s\n",
                __func__, curl_easy_strerror(res));
        goto cleanup;
    }

    // ---------- 6. 检查 HTTP 响应码 ----------
    {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        printf("[%s] HTTP response code: %ld\n", __func__, http_code);

        if (http_code != 200)
        {
            fprintf(stderr, "[%s] ERROR: HTTP %ld, expected 200.\n", __func__, http_code);
            goto cleanup;
        }
    }

    // ---------- 7. 打印结果 ----------
    printf("[%s] Response body:\n%s\n", __func__, response_data.c_str());

    record_info_decode(response_data);
cleanup:
    // ---------- 8. 清理资源 ----------
    if (headers)
        curl_slist_free_all(headers);
    if (curl)
        curl_easy_cleanup(curl);

    return response_data;  // std::string 自动管理内存，不会泄漏
}

void cloudTencent_dnspod::record_info_decode(string info)
{
    /*
    腾讯云 DNSPod Record.Info API 返回的 JSON 格式:

    {
        "status": { "code": "1", "message": "...", "created_at": "..." },
        "domain": {
            "id": "9842292",       ← 域名ID
            "domain": "yizerowu.com"  ← 域名
        },
        "record": {
            "id": "44146112",       ← 子域名记录ID
            "sub_domain": "yizerowwwww"  ← 子域名
        }
    }
    */

    // ---------- 1. 解析 JSON ----------
    tretap_CJson *root = tretap_CJson_Parse(info.c_str());
    if (!root)
    {
        fprintf(stderr, "[%s] ERROR: JSON parse failed.\n", __func__);
        return;
    }

    // ---------- 2. 检查 status.code ----------
    tretap_CJson *status = tretap_CJson_GetObjectItem(root, "status");
    if (!status)
    {
        fprintf(stderr, "[%s] ERROR: missing 'status' field.\n", __func__);
        tretap_CJson_Delete(root);
        return;
    }

    tretap_CJson *code = tretap_CJson_GetObjectItem(status, "code");
    if (!code || !code->valuestring)
    {
        fprintf(stderr, "[%s] ERROR: missing 'status.code' field.\n", __func__);
        tretap_CJson_Delete(root);
        return;
    }

    if (strcmp(code->valuestring, "1") != 0)
    {
        tretap_CJson *msg = tretap_CJson_GetObjectItem(status, "message");
        fprintf(stderr, "[%s] ERROR: API returned code=%s, message=%s\n",
                __func__,
                code->valuestring,
                msg && msg->valuestring ? msg->valuestring : "unknown");
        tretap_CJson_Delete(root);
        return;
    }
    printf("[%s] status.code = %s, success.\n", __func__, code->valuestring);

    // ---------- 3. 解析 domain 信息 ----------
    tretap_CJson *domain_json = tretap_CJson_GetObjectItem(root, "domain");
    if (!domain_json)
    {
        fprintf(stderr, "[%s] ERROR: missing 'domain' field.\n", __func__);
        tretap_CJson_Delete(root);
        return;
    }

    // 域名ID
    tretap_CJson *domain_id_json = tretap_CJson_GetObjectItem(domain_json, "id");
    if (domain_id_json && domain_id_json->valuestring)
    {
        parsed_domain_id = (uint32_t)atoi(domain_id_json->valuestring);
        printf("[%s] domain.id   = %u\n", __func__, parsed_domain_id);
    }
    else
    {
        fprintf(stderr, "[%s] WARNING: missing 'domain.id' field.\n", __func__);
    }

    // 域名
    tretap_CJson *domain_name_json = tretap_CJson_GetObjectItem(domain_json, "domain");
    if (domain_name_json && domain_name_json->valuestring)
    {
        domain_name = string(domain_name_json->valuestring);
        printf("[%s] domain.name = %s\n", __func__, domain_name.c_str());
    }
    else
    {
        fprintf(stderr, "[%s] WARNING: missing 'domain.domain' field.\n", __func__);
    }

    // ---------- 4. 解析 record 信息 ----------
    tretap_CJson *record_json = tretap_CJson_GetObjectItem(root, "record");
    if (!record_json)
    {
        fprintf(stderr, "[%s] ERROR: missing 'record' field.\n", __func__);
        tretap_CJson_Delete(root);
        return;
    }

    sub_domain_count = 0;

    // 子域名记录ID
    tretap_CJson *record_id_json = tretap_CJson_GetObjectItem(record_json, "id");
    uint32_t parsed_record_id = 0;
    if (record_id_json && record_id_json->valuestring)
    {
        parsed_record_id = (uint32_t)atoi(record_id_json->valuestring);
        printf("[%s] record.id = %u\n", __func__, parsed_record_id);
    }

    // 子域名
    tretap_CJson *sub_domain_json = tretap_CJson_GetObjectItem(record_json, "sub_domain");
    string parsed_sub_domain;
    if (sub_domain_json && sub_domain_json->valuestring)
    {
        parsed_sub_domain = string(sub_domain_json->valuestring);
        printf("[%s] record.sub_domain = %s\n", __func__, parsed_sub_domain.c_str());
    }

    // 存入数组
    if (sub_domain_count < 5)
    {
        sub_domain_list[sub_domain_count].id   = parsed_record_id;
        sub_domain_list[sub_domain_count].name = parsed_sub_domain;
        sub_domain_count++;
    }

    // ---------- 5. 释放 JSON ----------
    tretap_CJson_Delete(root);

    printf("[%s] Parse complete. domain_id=%u, domain=%s, records=%d\n",
           __func__, parsed_domain_id, domain_name.c_str(), sub_domain_count);
}
