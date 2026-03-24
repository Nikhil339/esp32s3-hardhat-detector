#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dl_model_base.hpp"

static const char *TAG = "HARDHAT_PROFILER";

// Reference the embedded model
extern const uint8_t model_espdl_start[] asm("_binary_hardhat_model_espdl_start");

extern "C" void app_main() {
    // 1. LOAD THE MODEL
    ESP_LOGI(TAG, "Initializing Model from Flash...");
    dl::Model *model = new dl::Model(
        (const char *)model_espdl_start, 
        fbs::MODEL_LOCATION_IN_FLASH_RODATA
    );

    // 2. TEST THE MODEL MATH
    ESP_LOGI(TAG, "-----------------------------------");
    ESP_LOGI(TAG, "Running Automated Math Test...");
    if (model->test() == ESP_OK) {
        ESP_LOGI(TAG, "RESULT: SUCCESS! C++ output matches Python perfectly.");
    } else {
        ESP_LOGE(TAG, "RESULT: FAILED! Math mismatch detected.");
    }

    // 3. PROFILE MEMORY
    ESP_LOGI(TAG, "-----------------------------------");
    ESP_LOGI(TAG, "Profiling Memory Usage...");
    model->profile_memory();

    // 4. PROFILE LAYER LATENCY
    ESP_LOGI(TAG, "-----------------------------------");
    ESP_LOGI(TAG, "Profiling Layer Speeds (Sorted Slowest to Fastest)...");
    model->profile_module(true); 

    ESP_LOGI(TAG, "-----------------------------------");
    ESP_LOGI(TAG, "Profiling Complete. Freeing memory...");
    
    delete model;
}
