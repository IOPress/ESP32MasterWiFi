#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_random.h"
#include "esp_tls.h"
#include "mbedtls\cipher.h"

void app_main(void)
{

    const mbedtls_cipher_info_t *cipher_info;
    const int *list;
    printf("Available ciphers:\n");
    list = mbedtls_cipher_list();
    while (*list)
    {
        cipher_info = mbedtls_cipher_info_from_type(*list);
        printf("  %s\n", mbedtls_cipher_info_get_name(cipher_info));
        list++;
    }
}
