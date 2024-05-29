# 组件使用

将 `iot_modem` 添加到工程目录的components中。


# 组件配置

`EC600NCNLA-N05` 为示例。

```
idf.py menuconfig
    Component config
        IoT USB Host
        IoT USB Host CDC
        IoT USB Host Modem
            Choose Modem Board (EC600NCNLA-N05)
            (5) Retry Times During Dial-up
            (internet) Set Modem APN
            (1234) Set SIM PIN
            GPIO Config
                (0) Modem power control gpio, set 0 if not use 
                (11) Modem reset control gpio, set 0 if not use
                (0) Power control gpio inactive level
                (1) Reset control gpio inactive level 
```

