import sys
import struct
import zlib

HEADER_MAGIC_0 = 0xFF01FF02
HEADER_MAGIC_1 = 0xFF03FF04

def calc_crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF

def patch_binary(input_bin_path: str, output_bin_path: str):
    with open(input_bin_path, "rb") as f:
        raw_data = f.read()

    # Extract MSP and Reset Vector from raw vector table
    msp, reset_vector = struct.unpack("<II", raw_data[:8])
    payload = raw_data[32:] # Extract payload beyond vector metadata

    image_size = len(raw_data)
    crc32_val = calc_crc32(payload)
    git_hash = 0x12345678  # Example SHA tag placeholder

    # Construct 32-Byte Header Structure
    header = struct.pack(
        "<IIIIIIII",
        msp,
        reset_vector,
        HEADER_MAGIC_0,
        HEADER_MAGIC_1,
        image_size,
        crc32_val,
        git_hash,
        0xFFFFFFFF  # Reserved
    )

    signed_bin = header + payload

    with open(output_bin_path, "wb") as f:
        f.write(signed_bin)

    print(f"Patched Header: Size={image_size} Bytes, CRC32=0x{crc32_val:08X}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python append_crc.py <input_app.bin> <output_app_signed.bin>")
        sys.exit(1)
    patch_binary(sys.argv[1], sys.argv[2])