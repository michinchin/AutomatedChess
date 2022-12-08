import logging
import asyncio 
import platform
import ast

from bleak import BleakClient
from bleak import BleakScanner
from bleak import discover


# on_value = bytearray([0x01])
# off_value = bytearray([0x00])

curr_uuid = "d7a16eff-1ee7-4344-a3d2-a8203d97d75c"

async def setInput(client):
    command = input("Please enter move (i.e. 1 0):")
    cmds = command.split(' ')

    await client.write_gatt_char(curr_uuid,  bytearray([bytes(cmds[0].encode('UTF-8')), bytes(cmds[1].encode('UTF-8')) ]), True) 
    
async def run():
    print('Looking for nano ble peripheral')

    found = False
    devices = await BleakScanner.discover()
    for d in devices:
        # print(d)
        if d.name is not None and 'Arduino' in d.name:
            print('Found Arduino Nano 33')
            print(d.details)
            found = True
            async with BleakClient(d.address) as client:
                while True:
                    await setInput(client)
         
    if not found:
        print('Could not find Arduino nano 33 ble')

def main():
    loop = asyncio.get_event_loop()
    try: 
        loop.run_until_complete(run())
    except KeyboardInterrupt:
        print('\nKeyboard Interrupt')
    finally:
        print('Program Finished')

if __name__ == "__main__":
    main()
