#ifndef ERROR_H
#define ERROR_H

#define STRING(x) #x
#define ERROR_CHECK(x) do {                                             \
        esp_err_t tmp_err = (x);                                        \
        if (tmp_err != ESP_OK) {                                        \
            ESP_LOGE(TAG, STRING(x) " (%s)", esp_err_to_name(tmp_err));     \
            return ESP_FAIL;                                            \
        }                                                               \
    } while(0);

#endif