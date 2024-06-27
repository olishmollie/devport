#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/io.h>

#define ARR_SIZE(x) sizeof((x)) / sizeof((x[0]))

#define SIO_EFER 0x2e               /* Extended function enable register */
#define SIO_EFDR SIO_EFER + 1       /* Extended function data register */

#define SIO_UNLOCK 0x87
#define SIO_LOCK 0xaa
#define SIO_LDN_SEL 0x07

uint8_t sio_read(uint8_t reg, uint16_t port)
{
    outb_p(reg, port);
    return inb_p(port + 1);
}

void sio_write(uint8_t value, uint8_t reg, uint16_t port)
{
    outb_p(reg, port);
    outb_p(value, port + 1);
}

void sio_enter(void)
{
    outb_p(SIO_UNLOCK, SIO_EFER);
    outb_p(SIO_UNLOCK, SIO_EFER);
}

void sio_exit(void)
{
    outb_p(SIO_LOCK, SIO_EFER);
}

uint16_t sio_temperature_raw(uint8_t high, uint8_t low)
{
    uint16_t base = high;
    printf("base = %d\n", base);
    return (base << 1) | low;
}

double sio_temperature(uint8_t high, uint8_t low)
{
    bool neg = high & 0x80;
    int8_t base = ((high & 0x7f) << 1) | low;
    if (neg)
        printf("It's negative!\n");
    printf("base = %d\n", base);
    return base * 0.5;
}

int main(int argc, char **argv)
{
    if (ioperm(0, 6000, 1) < 0) {
		printf("%s must be run as root\n", argv[0]);
        return 1;
    }

    sio_enter();

    int chipid = (sio_read(0x20, SIO_EFER) << 8) | sio_read(0x21, SIO_EFER);
    printf("chipid = 0x%x\n", chipid);

    /* Select hardware manager logical device. */
    sio_write(0x0b, SIO_LDN_SEL, SIO_EFER);

    int iobase = (sio_read(0x60, SIO_EFER) << 8) | sio_read(0x61, SIO_EFER);
    printf("iobase = 0x%x\n", iobase);

    int cr1a = sio_read(0x1a, SIO_EFER);
    printf("cr1a = 0x%x\n", cr1a);
    int cr1b= sio_read(0x1b, SIO_EFER);
    printf("cr1b = 0x%x\n", cr1b);
    int cr2a = sio_read(0x2a, SIO_EFER);
    printf("cr2a = 0x%x\n", cr2a);
    int cr2f = sio_read(0x2f, SIO_EFER);
    printf("cr2f = 0x%x\n", cr2f);

    /*int vendorid = sio_read(0xfe, iobase + 5);*/
    /*printf("vendorid = 0x%x\n\n", vendorid);*/

    printf("\n");
    printf("CPUVCORE = %g\n", sio_read(0x00, iobase + 5) * 0.008);
    printf("    VIN0 = %g\n", sio_read(0x01, iobase + 5) * 0.008);
    printf("    AVSB = %g\n", sio_read(0x02, iobase + 5) * 0.016);
    printf("    3VCC = %g\n", sio_read(0x03, iobase + 5) * 0.016);
    printf("    VIN1 = %g\n", sio_read(0x04, iobase + 5) * 0.008);
    printf("    VIN2 = %g\n", sio_read(0x05, iobase + 5) * 0.008);
    printf("    VHIF = %g\n", sio_read(0x06, iobase + 5) * 0.016);
    printf("    3VSB = %g\n", sio_read(0x07, iobase + 5) * 0.016);
    printf("    VBAT = %g\n", sio_read(0x08, iobase + 5) * 0.016);
    printf("     VTT = %g\n", sio_read(0x09, iobase + 5) * 0.008);

    sio_exit();

    return 0;
}
