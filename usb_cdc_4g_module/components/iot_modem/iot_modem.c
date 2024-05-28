#include "iot_modem.h"
#include "usbh_modem_board.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"

const char* TAG = "iot_modem";
static int s_iot_moden_init = 0;

static void iot_modem_event(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    if (event_base == MODEM_BOARD_EVENT) {
        if (event_id == MODEM_EVENT_SIMCARD_DISCONN) {
            ESP_LOGW(TAG, "Modem Board Event: SIM Card disconnected");
        } else if (event_id == MODEM_EVENT_SIMCARD_CONN) {
            ESP_LOGI(TAG, "Modem Board Event: SIM Card Connected");
        } else if (event_id == MODEM_EVENT_DTE_DISCONN) {
            ESP_LOGW(TAG, "Modem Board Event: USB disconnected");
        } else if (event_id == MODEM_EVENT_DTE_CONN) {
            ESP_LOGI(TAG, "Modem Board Event: USB connected");
        } else if (event_id == MODEM_EVENT_DTE_RESTART) {
            ESP_LOGW(TAG, "Modem Board Event: Hardware restart");
        } else if (event_id == MODEM_EVENT_DTE_RESTART_DONE) {
            ESP_LOGI(TAG, "Modem Board Event: Hardware restart done");
        } else if (event_id == MODEM_EVENT_NET_CONN) {
            ESP_LOGI(TAG, "Modem Board Event: Network connected");
        } else if (event_id == MODEM_EVENT_NET_DISCONN) {
            ESP_LOGW(TAG, "Modem Board Event: Network disconnected");
        } else if (event_id == MODEM_EVENT_WIFI_STA_CONN) {
            ESP_LOGI(TAG, "Modem Board Event: Station connected");
        } else if (event_id == MODEM_EVENT_WIFI_STA_DISCONN) {
            ESP_LOGW(TAG, "Modem Board Event: All stations disconnected");
        }
    }
}

/**
 * @brief iot modem init
 * 
 * @return int 
 *  @retval 0 success
 *  @retval -1 failed
 */
int iot_modem_init(void)
{
    if(s_iot_moden_init) {
        ESP_LOGW(TAG, "iot modem already init");
        return 0;
    }

    /* Initialize default TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Waiting for modem powerup */
    ESP_LOGI(TAG, "====================================");
    ESP_LOGI(TAG, "            ESP 4G Cat.1            ");
    ESP_LOGI(TAG, "====================================");

    /* Initialize modem board. Dial-up internet */
    modem_config_t modem_config = MODEM_DEFAULT_CONFIG();
    /* Modem init flag, used to control init process */
#ifndef CONFIG_EXAMPLE_ENTER_PPP_DURING_INIT
    /* if Not enter ppp, modem will enter command mode after init */
    modem_config.flags |= MODEM_FLAGS_INIT_NOT_ENTER_PPP;
    /* if Not waiting for modem ready, just return after modem init */
    modem_config.flags |= MODEM_FLAGS_INIT_NOT_BLOCK;
#endif
    modem_config.handler = iot_modem_event;
    esp_err_t err = modem_board_init(&modem_config);
    
    s_iot_moden_init = 1;
    if(err == ESP_OK) {
        ESP_LOGI(TAG, "iot modem int success");
    } else {
        ESP_LOGE(TAG, "iot modem int failed, %s", esp_err_to_name(err));
    }
    return (err == ESP_OK) ? 0 : -1;
}

int iot_modem_deinit(void)
{
    return 0;
}

void iot_modem_info(void)
{
    int rssi = 0, ber = 0;
    modem_get_sim_signal(&rssi, &ber);
    ESP_LOGI(TAG, "rssi: %d, ber: %d", rssi, ber);

    char buff[64] = { 0 };
    modem_get_sim_name(buff, sizeof(buff));
    ESP_LOGI(TAG, "sim name: %s", buff);

    modem_get_sim_number(buff, sizeof(buff));
    ESP_LOGI(TAG, "sim number: %s", buff);

    modem_get_sim_ip_info(buff, sizeof(buff));
    ESP_LOGI(TAG, "sim ip: %s", buff);
}
