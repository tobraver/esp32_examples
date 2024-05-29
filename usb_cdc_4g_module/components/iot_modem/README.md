# 组件使用

将 `iot_modem` 添加到工程目录的components中。


# 组件配置

`EC600NCNLA-N05` 为示例。

```
idf.py menuconfig
    Component config
        IoT USB Host
        IoT USB Host CDC
        IoT USB Host Modem
            Choose Modem Board (EC600NCNLA-N05)
            (5) Retry Times During Dial-up
            (internet) Set Modem APN
            (1234) Set SIM PIN
            GPIO Config
                (0) Modem power control gpio, set 0 if not use 
                (11) Modem reset control gpio, set 0 if not use
                (0) Power control gpio inactive level
                (1) Reset control gpio inactive level 
```

# 组件逻辑

`iot_modem` 依赖于 `iot_usbh_modem` `iot_usbh_cdc` `iot_usbh`

其中 `iot_usbh_modem` 来自于 `esp-iot-solution/master/components/usb/iot_usbh_modem`

其中 `iot_usbh_cdc` 来自于 `esp-iot-solution/master/components/usb/iot_usbh_cdc`

其中 `iot_usbh` 来自于 `esp-iot-solution/master/components/usb/iot_usbh`


# 组件移植

- `iot_usbh` 修改内容

1. 修改 idf_component.yml

```
version: "0.2.1"
targets:
  - esp32s2
  - esp32s3
dependencies:
  idf: ">=4.4.1"
```

2. 修改 iot_usbh.h, 增加宏定义

```
// @ref idf_component.yml -> version
#define IOT_USBH_VER_MAJOR 0 /*!< Major version number */
#define IOT_USBH_VER_MINOR 2 /*!< Minor version number */
#define IOT_USBH_VER_PATCH 1 /*!< Patch version number */
```

- `iot_usbh_cdc` 修改内容

1. 修改 idf_component.yml

```
version: "0.2.2"
targets:
  - esp32s2
  - esp32s3
description: Simple USB Host CDC driver maintained in esp-iot-solution
dependencies:
  idf: ">=4.4.1"
  iot_usbh:
    version: "0.*"
    public: true
    override_path: "../iot_usbh"
```

2. 修改 iot_usbh_cdc.h, 增加宏定义

```
// @ref idf_component.yml -> version
#define IOT_USBH_CDC_VER_MAJOR 0 /*!< Major version number */
#define IOT_USBH_CDC_VER_MINOR 2 /*!< Minor version number */
#define IOT_USBH_CDC_VER_PATCH 2 /*!< Patch version number */
```

- `iot_usbh_modem` 修改内容

1. 修改 idf_component.yml

```
version: "0.2.1"
targets:
  - esp32s2
  - esp32s3
description: esp-iot-solution USB Host Modem driver
dependencies:
  idf: ">=4.4.1"
  iot_usbh_cdc:
    version: "0.*"
    override_path: "../iot_usbh_cdc"
```

2. 修改 esp_modem.h, 增加宏定义

```
// @ref idf_component.yml -> version
#define IOT_USBH_MODEM_VER_MAJOR 0 /*!< Major version number */
#define IOT_USBH_MODEM_VER_MINOR 2 /*!< Minor version number */
#define IOT_USBH_MODEM_VER_PATCH 1 /*!< Patch version number */
```

3. 修改 esp_modem_dce_common_commands.c, 增加 ICCID指令处理

```
/**
 * @brief Handle response from AT+QCCID
 */
static esp_err_t esp_modem_dce_common_handle_qccid(esp_modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, ESP_MODEM_STATE_FAIL);
        return err;
    }

    if (strstr(line, "+QCCID")) {
        common_string_t *result_str = dce->handle_line_ctx;
        assert(result_str->string != NULL && result_str->len != 0);
        char fmt_str[32] = { 0 };
        snprintf(fmt_str, sizeof(fmt_str), "%%*s%%%ds", result_str->len -1);
        /* +QCCID: <ICCID> */
        sscanf(strstr(line, "+QCCID"), fmt_str, result_str->string);
        err = ESP_OK;
    }

    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, ESP_MODEM_STATE_SUCCESS);
    }
    return err;
}

esp_err_t esp_modem_dce_get_unique_sim_number(esp_modem_dce_t *dce, void *param, void *result)
{
    common_string_t common_str = { .command = "AT+QCCID\r", .string = result, .len = (size_t)param };
    return esp_modem_dce_generic_command(dce, common_str.command, MODEM_COMMAND_TIMEOUT_OPERATOR,
                                         esp_modem_dce_common_handle_qccid, &common_str);
}

```

4. 修改 usbh_modem_board.c, 增加对ICCID处理、IP、运营商信息的缓存

```
case STAGE_CHECK_SIM:
    if (_check_sim_card() != true) {
        retry_after_ms = 3000;
        ++stage_retry_times;
    } else {
        modem_stage = STAGE_CHECK_ICCID;
        goto _stage_succeed;
    }
    break;
case STAGE_CHECK_ICCID:
    if (_check_sim_number() != true) {
        retry_after_ms = 3000;
        ++stage_retry_times;
    } else {
        modem_stage = STAGE_CHECK_SIGNAL;
        goto _stage_succeed;
    }
    break;
```

```
static bool _check_sim_number()
{
    char iccid[64] = "";
    if (modem_board_get_unique_sim_number(iccid, sizeof(iccid)) == ESP_OK) {
        if (strlen(iccid) > 0) {
            ESP_LOGI(TAG, "SIM number: %s", iccid);
            snprintf(s_modem_sim_number, sizeof(s_modem_sim_number), "%s", iccid);
            return true;
        } else {
            ESP_LOGW(TAG, "SIM number is not find");
            return false;
        }
    }
    ESP_LOGW(TAG, "Get sim number failed");
    return false;
}

esp_err_t modem_board_get_unique_sim_number(char *buf, size_t buf_size)
{
    MODEM_CHECK(buf != NULL && buf_size != 0, "arg can not be NULL", ESP_ERR_INVALID_ARG);
    MODEM_CHECK(s_dce != NULL, "modem not ready", ESP_ERR_INVALID_STATE);
    return esp_modem_dce_get_unique_sim_number(s_dce, (void *)buf_size, (void *)buf);
}
```

```
static esp_modem_dce_csq_ctx_t s_modem_single = {0, 0};
static char s_modem_sim_number[64] = { 0 };
static char s_modem_sim_name[64] = { 0 };
static esp_netif_ip_info_t s_modem_ip_info = { 0 };

esp_err_t modem_get_sim_signal(int *rssi, int *ber)
{
    *rssi = s_modem_single.rssi;
    *ber = s_modem_single.ber;
    return ESP_OK;
}

esp_err_t modem_get_sim_name(char *buf, size_t buf_size)
{
    if(strlen(s_modem_sim_name)) {
        snprintf(buf, buf_size, "%s", s_modem_sim_name);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t modem_get_sim_number(char *buf, size_t buf_size)
{
    if(strlen(s_modem_sim_number)) {
        snprintf(buf, buf_size, "%s", s_modem_sim_number);
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t modem_get_sim_ip_info(char *buf, size_t buf_size)
{
    if(s_modem_ip_info.ip.addr) {
        snprintf(buf, buf_size, IPSTR, IP2STR(&s_modem_ip_info.ip));
        return ESP_OK;
    }
    return ESP_FAIL;
}
```
