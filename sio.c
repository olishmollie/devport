#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/io.h>

#define EFER 0x2e
#define EFDR 0x2f
#define UNLOCK 0x87
#define LOCK 0xaa
#define LDN_SEL 0x07

uint8_t sio_readb(uint8_t reg, uint16_t port)
{
    outb(reg, port);
    return inb(port + 1);
}

void sio_writeb(uint8_t value, uint8_t reg, uint16_t port)
{
    outb(reg, port);
    outb(value, port + 1);
}

void sio_enter(void)
{
    outb(UNLOCK, EFER);
    outb(UNLOCK, EFER);
}

void sio_exit(void)
{
    outb(LOCK, EFER);
}

void sio_bank_select(uint8_t bank, uint16_t iobase)
{
    sio_writeb(bank, 0x4e, iobase);
}

bool sio_espi_enabled(void)
{
    return sio_readb(0x2f, EFER) & 0x4;
}

bool sio_i2c_enabled(void)
{
    /* Check pins 62/63. */
    bool kbc_en = sio_readb(0x2f, EFER) & 0x10;
    /* Check pins 94/96. */
    bool cr1b_en = sio_readb(0x1b, EFER) & 0x2;

    return !kbc_en || cr1b_en;
}

void sio_chip_info(uint8_t bank, uint16_t iobase)
{
    sio_bank_select(0x80, iobase);
    int vendorid_high = sio_readb(0xfe, iobase);
    sio_bank_select(0x00, iobase);
    int vendorid_low = sio_readb(0xfe, iobase);
    sio_writeb(bank, 0x43, iobase);
    printf("Vendor ID: 0x%x\n", (vendorid_high << 8) | vendorid_low);
    printf("  Chip ID: 0x%x\n", sio_readb(0x20, EFER) << 8 | sio_readb(0x21, EFER));
    printf("    espi?: %s\n", sio_espi_enabled() ? "true": "false");
    printf("     i2c?: %s\n", sio_i2c_enabled() ? "true" : "false");
}

void sio_ldn_select(uint8_t device)
{
    sio_writeb(device, LDN_SEL, EFER);
}

double sio_temperature(uint8_t high, uint8_t low)
{
    bool neg = high & 0x80;
    int base = ((high & 0x7f) << 1) | ((low & 0x80) >> 7);
    if (neg)
        base -= 1 << 9;
    return base * 0.5;
}

int main(int argc, char **argv)
{
    if (ioperm(0, 65535, 1)) {
		printf("%s must be run as root\n", argv[0]);
        return 1;
    }

    sio_enter();

    uint8_t ldn = 0x0b;
    uint8_t bank = 0x00;

    sio_ldn_select(ldn);

    int iobase = ((sio_readb(0x60, EFER) << 8) | sio_readb(0x61, EFER)) + 5;

    sio_bank_select(bank, iobase);

    sio_chip_info(bank, iobase);

    sio_exit();

    return 0;
}
