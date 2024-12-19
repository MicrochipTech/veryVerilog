from sys import argv

import hid


def autodetect_minfpga_path(product_string: str = 'miniFPGA board') -> str|None:
    for device_dict in hid.enumerate():
        if device_dict.get('product_string') == product_string:
            return device_dict.get('path')
    return None


def intToPort(pin_name: str, value:int, portbita:int=8):
    portstr = ''
    for b in range(portbita):
        portstr += f' {pin_name}{b}={"H" if value & (1<<b) else "L"}'
    return portstr


def dissect_feature_rpt0(rpt_in:list[int]):
    if rpt_in[0] == 0:
        print(f'→ ICSP_meta_command_id = 0x{rpt_in[1]:02X}')
        ICSP_target_state_str = 'Unknown'
        if rpt_in[2] == 0:
            ICSP_target_state_str = 'Running'
        elif rpt_in[2] == 1:
            ICSP_target_state_str = 'Soon running'
        elif rpt_in[2] == 2:
            ICSP_target_state_str = 'Stopped'
        print(f'→ ICSP_target_state = {ICSP_target_state_str} (={rpt_in[2]})')
        baud_exact = rpt_in[3] + (rpt_in[4] << 8) + (rpt_in[5] << 16)
        baud_approx_str = ''
        baud_factor = 100 if baud_exact < 250 else 300
        if baud_exact % baud_factor:
            baud_std = int(baud_exact / baud_factor)*baud_factor
            baud_approx_str = f'(≅ {baud_std}, error={(baud_exact-baud_std)/baud_std:.2%})'
        print(f'→ HW Baud rate = {baud_exact} {baud_approx_str}')
        print(f'→ PORTA=0x{rpt_in[6]:02X} :{intToPort("RA", rpt_in[6])}')
        print(f'→ PORTB=0x{rpt_in[7]:02X} :{intToPort("RB", rpt_in[7])}')
        print(f'→ PORTC=0x{rpt_in[8]:02X} :{intToPort("RC", rpt_in[8])}')


hid_device_path:str|None
if len(argv) > 1:
    hid_device_path = argv[1]
else:
    hid_device_path = autodetect_minfpga_path()

if hid_device_path:
    minifpga = hid.device()
    minifpga.open_path(hid_device_path)
    minifpga.set_nonblocking(True)
    rpt_in = minifpga.get_feature_report(0, 255)
    print(f'Get Feature(0,255) → ', rpt_in)
    dissect_feature_rpt0(rpt_in)
    print(minifpga.error())
    rpt_out = list(range(20))
    rpt_out[0]=0
    rpt_out[1]=2
    rpt_out[2]=254
    print(f'Send Feature({rpt_out}) → ', minifpga.send_feature_report(rpt_out))
    rpt_in = minifpga.get_feature_report(0, 255)
    print(f'Get Feature(0,255) → ', rpt_in)
    print(minifpga.error())
    dissect_feature_rpt0(rpt_in)
    rpt_out = [0, 1, 96, 0]
    print(f'Send Feature({rpt_out}) → ', minifpga.send_feature_report(rpt_out ))
    rpt_in = minifpga.get_feature_report(0, 255)
    print(f'Get Feature(0,255) → ', rpt_in)
    print(minifpga.error())
    dissect_feature_rpt0(rpt_in)
    read_num = 100
    data_got = minifpga.read(read_num)
    print(f'Read({read_num})=<{len(data_got)}>{data_got}' )
    data_out = [0]+list(range(1,31))
    print(f'Write(<{len(data_out)}>{data_out})=', minifpga.write(data_out))
    minifpga.set_nonblocking(False)
    data_got = minifpga.read(read_num)
    print(f'Read({read_num})=<{len(data_got)}>{data_got}' )
    minifpga.close()