/**
 * @file sntp.c
 * @author {pradip} ({pradipshrestha@machineertech.com})
 * @brief gets the local present time and date, converts timestamp into date and time
 * @version 0.1
 * @date 2021-09-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "time.h"
#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "sntp1.h"

#define WIFI_SSID "aayush777_2.4" //give your wifi ssid here
#define WIFI_PASS "CLEB12F512"    //gives your wifi password here

static EventGroupHandle_t wifi_event_group;

const int CONNECTED_BIT = BIT0;

static const char *TAG = "SNTP";
int ad_day, ad_year, ad_month, hr, min, sec;
int sey = 1943,
    sem = 4, sed = 14, engmonth, engyear, totaldayscount = 0, months, k, engdiff, i, j, nepdiff, engday;
int sny = 2000, snm = 1, snd = 1, endday = 0, var = 1, day = 1, iny, inm, ind;

int engdays[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31,
    30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
    30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31,
    30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31,
    30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31,
    28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,
    31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30,
    31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30,
    31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28,
    31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31,
    30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31,
    30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31,
    30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31,
    30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31,
    29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30,
    31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30,
    31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30,
    31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29,
    31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31,
    30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
    30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31,
    30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31,
    30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31,
    28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,
    31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30,
    31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30,
    31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28,
    31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29, 31, 30, 31,
    30, 31, 31, 30, 31, 30, 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int nepdays[] = {
    0, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31,
    31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31,
    30, 30, 30, 29, 29, 30, 31, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29,
    30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 31, 31, 31,
    32, 31, 31, 29, 30, 30, 29, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29,
    30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 31, 31, 31, 32, 31, 31, 29, 30, 30, 29, 30, 30, 31,
    31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30,
    30, 30, 29, 29, 30, 31, 31, 31, 31, 32, 31, 31, 29, 30, 30, 29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30,
    30, 31, 32, 31, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 31, 32,
    31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29,
    29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 31, 32, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31,
    32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 30, 32, 31, 32, 31, 30, 30,
    30, 29, 30, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 31, 32, 30, 30, 29, 30, 29, 30, 30,
    31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 32, 31, 31,
    31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29,
    30, 31, 30, 32, 31, 32, 31, 31, 29, 30, 30, 29, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32,
    32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 31, 31, 31, 32, 31, 31, 29, 30,
    30, 29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31,
    32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 31, 31, 31, 32, 31, 31, 29, 30, 30, 29, 30, 30, 31, 31, 32, 31, 31, 31,
    30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30,
    31, 31, 31, 31, 32, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32,
    31, 30, 30, 30, 29, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 31, 32, 31, 31, 30, 29, 30,
    29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 30, 31, 32,
    31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 31, 32, 30, 30,
    29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31,
    31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31,
    30, 30, 30, 29, 29, 30, 31, 30, 32, 31, 32, 31, 31, 29, 30, 29, 30, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29,
    30, 30, 31, 31, 32, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 31, 31, 31,
    32, 31, 31, 29, 30, 30, 29, 29, 31, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 29,
    30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 31, 31, 31, 31, 32, 31, 31, 29, 30, 30, 29, 30, 30, 31,
    31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30,
    30, 30, 29, 29, 30, 31, 31, 31, 31, 32, 31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30,
    30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29, 30, 29, 31, 31, 31, 31, 32,
    31, 31, 30, 29, 30, 29, 30, 30, 31, 31, 32, 31, 31, 31, 30, 29, 30, 29, 30, 30, 31, 32, 31, 32, 31, 30, 30, 30, 29,
    29, 30, 30, 31, 31, 32, 32, 31, 30, 30, 30, 29, 30, 30, 30, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 30, 30, 31, 31,
    32, 31, 31, 30, 30, 30, 29, 30, 30, 30, 31, 31, 32, 31, 31, 30, 30, 30, 29, 30, 30, 30, 31, 32, 31, 32, 30, 31, 30,
    30, 29, 30, 30, 30, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 30, 30, 31, 31, 32, 31, 31, 31, 30, 30, 29, 30, 30, 30,
    30, 31, 32, 32, 30, 31, 30, 30, 29, 30, 30, 30, 30, 32, 31, 32, 31, 30, 30, 30, 29, 30, 30, 30, 30, 32, 31, 32, 31,
    30, 30, 30, 29, 30, 30, 30};

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;

static void obtain_time(void);
static void initialize_sntp(void);
static void initialise_wifi(void);
static esp_err_t event_handler(void *ctx, system_event_t *event);
/**
 * @brief gives the present local time
 * 
 */
void time_function()
{
    ++boot_count;
    ESP_LOGI(TAG, "Boot count: %d", boot_count);

    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900))
    {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }

    // the timezone of nepal is set below
    setenv("TZ", "<+0545>-5:45", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%a %Y-%m-%dT%H:%M:%S+05:45", &timeinfo); //timeinfo is stored in strftime_buff in string format
    ESP_LOGI(TAG, "The current date/time in Nepal is: %s", strftime_buf);
}

static void obtain_time(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi();
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    ESP_ERROR_CHECK(esp_wifi_stop());
}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}
/**
 * @brief converts any timestamp to their respective date and time and finally converts to ad to bs date
 * 
 * @param timestamp 
 * @param buff 
 * @return esp_err_t 
 */
esp_err_t date_convert(time_t timestamp, char *buff)
{
    struct tm ts;
    char eng_date[40];

    ts = *localtime(&timestamp);
    strftime(eng_date, 40, "%Y-%m-%dT%H:%M:%S+05:45", &ts);
    // ESP_LOGI(TAG, "converted date from timestam in AD is: %s\n", eng_date);
    sscanf(eng_date, "%d-%d-%dT%d:%d:%d", &ad_year, &ad_month, &ad_day, &hr, &min, &sec);
    /**
 * @brief converting ad date to bs from here
 * 
 */
    engdiff = ad_year - sey;
    if (engdiff == 0) /*when the entered day is equal to the starting element of the array*/
    {
        for (k = 3; k < (ad_month - 1); k++) /*here starting with bcoz we have starting month 4 */
        {
            totaldayscount += engdays[k];
        }

        totaldayscount += ad_day - sed;
    }
    else
    {
        months = (engdiff)*12;
        for (i = 3; i < months; i++) /*here initializing i=3 because we have starting english month 4 */
        {
            totaldayscount += engdays[i];
        }
        for (j = i; j < (i + (ad_month - 1)); j++) /*here engmonth is -1 because we need not to add complete month eg if it is july 4 the entire day of july is not added infact only days upto july 4 is added*/
        {                                          /*adding the remaining months to the totaldayscount*/
            totaldayscount += engdays[j];
        }
        totaldayscount += ad_day - sed; /*here subtracting the sed bcoz it is already added at the beginning of the counting*/
    }
    iny = sny;
    inm = snm;
    ind = snd;
    while (totaldayscount != 0)
    {
        endday = nepdays[var];
        ind++;
        day++;
        if (ind > endday)
        {
            var++; /* here we have used var to call the elements of array because at the end we have used inm=1 which may cause malfunction of the converter*/
            inm++;
            ind = 1;
            if (inm > 12)
            {
                iny++;

                inm = 1;
            }
        }
        if (day > 7)
        {
            day = 1;
        }
        totaldayscount--;
    }

    // ESP_LOGI(TAG, "\tThe Converterd Date in BS:%d-%d-%d BS\n", iny, inm, ind);
    sprintf(buff, "%d-%d-%dT%d:%d:%d+05:45", iny, inm, ind, hr, min, sec);
    ESP_LOGI(TAG, "converted time in BS is: %s\n", buff);

    return ESP_OK;
}
