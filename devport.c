#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>

#define DEVPORT_UNLOCK 0x87
#define DEVPORT_LOCK 0xaa
#define DEVPORT_LDN 0x07

uint8_t devport_read(uint16_t port)
{
    return inb_p(port);
}

void devport_write(uint8_t value, uint16_t port)
{
    outb_p(value, port);
}

void devport_enter(void)
{
    devport_write(DEVPORT_UNLOCK, 0x2e);
    devport_write(DEVPORT_UNLOCK, 0x2e);
}

void devport_exit(void)
{
    devport_write(DEVPORT_LOCK, 0x2e);
}

void devport_select_ldn(uint8_t device)
{
    devport_write(DEVPORT_LDN, 0x2e);
    devport_write(device, 0x2f);
}

int main(int argc, char **argv)
{
    if (ioperm(0, 6000, 1) < 0) {
		printf("%s must be run as root\n", argv[0]);
        return 1;
    }

    int result = 0;

    devport_enter();
    devport_select_ldn(0xb);

    /*devport_write(0x20, 0x2e);*/
    /*result = devport_read(0x2f);*/

    devport_write(0x58, 0x2e);
    result = devport_read(0x2f);

    printf("result = 0x%x\n", result);
    devport_exit();

    return 0;
}
