import pyb, sensor, image, math, time
enable_lens_corr = False # turn on for straighter lines...
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()

# All lines also have `x1()`, `y1()`, `x2()`, and `y2()` methods to get their end-points
# and a `line()` method to get all the above as one 4 value tuple for `draw_line()`.
def degrees(radians):
    return (180 * radians) / math.pi

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

while(True):
    clock.tick()
    img = sensor.snapshot()
    if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

    for l in img.find_line_segments(merge_distance = 200, max_theta_diff = 200):
        img.draw_line(l.line(), color = (255, 255, 0))
        print_args = (l.x1(), l.y1(), l.x2(), l.y2())
        uart.write(("/line/run %f %f %f %f\n" % print_args).encode())
        print(("/line/run %f %f %f %f\n" % print_args).encode())
