# README:
# To run this script, you need to install 2 libs
# pip install pyserial
# pip install click

import serial
import serial.tools.list_ports
import click
import re
import time

@click.group()
def cli():
    pass

@click.command("m", help="monitor port")
@click.option('--port', '-p', required=True, help="set port")
@click.option('--file', '-f', default="data.log", help="set output")
@click.option('--baudrate', '-b', show_default=True, default=9600, help="set baudrate")
@click.option('--pattern', help="set pattern")
def cmd_monitor_port(port: str, baudrate: int, file: str, pattern: str):
    with serial.Serial(port, baudrate, timeout= 1) as ser, open(file, 'ab+') as bfile:
        title = f"Start Monitoring! port: {port}, baudrate: {baudrate}, file: {file}, pattern: {pattern}\r\n"
        bfile.write(title.encode('utf-8'))
        print(title)
        
        while True:
            line = ser.readline()

            if (pattern is None or re.search(pattern, line.decode()) is not None):
                bfile.write(line)
                print(line.decode(), end="")
            time.sleep(0.0001)

@click.command("p", help="Show ports")
def cmd_print_ports():
    for p in serial.tools.list_ports.comports():
        print(p)

cli.add_command(cmd_print_ports)
cli.add_command(cmd_monitor_port)

if __name__ == '__main__':
    cli()

    