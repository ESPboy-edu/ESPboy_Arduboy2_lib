"""
=============================================================================
ArduboyFX Paged LZSS Compressor for ESPboy
=============================================================================
Description:
    Compresses raw binary assets (fxdata.bin) into 4 KB LZSS-compressed
    pages with a page-offset lookup table header.
    Used for Mode 3 (Paged Dynamic LZSS Mode for massive games like Dark & Under II).

Usage Instructions:

macOS / Linux:
    python3 lzss_compress.py <input.bin> <output.bin>
    Example:
        python3 lzss_compress.py fxdata.bin fxdata_paged.bin

Windows:
    python lzss_compress.py <input.bin> <output.bin>
    Example:
        python lzss_compress.py fxdata.bin fxdata_paged.bin
=============================================================================
"""

"""
=============================================================================
ArduboyFX Paged LZSS Compressor for ESPboy (Fixed Match Engine)
=============================================================================
"""



import sys
import os
import struct

PAGE_SIZE = 4096

def compress_page(page_data):
    out = bytearray()
    i = 0
    length = len(page_data)

    # Обработка блоков ровно по 8 элементов
    while i < length:
        flags = 0
        tokens = bytearray()

        for bit in range(8):
            if i >= length:
                break

            best_len = 0
            best_pos = 0
            max_search = min(length - i, 255 + 15 + 3)

            # Точный поиск совпадений LZSS
            if i > 0:
                for pos in range(0, i):
                    l = 0
                    while l < max_search and page_data[i + l] == page_data[pos + l]:
                        l += 1
                    if l > best_len:
                        best_len = l
                        best_pos = pos
                        if best_len == max_search:
                            break

            if best_len >= 3:
                # Совпадение (бит = 0)
                l_val = best_len - 3
                if l_val >= 15:
                    tokens.append((15 << 4) | (best_pos >> 8))
                    tokens.append(best_pos & 0xFF)
                    rem = l_val - 15
                    while rem >= 255:
                        tokens.append(255)
                        rem -= 255
                    tokens.append(rem)
                else:
                    tokens.append((l_val << 4) | (best_pos >> 8))
                    tokens.append(best_pos & 0xFF)
                i += best_len
            else:
                # Литерал (бит = 1)
                flags |= (1 << bit)
                tokens.append(page_data[i])
                i += 1

        out.append(flags)
        out.extend(tokens)

    return out

def verify_page(original, compressed):
    """ Проверка правильности распаковки блока """
    decomp = bytearray()
    ptr = 0
    while len(decomp) < len(original) and ptr < len(compressed):
        flags = compressed[ptr]
        ptr += 1
        for bit in range(8):
            if len(decomp) >= len(original) or ptr >= len(compressed):
                break
            if flags & (1 << bit):
                decomp.append(compressed[ptr])
                ptr += 1
            else:
                b1 = compressed[ptr]
                b2 = compressed[ptr + 1]
                ptr += 2
                match_pos = ((b1 & 0x0F) << 8) | b2
                match_len = (b1 >> 4)
                if match_len == 15:
                    while True:
                        ext = compressed[ptr]
                        ptr += 1
                        match_len += ext
                        if ext != 255:
                            break
                match_len += 3
                for _ in range(match_len):
                    if len(decomp) >= len(original):
                        break
                    decomp.append(decomp[match_pos])
                    match_pos += 1
    return bytes(decomp) == original

def main():
    if len(sys.argv) < 3:
        print("Usage: python lzss_compress.py <input.bin> <output.bin>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    if not os.path.exists(input_path):
        print(f"Error: File '{input_path}' not found.")
        sys.exit(1)

    print(f"Reading '{input_path}'...")
    with open(input_path, "rb") as f:
        raw_data = f.read()

    total_pages = (len(raw_data) + PAGE_SIZE - 1) // PAGE_SIZE
    print(f"Total logical pages ({PAGE_SIZE} bytes each): {total_pages}")

    compressed_pages = []
    for page_idx in range(total_pages):
        chunk = raw_data[page_idx * PAGE_SIZE : (page_idx + 1) * PAGE_SIZE]
        comp_chunk = compress_page(chunk)

        if not verify_page(chunk, comp_chunk):
            print(f"CRITICAL ERROR: Decompression test failed on page {page_idx}!")
            sys.exit(1)

        compressed_pages.append(comp_chunk)
        if page_idx % 50 == 0 or page_idx == total_pages - 1:
            print(f"Compressed & Verified {page_idx + 1}/{total_pages} pages...")

    header_size = 4 + (total_pages * 4)
    current_offset = header_size

    offsets = []
    for comp_chunk in compressed_pages:
        offsets.append(current_offset)
        current_offset += len(comp_chunk)

    print(f"Writing packed file '{output_path}'...")
    with open(output_path, "wb") as f:
        f.write(struct.pack("<I", total_pages))
        for offset in offsets:
            f.write(struct.pack("<I", offset))
        for comp_chunk in compressed_pages:
            f.write(comp_chunk)

    orig_sz = len(raw_data) / 1024.0
    new_sz = current_offset / 1024.0
    ratio = (current_offset / len(raw_data)) * 100.0

    print("\nSUCCESS! All pages compressed and verified without errors.")
    print(f"Original Size:   {orig_sz:.2f} KB")
    print(f"Compressed Size: {new_sz:.2f} KB")
    print(f"Ratio:           {ratio:.1f}%\n")

if __name__ == "__main__":
    main()