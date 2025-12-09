#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_random.h"
#include "esp_tls.h"
#include "mbedtls\cipher.h"

void app_main(void)
{

    int ret;

    mbedtls_cipher_context_t cipher_ctx;
    const mbedtls_cipher_info_t *cipher_info;
    cipher_info = mbedtls_cipher_info_from_string("AES-128-CBC");
    ret = mbedtls_cipher_setup(&cipher_ctx, cipher_info);

    unsigned char IV[16];
    esp_fill_random(IV, 16);
    ret = mbedtls_cipher_set_iv(&cipher_ctx, IV, mbedtls_cipher_get_iv_size(&cipher_ctx));

    unsigned char key[16];
    esp_fill_random(key, 16);
    ret = mbedtls_cipher_setkey(&cipher_ctx, key, mbedtls_cipher_get_key_bitlen(&cipher_ctx), MBEDTLS_ENCRYPT);

    ret = mbedtls_cipher_reset(&cipher_ctx);

    u_char buffer[16] = "Hello World";
    u_char output[16];
    size_t olen;
    ret = mbedtls_cipher_update(&cipher_ctx, buffer, 16, output, &olen);
    printf("cipher text ");
    for (int i = 0; i < olen; i++)
    {
        printf("%02X", output[i]);
    }
    printf("\n");

    u_char plaintext[16];
    ret = mbedtls_cipher_setkey(&cipher_ctx, key, mbedtls_cipher_get_key_bitlen(&cipher_ctx), MBEDTLS_DECRYPT);
    ret = mbedtls_cipher_set_iv(&cipher_ctx, IV, mbedtls_cipher_get_iv_size(&cipher_ctx));
    ret = mbedtls_cipher_reset(&cipher_ctx);
    mbedtls_cipher_update(&cipher_ctx, output, 16, plaintext, &olen);
    printf("plain text %.16s\n", plaintext);
}
