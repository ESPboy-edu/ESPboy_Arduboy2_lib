"""
=============================================================================
ArduboyFX RLE Compressor for ESPboy (Updated with C++ Defines Output)
=============================================================================
Description:
    Compresses raw binary assets (fxdata.bin) using ArduboyFX RLE algorithm.
    Outputs exact byte sizes and ready-to-use C++ #define macros for Mode 2.

Usage Instructions:

macOS / Linux:
    python3 rle_compress.py <input.bin> <output.bin>
    Example:
        python3 rle_compress.py fxdata.bin fxdta_rle.bin

Windows:
    python rle_compress.py <input.bin> <output.bin>
    Example:
        python rle_compress.py fxdata.bin fxdta_rle.bin
=============================================================================
"""

import sys
import os

def compress_rle(data):
    out = bytearray()
    i = 0
    n = len(data)

    while i < n:
        # Check run length of repeated bytes
        run_len = 1
        while i + run_len < n and run_len < 127 and data[i] == data[i + run_len]:
            run_len += 1

        # If 3 or more identical bytes, encode as RLE run
        if run_len >= 3:
            out.append(0x80 | run_len)
            out.append(data[i])
            i += run_len
        else:
            # Collect literal bytes
            lit_start = i
            while i < n and (i - lit_start) < 127:
                # Stop if a run of 3 identical bytes is found
                if i + 2 < n and data[i] == data[i + 1] == data[i + 2]:
                    break
                i += 1
            
            lit_len = i - lit_start
            if lit_len > 0:
                out.append(lit_len & 0x7F)
                out.extend(data[lit_start:i])

    return out

def main():
    if len(sys.argv) < 3:
        print("Error: Missing arguments.")
        print("Usage: python rle_compress.py <input.bin> <output.bin>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    if not os.path.exists(input_path):
        print(f"Error: File '{input_path}' not found.")
        sys.exit(1)

    print(f"Reading '{input_path}'...")
    with open(input_path, "rb") as f:
        raw_data = f.read()

    print("Compressing using RLE...")
    compressed = compress_rle(raw_data)

    print(f"Saving to '{output_path}'...")
    with open(output_path, "wb") as f:
        f.write(compressed)

    orig_bytes = len(raw_data)
    comp_bytes = len(compressed)
    orig_kb = orig_bytes / 1024.0
    comp_kb = comp_bytes / 1024.0
    ratio = (comp_bytes / orig_bytes) * 100.0

    print("\n-------------------------------------------------------------")
    print("COMPRESSION SUMMARY")
    print("-------------------------------------------------------------")
    print(f"Original Size:   {orig_bytes} bytes ({orig_kb:.2f} KB)")
    print(f"Compressed Size: {comp_bytes} bytes ({comp_kb:.2f} KB)")
    print(f"Ratio:           {ratio:.1f}%")
    print("-------------------------------------------------------------")
    print("Copy & Paste these defines into your project (ArduboyFX.h / fxdta.h):")
    print("-------------------------------------------------------------")
    print(f"#define UNPACKED_FILE_SIZE {orig_bytes}")
    print(f"#define RLE_FILE_SIZE      {comp_bytes}")
    print("-------------------------------------------------------------\n")

if __name__ == "__main__":
    main()