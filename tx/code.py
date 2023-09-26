import board
import  digitalio
import time

p_bclk = digitalio.DigitalInOut(board.IO1)
p_bclk.direction = digitalio.Direction.OUTPUT

p_lrck = digitalio.DigitalInOut(board.IO2)
p_lrck.direction = digitalio.Direction.OUTPUT

p_data = digitalio.DigitalInOut(board.IO3)
p_data.direction = digitalio.Direction.OUTPUT



def send_bit(lrck: bool, value: bool):
    p_bclk.value = False
    p_lrck.value = lrck
    p_data.value = value
    time.sleep(0.0001)
    p_bclk.value = True
    time.sleep(0.0001)


def send_frame(s1: int, s2: int, s3: int, s4: int, with_lrck: bool):
    bit_cnt = 0
    for s in (s1, s2, s3, s4):
        for i in range(32):
            v = (s & 0x80000000) and True or False
            send_bit(with_lrck and bit_cnt==127, v)
            s = s << 1
            bit_cnt += 1

            
for i in range(1000):
    print(i)
    s1 = 0xAA000000 + (i << 8) + 1
    s2 = 0xAA000000 + (i << 8) + 2
    s3 = 0xAA000000 + (i << 8) + 3
    s4 = 0xAA000000 + (i << 8) + 4
    send_frame(s1, s2, s3, s4, i > 500)


