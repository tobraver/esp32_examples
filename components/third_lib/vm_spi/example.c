

vm_spi_t g_rc522_vm_spi = {0};


void WriteRawRC(uint8_t ucAddress, uint8_t ucValue)
{
    uint8_t ucAddr;

    ucAddr = (ucAddress << 1) & 0x7E;
    vm_spi_select(&g_rc522_vm_spi, 1);

    vm_spi_write_buffer(&g_rc522_vm_spi, &ucAddr, sizeof(ucAddr));
    vm_spi_write_buffer(&g_rc522_vm_spi, &ucValue, sizeof(ucValue));

    vm_spi_select(&g_rc522_vm_spi, 0);
}

uint8_t ReadRawRC(uint8_t ucAddress)
{
    uint8_t ucAddr = 0, ucReturn = 0;

    ucAddr = ((ucAddress << 1) & 0x7E) | 0x80;
    vm_spi_select(&g_rc522_vm_spi, 1);

    vm_spi_write_buffer(&g_rc522_vm_spi, &ucAddr, sizeof(ucAddr));
    vm_spi_read_buffer(&g_rc522_vm_spi, &ucReturn, sizeof(ucReturn));

    vm_spi_select(&g_rc522_vm_spi, 0);

    return ucReturn;
}

void example()
{
    g_rc522_vm_spi.cs_io_num = RC522_CS_PIN_NUM;
    g_rc522_vm_spi.sck_io_num = RC522_SCK_PIN_NUM;
    g_rc522_vm_spi.mosi_io_num = RC522_MOSI_PIN_NUM;
    g_rc522_vm_spi.miso_io_num = RC522_MISO_PIN_NUM;
    vm_spi_init(&g_rc522_vm_spi);


    WriteRawRC();

    ReadRawRC();
}

