import os

CHUNK_SIZE_MB = 20  # adjust as needed
THRESHOLD_MB = 50   # split files larger than this
ROOT = "."          # root folder to scan

def split_file(path, chunk_size_mb=20):
    chunk_size = chunk_size_mb * 1024 * 1024
    base = os.path.basename(path)
    folder = os.path.dirname(path)

    with open(path, "rb") as f:
        i = 0
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            out_path = os.path.join(folder, f"{base}.part{i}")
            with open(out_path, "wb") as out:
                out.write(chunk)
            i += 1

    print(f"[SPLIT] {path} → {i} parts")

def scan_and_split(root):
    threshold_bytes = THRESHOLD_MB * 1024 * 1024

    for dirpath, _, filenames in os.walk(root):
        for name in filenames:
            full_path = os.path.join(dirpath, name)
            size = os.path.getsize(full_path)

            if size > threshold_bytes:
                print(f"[FOUND LARGE FILE] {full_path} ({size/1024/1024:.2f} MB)")
                split_file(full_path, CHUNK_SIZE_MB)
                # remove original file after splitting
                os.remove(full_path)
                print(f"[REMOVED ORIGINAL] {full_path}")

if __name__ == "__main__":
    scan_and_split(ROOT)
