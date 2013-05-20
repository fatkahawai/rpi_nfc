echo

gcc -o rpi_nfc rpi_nfc.c tcp_client.c nfc_driver.c led_driver.c nfc-utils.c -lnfc 
