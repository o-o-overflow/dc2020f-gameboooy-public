#!/usr/bin/python3
from gevent.server import StreamServer
from mprpc import RPCServer

ROMLOADER_SERVER = 8081

GAMEBOY_STRUCT_SIZE = 151784
GAMEBOY_CART_OFFSET = 20

ROM_OFFSET_NAME = 0x134
ROM_OFFSET_TYPE = 0x147

ROM_PLAIN = 0

class RomLoader(RPCServer):
    def load(self, state, cart):
        gb = bytearray(GAMEBOY_STRUCT_SIZE)
        try:
            assert type(state) == bytes and type(cart) == bytes, 'invalid type'
            assert len(state) >= GAMEBOY_STRUCT_SIZE, 'incomplete state'
            gb = bytearray(state[:GAMEBOY_STRUCT_SIZE])

            l = len(cart)
            print("len(cart) + "+str(l))
            assert len(cart) == 32 * 1024, 'Only 32KB games with no mappers are supported!'

            header = cart[:0x180]
            name = ''.join(map(chr, header[ROM_OFFSET_NAME:])).split('\x00')[0]
            print('Internal ROM name: %s' % name)
            assert header[ROM_OFFSET_TYPE] == ROM_PLAIN, 'Only 32KB games with no mappers are supported!'

            for i in range(len(cart)):
                gb[GAMEBOY_CART_OFFSET + i] = cart[i]
            print('rom loaded')

        except Exception as e:
            print(repr(e))
            pass
        return bytes(gb)

loader = RomLoader()
setattr(loader, 'romloader::load', getattr(loader, 'load'))
server = StreamServer(('127.0.0.1', ROMLOADER_SERVER), loader)
server.serve_forever()
