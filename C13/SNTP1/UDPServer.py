import asyncio
class ClientDatagramProtocol(asyncio.DatagramProtocol):
    def datagram_received(self, data, addr):
        message = data.decode("utf8")
        print("Received",message,"from", addr)
        data=b"Hello UDP World"
        self.transport.sendto(data,addr=("192.168.253.68",123))
    def connection_made(self, transport):
        self.transport = transport

async def main():
    loop = asyncio.get_running_loop()
    transport, protocol = await loop.create_datagram_endpoint(
          lambda: ClientDatagramProtocol(), local_addr=('0.0.0.0', 123))
    await asyncio.sleep(100000)
    transport.close()

asyncio.run(main())