#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_random.h"
#include "esp_tls.h"
#include <string.h>
#include "mbedtls/sha256.h"

void app_main(void)
{
    u_char input[] = "Hello SHA256 World";
    unsigned char hash[32];
    int ret;

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);

    ret = mbedtls_sha256_starts(&ctx, 0);
    ret = mbedtls_sha256_update(&ctx, input, strlen((char*)input));
    ret = mbedtls_sha256_finish(&ctx, hash);

    printf("Input string: \"%s\"\n", input);
    printf("SHA256 Hash: ");
    for (int i = 0; i < 32; i++)
    {
        printf("%02X", hash[i]);
    }
    printf("\n");
    mbedtls_sha256_free(&ctx);
}
