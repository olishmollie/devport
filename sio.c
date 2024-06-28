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

/* void sio_chip_info(uint16_t iobase) */
/* { */
/*     int chipid = (sio_readb(0x20, EFER) << 8) | sio_readb(0x21, EFER); */
/*     sio_writeb(0x80, 0x4e, iobase); */
/*     int vendorid_high = sio_readb(0x4f, iobase); */
/*     sio_writeb(0x00, 0x4e, iobase); */
/*     int vendorid_low = sio_readb(0x4f, iobase); */
/* } */

void sio_ldn_select(uint8_t device)
{
    sio_writeb(device, LDN_SEL, EFER);
}

void sio_bank_select(uint8_t bank, uint16_t iobase)
{
    sio_writeb(bank, 0x4e, iobase);
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

    sio_ldn_select(0x0b);

    int iobase = ((sio_readb(0x60, EFER) << 8) | sio_readb(0x61, EFER)) + 5;

    sio_bank_select(0x04, iobase);

    double cpuvcore = sio_readb(0x80, iobase) * 0.008;
    printf("cpuvcore  %0.4f V, ", cpuvcore);

    sio_bank_select(0x01, iobase);
    printf("cputin  %.1f C\n",
           sio_temperature(sio_readb(0x50, iobase), sio_readb(0x51, iobase)));

    sio_exit();

    return 0;
}
