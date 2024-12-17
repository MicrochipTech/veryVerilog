from datetime import datetime
from itertools import batched, chain

import hid
import sys


class ICSP_HID:
    def __init__(self, hid_dev: hid.device):
        self._hid = hid_dev
        self._meta_cmd = 255
        self._cmd_bits = 8
        self._data_bits = 24

    @property
    def meta_cmd(self):
        rpt_in = self._hid.get_feature_report(0, 255)
        if len(rpt_in) > 1:
            self._meta_cmd = rpt_in[1]
        return self._meta_cmd & 0xFF

    @meta_cmd.setter
    def meta_cmd(self, new_val : int):
        new_val = int(new_val)
        if 0 <= new_val <= 255:
            self._hid.send_feature_report([0,2,new_val])

    @property
    def cmd_bits(self):
        return max(min(self._cmd_bits, 8), 1)

    @property
    def data_bits(self):
        return max(min(self._data_bits, 24), 1)

    def get_command_bytes(self, cmd: int, param: int = 0, meta_cmd: bool = False) -> list[int]:
        cmd = cmd & 0xFF
        param = param & 0xFFFFFF
        if meta_cmd:
            param = param & 0xFFFF
            cmd = [self.meta_cmd, cmd, param & 0xFF, (param >> 8) & 0xFF]
        else:
            cmd = [cmd, param & 0xFF, (param >> 8) & 0xFF, (param >> 16) & 0xFF]
        return cmd

    def xchg_command_block(self, cmd_block: list) -> tuple:
        if len(cmd_block) % 4 != 0:
            raise RuntimeError('HID Command block misalignment in "send_command_block"')
        reply:list[int] = []
        for cmd_block_chunk in batched(cmd_block, 64):
            data_out = list(cmd_block_chunk) + self.get_command_bytes(cmd=0, meta_cmd=True) * ((64 - len(cmd_block_chunk)) >> 2)  # Fill with MetaCMD NOP
            self._hid.write([0] + data_out)
            self._hid.set_nonblocking(False)
            reply += self._hid.read(64)
        return tuple(reply)

    def lvp_enter(self):
        reply = self.xchg_command_block(
            self.get_command_bytes(cmd=2, param=self.meta_cmd, meta_cmd=True) +
            self.get_command_bytes(cmd=3, param=self.cmd_bits, meta_cmd=True) +
            self.get_command_bytes(cmd=4, param=self.data_bits, meta_cmd=True) +
            self.get_command_bytes(cmd=5, meta_cmd=True)
        )

    def lvp_exit(self):
        reply = self.xchg_command_block(
            self.get_command_bytes(cmd=6, meta_cmd=True)
        )

    def set_pc(self, new_pc: int):
        reply = self.xchg_command_block(
            self.get_command_bytes(cmd=0x80, param=new_pc)
        )

    def read_word(self, advance_pc = False) -> int:
        reply = self.xchg_command_block(
            self.get_command_bytes(cmd=8, param=0xFE if advance_pc else 0xFC, meta_cmd=True)
        )
        if reply[0] == 0xFC:
            return reply[1] + (reply[2] << 8) + (reply[3] << 16)
        else:
            raise RuntimeError('Unexpected read command in "read_word"', reply)

    def get_hw_baud_rate(self) -> int:
        reply = self.xchg_command_block(
            self.get_command_bytes(cmd=9, meta_cmd=True)
        )
        if reply[0] == self.meta_cmd:
            return reply[1] + (reply[2] << 8) + (reply[3] << 16)
        else:
            raise RuntimeError('Unexpected read command in "get_hw_baud_rate"', reply)

    def set_hw_baud_rate(self, new_baud : int) -> int:
        reply = self.xchg_command_block(
            self.get_command_bytes(cmd=1, param=int(new_baud/100), meta_cmd=True) +
            self.get_command_bytes(cmd=9, meta_cmd=True)
        )
        if reply[4] == self.meta_cmd:
            return reply[5] + (reply[6] << 8) + (reply[7] << 16)
        else:
            raise RuntimeError('Unexpected read command in "get_hw_baud_rate"', reply)

    def cdc_loopback(self, data: bytes):
        cmd_list = [self.get_command_bytes(cmd=11, meta_cmd=True, param=ch) for ch in data]
        cmd_list = list(chain.from_iterable(cmd_list))
        reply = self.xchg_command_block(cmd_list)

def autodetect_minfpga_path(product_string: str = 'miniFPGA board') -> str | None:
    for device_dict in hid.enumerate():
        if device_dict.get('product_string') == product_string:
            return device_dict.get('path')
    return None


if __name__ == '__main__':
    hid_device_path: str | None
    if len(sys.argv) > 1:
        hid_device_path = sys.argv[1].encode()
    else:
        hid_device_path = autodetect_minfpga_path()
    if hid_device_path:
        hid_device = hid.device()
        hid_device.open_path(hid_device_path)
        print(f'Opened HID device: \n{hid_device.get_manufacturer_string()}→{hid_device.get_product_string()}')
        pic_icsp = ICSP_HID(hid_device)
        pic_icsp.meta_cmd = 0xFF
        pic_icsp.lvp_exit()
        pic_icsp.lvp_enter()
        pic_icsp.set_pc(0x8006*2)
        print(f'DEVID={pic_icsp.read_word()>>1:#04x}')
        pic_icsp.set_pc(0x8005*2)
        print(f'REVID={pic_icsp.read_word()>>1:#04x}')
        pic_icsp.set_pc(0x0000*2)
        print(f'@0x0000={pic_icsp.read_word()>>1:#08x}')
        print('DIA →')
        for addr in range(0x8100, 0x811F+1):
            pic_icsp.set_pc(addr*2)
            print(f'@{addr:#04x}={pic_icsp.read_word()>>1:#08x}')
        print('DCI →')
        for addr in range(0x8200, 0x8204+1):
            pic_icsp.set_pc(addr*2)
            print(f'@{addr:#04x}={pic_icsp.read_word()>>1:#08x}')
        pic_icsp.lvp_exit()
        # Check baud HW rate get/set
        print('HW_baud_rate=', pic_icsp.get_hw_baud_rate())
        print('set_baud_rate=', pic_icsp.set_hw_baud_rate(115200))
        print('HW_baud_rate=', pic_icsp.get_hw_baud_rate())
        pic_icsp.meta_cmd = 101
        pic_icsp.cdc_loopback(f'Hello USB serial world! UID:{datetime.now().timestamp()}\n'.encode())
        hid_device.close()
    else:
        print('HID device not autodetected')
