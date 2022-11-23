import logging
import asyncio 
import platform
import ast

from bleak import BleakScanner
from bleak import discover

RED_LED_UUID = '13012F01-F8C3-4F4A-A8F4-15CD926DA146'
GREEN_LED_UUID = '13012F02-F8C3-4F4A-A8F4-15CD926DA146'
BLUE_LED_UUID = '13012F03-F8C3-4F4A-A8F4-15CD926DA146'

on_value = bytearray([0x01])
off_value = bytearray([0x00])

RED = False
GREEN = False
BLUE = False

async def run():
    global RED, GREEN, BLUE
    print('Looking for nano ble peripheral')

    found = False
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
        # if d.name is not None and 'Nano 33 BLE' in d.name:
        #     print('Found Arduino Nano 33')
        #     print(d.details)
        #     found = True
         
    if not found:
        print('Could not find Arduino nano 33 ble')

def main():
    command = input("Please enter move:")
    print("You entered", command)
    loop = asyncio.get_event_loop()
    try: 
        loop.run_until_complete(run())
    except KeyboardInterrupt:
        print('\nKeyboard Interrupt')
    finally:
        print('Program Finished')

if __name__ == "__main__":
    main()