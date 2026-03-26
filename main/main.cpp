#include <stdio.h>
#include <vector>
#include <string>
#include "esp_log.h"
#include "esp_camera.h"
#include "camera_interface.h"
#include "dl_model_base.hpp"
#include "dl_image_process.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "HARDHAT_DETECTOR";

void detection_task(void *pvParameters) {
    ESP_LOGI(TAG, "Detection task started. Format: NCHW, Normalization: [-1, 1]");
    
    initialize_camera();

    // 1. Load Model from partition
    dl::Model *model = new dl::Model("model", fbs::MODEL_LOCATION_IN_FLASH_PARTITION);
    dl::TensorBase *input_tensor = model->get_input();
    
    // Model expects [1, 3, 256, 256]
    std::vector<int> input_shape = input_tensor->get_shape();
    int model_h = input_shape[2];
    int model_w = input_shape[3];

    uint8_t *rgb_buf = (uint8_t *)heap_caps_malloc(model_w * model_h * 3, MALLOC_CAP_SPIRAM);
    dl::image::ImageTransformer transformer;

    while (true) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // 2. Resize and Convert to RGB888
        dl::image::img_t src_img = { .data = fb->buf, .width = (uint16_t)fb->width, .height = (uint16_t)fb->height, .pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB565 };
        dl::image::img_t dst_img = { .data = rgb_buf, .width = (uint16_t)model_w, .height = (uint16_t)model_h, .pix_type = dl::image::DL_IMAGE_PIX_TYPE_RGB888 };

        transformer.set_src_img(src_img).set_dst_img(dst_img).transform();

        // 3. Preprocess: Normalization [-1, 1] and NCHW Transpose
        float *input_ptr = (float *)input_tensor->get_element_ptr();
        int plane_size = model_h * model_w;

        for (int i = 0; i < plane_size; i++) {
            // Math: (pixel / 127.5) - 1.0  is same as  (pixel - 127.5) / 127.5
            input_ptr[i] = (rgb_buf[i * 3 + 0] - 127.5f) / 127.5f;                  // R plane
            input_ptr[plane_size + i] = (rgb_buf[i * 3 + 1] - 127.5f) / 127.5f;      // G plane
            input_ptr[2 * plane_size + i] = (rgb_buf[i * 3 + 2] - 127.5f) / 127.5f;  // B plane
        }

        // 4. Run Inference
        model->run();

        // 5. Interpret Output (Confidence of NO Hardhat)
        dl::TensorBase *output = model->get_output();
        float *score = (float *)output->get_element_ptr();

        // Usually index 0 or the higher value is the prediction
        float no_hardhat_conf = score[0]; 
        
        // Let's print both just in case, but highlight the "No Hardhat" one
        ESP_LOGI(TAG, "Confidence NO-HARDHAT: %.2f%%", no_hardhat_conf * 100.0f);

        if (no_hardhat_conf > 0.80f) {
            ESP_LOGW(TAG, "!!! ALERT: NO HARDHAT DETECTED (Conf: %.2f) !!!", no_hardhat_conf);
        } else {
            ESP_LOGI(TAG, "Status: Hardhat present.");
        }

        esp_camera_fb_return(fb);
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

extern "C" void app_main() {
    xTaskCreatePinnedToCore(detection_task, "detection_task", 1024 * 32, NULL, 5, NULL, 1);
}
