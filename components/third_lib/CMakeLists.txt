set(COMPONENT_ADD_INCLUDEDIRS
    bsp_uart
    ticker
    vm_spi
    crc16
    mutex
    update
    wiegand
    prefs
    prefs_ev
    base64
)

set(COMPONENT_REQUIRES main)

set(COMPONENT_SRCS 
    bsp_uart/bsp_uart.c
    ticker/ticker.c
    vm_spi/vm_spi.c
    crc16/crc16.c
    mutex/mutex.c
    update/update.c
    wiegand/wiegand.c
    prefs/prefs.c
    prefs_ev/prefs_ev.c
    base64/base64.c
)

register_component()
