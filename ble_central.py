import logging
import asyncio 
import platform
import ast

from bleak import BleakClient
from bleak import BleakScanner
from bleak import discover


on_value = bytearray([0x01])
off_value = bytearray([0x00])

async def run(input):
    global RED, GREEN, BLUE
    print('Looking for nano ble peripheral')

    found = False
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
        if d.name is not None and 'Arduino' in d.name:
            print('Found Arduino Nano 33')
            print(d.details)
            found = True
            # async with BleakClient(d.address) as client:
                # await client.write_gatt_char(0x2A57, on_value)
         
    if not found:
        print('Could not find Arduino nano 33 ble')

def getInput():
    command = input("Please enter move:")
    print("You entered", command)
    cmds = command.split(' ')
    return cmds

def main():
    input = getInput()
    loop = asyncio.get_event_loop()
    try: 
        loop.run_until_complete(run(input))
    except KeyboardInterrupt:
        print('\nKeyboard Interrupt')
    finally:
        print('Program Finished')

if __name__ == "__main__":
    main()