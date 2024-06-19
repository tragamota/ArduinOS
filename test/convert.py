import serial
import time
import argparse

command_map = {
    0x01: 'CHAR',
    0x02: 'INT',
    0x03: 'FLOAT',
    0x04: 'STRING',
    0x05: 'SET',
    0x06: 'GET',
    0x07: 'INCREMENT',
    0x08: 'DECREMENT',
    0x09: 'PLUS',
    0x0A: 'MINUS',
    0x0B: 'TIMES',
    0x0C: 'DIVIDE',
    0x0D: 'MODULO',
    0x0E: 'UNARYMINUS',
    0x0F: 'EQUALS',
    0x10: 'NOTEQUALS',
    0x11: 'LESSTHAN',
    0x12: 'LESSTHANOREQUALS',
    0x13: 'GREATERTHAN',
    0x14: 'GREATERTHANOREQUALS',
    0x15: 'LOGICALAND',
    0x16: 'LOGICALOR',
    0x17: 'LOGICALXOR',
    0x18: 'LOGICALNOT',
    0x19: 'BITWISEAND',
    0x1A: 'BITWISEOR',
    0x1B: 'BITWISEXOR',
    0x1C: 'BITWISENOT',
    0x1D: 'TOCHAR',
    0x1E: 'TOINT',
    0x1F: 'TOFLOAT',
    0x20: 'ROUND',
    0x21: 'FLOOR',
    0x22: 'CEILING',
    0x23: 'MIN',
    0x24: 'MAX',
    0x25: 'ABS',
    0x26: 'CONSTRAIN',
    0x27: 'MAP',
    0x28: 'POW',
    0x29: 'SQ',
    0x2A: 'SQRT',
    0x2B: 'DELAY',
    0x2C: 'DELAYUNTIL',
    0x2D: 'MILLIS',
    0x2E: 'PINMODE',
    0x2F: 'ANALOGREAD',
    0x30: 'ANALOGWRITE',
    0x31: 'DIGITALREAD',
    0x32: 'DIGITALWRITE',
    0x33: 'PRINT',
    0x34: 'PRINTLN',
    0x35: 'OPEN',
    0x36: 'CLOSE',
    0x37: 'WRITE',
    0x38: 'READINT',
    0x39: 'READFLOAT',
    0x3A: 'READSTRING',
    0x3B: 'READCHAR',
    0x3C: 'IF',
    0x3D: 'ELSE',
    0x3E: 'ENDIF',
    0x3F: 'WHILE',
    0x40: 'ENDWHILE',
    0x41: 'LOOP',
    0x42: 'ENDLOOP',
    0x43: 'STOP',
    0x44: 'FORK',
    0x45: 'WAITUNTILDONE'
}

reverse_command_map = {v: k for k, v in command_map.items()}


def setup_serial(port, baudrate):
    ser = serial.Serial(
        port=port,
        baudrate=baudrate,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1
    )
    return ser


def parse_item(item):
    if len(item) == 3 and item[0] == "'" and item[2] == "'":
        return ord(item[1])
    else:
        return int(item)


def convert_to_bytecode(data):
    result = bytearray()
    tokens = data.split()
    for item in tokens:
        if item in reverse_command_map:
            result.append(reverse_command_map[item])
        else:
            try:
                # Convert to integer and append as byte
                result.append(parse_item(item))
            except ValueError:
                raise ValueError(f"Unknown command or data: {item}")
    return result


def upload_bytecode(ser, filename, bytecode):
    # Send erase command
    time.sleep(2)
    print(ser.read_all())
    time.sleep(2)

    erase_command = f"erase {filename}\n"
    ser.write(erase_command.encode())
    time.sleep(2)  # Wait for a short period to ensure the command is processed

    print(ser.read_all())

    time.sleep(2)

    command = f"store\n{filename}\n{len(bytecode)}\n"
    ser.write(command.encode())

    print(ser.read_all())

    time.sleep(2)

    for byte in bytecode:
        ser.write(bytes([byte]))
        time.sleep(0.015)

    time.sleep(2)

    print(ser.read_all())

    time.sleep(2)

    ser.write("\n".encode())


def main():
    parser = argparse.ArgumentParser(description="Upload a program to a microcontroller via serial port.")
    parser.add_argument("filename", type=str, help="The file containing the program instructions.")
    parser.add_argument("baudrate", type=int, help="The baud rate for the serial connection.")
    parser.add_argument("port", type=str, help="The serial port to use for the connection.")

    args = parser.parse_args()

    try:
        with open(args.filename, 'r') as file:
            instructions = file.readlines()

        ser = setup_serial(args.port, args.baudrate)

        bytecode_data = []

        for instruction in instructions:
            bytecode_data.extend(convert_to_bytecode(instruction))

        print(bytecode_data)
        upload_bytecode(ser, args.filename, bytecode_data)

        print("Data sent.")

        ser.close()
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    main()
