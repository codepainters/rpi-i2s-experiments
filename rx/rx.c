#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define I2S_REGS_START  (0xfe203000)
#define I2S_REGS_LEN    (32)

#define I2S_CS_A    (0)
#define I2S_FIFO_A  (1)
#define I2S_MODE_A  (2)
#define I2S_RXC_A   (3)
#define I2S_TXC_A   (4)
#define I2S_INTEN_A (6)

#define I2S_CS_A__RXON    (1 << 1)
#define I2S_CS_A__RXR     (1 << 18)

static volatile uint32_t *i2s;

void map_peripherals() {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open(/dev/gpiomem)");
        exit(1);
    }

    i2s = (uint32_t *) mmap(NULL, I2S_REGS_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, I2S_REGS_START);
    if (i2s == MAP_FAILED) {
        perror("mmap(i2s)");
        exit(1);
    }
}

void i2s_setup() {
    // reset RXON & TXON, set EN
    i2s[I2S_CS_A] = 0x00000001;

    // EN = 1, TXTHR = 00, RXTHR = 00, DMAEN = 0, RXSEX = 0
    i2s[I2S_CS_A] = 0x00000001;

    // FSLEN = 1, FLEN = 63 (64 bit frame), FSI = 0, FSM = 1 (PCM_FS is input),
    // CLKI = 1 (inp. sampled on rising edge), CLKM = 1 (PCM_CLK is input),
    // FXTP = 0, RXTP = 0, PDME = 0, PDMN = 0, CLK_DIS = 0 (enable PCM clock)
    i2s[I2S_MODE_A] = 0x00e0fc01;

    // setup 2 slots x 32 bits in both directions
    i2s[I2S_RXC_A] = 0xc018c218;
    i2s[I2S_TXC_A] = 0xc018c218;

    // no interrupts
    i2s[I2S_INTEN_A] = 0x00000000;

    // assert RXCLR, TXCLR
    i2s[I2S_CS_A] |= (1 << 4) | (1 << 3);
    usleep(1000);
}

void i2s_rx_loop(unsigned int n) {
    // set RXON
    i2s[I2S_CS_A] |= I2S_CS_A__RXON;
    printf("RX loop\n");
    while (n--) {
        // wait for data in FIFO
        while ((i2s[I2S_CS_A] & I2S_CS_A__RXR) == 0) {};
        uint32_t v = i2s[I2S_FIFO_A];
        printf("0x%08x\n", v);
    }
}

int main() {
    printf("Starting...\n");
    map_peripherals();

    i2s_setup();
    i2s_rx_loop(10000000);
}
