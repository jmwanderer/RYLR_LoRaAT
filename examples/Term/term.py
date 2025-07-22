"""
A simple termial-like script to talk to the rylr998 over a 
USB to UART adapter on Linux. 

Usage: python3 term.py [tty device] [prompt]

Terminal programs have trouble with:
- buffering full line
- and sending a \r\n

Quick workaround while figuring out how to configure the terminal programs

Press <return> to read whatever is in the RX buffer.
"""

import os
import readline
import time
import sys
import select

tty_dev = "/dev/ttyUSB0"
prompt  = "$ "

sys.argv.pop(0)
if len(sys.argv) > 0:
    tty_dev = sys.argv[0]
    sys.argv.pop(0)

if len(sys.argv) > 0:
    prompt = sys.argv[0] + "$ "
    sys.argv.pop(0)


os.system(f"stty -F {tty_dev} 115200")
cfg = ( "-parenb -parodd -cmspar cs5 -hupcl -cstopb -cread -clocal -crtscts " +
        "-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl -ixon -ixoff " +
        "-iuclc -ixany -imaxbel -iutf8 " +
        "-opost -olcuc -ocrnl -onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0 " +
        "-isig -icanon -iexten -echo -echoe -echok -echonl -noflsh -xcase -tostop -echoprt " +
        "-echoctl -echoke -flusho -extproc")

os.system(f"stty -F {tty_dev} {cfg}")

tty = os.open(tty_dev, os.O_RDWR)


readline.add_history(
    "AT+SEND=2,100,HELLOHELLOHELLOHELLOHELLOHELLOHELLOHELLOHELLOHELLO" +
    "HELLOHELLOHELLOHELLOHELLOHELLOHELLOHELLOHELLOHELLO")
readline.add_history("AT+SEND=2,31,HELLOHELLOHELLOHELLOHELLOHELLO")
readline.add_history("AT+VER?")
readline.add_history("AT+SEND=2,5,HELLO")
readline.add_history("AT+SEND=2,4,PING")
readline.add_history("AT+CPIN=CA0043A7")
readline.add_history("AT+CRFOP=8")

while True:
    try:
        cmd = input(prompt)
    except EOFError:
        print("exit...")
        sys.exit(0)

    if len(cmd) > 0:
        cmd = f"{cmd.strip()}\r\n".encode('ascii')
        count = os.write(tty, cmd)
        print(f"wrote {count}")
        time.sleep(0.3)
    ready = select.select([tty], [], [], 0)
    if len(ready[0]) > 0:
        result = os.read(tty, 100)
        print(result)
