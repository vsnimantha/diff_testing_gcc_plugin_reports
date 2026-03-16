import os
import re

ROOT = "."

def join_parts(base_path):
    output_path = base_path
    folder = os.path.dirname(base_path)
    base_name = os.path.basename(base_path)

    with open(output_path, "wb") as out:
        i = 0
        while True:
            part_path = os.path.join(folder, f"{base_name}.part{i}")
            if not os.path.exists(part_path):
                break
            with open(part_path, "rb") as part:
                out.write(part.read())
            i += 1

    print(f"[REBUILT] {output_path} from {i} parts")

    # optional: remove parts
    for j in range(i):
        os.remove(os.path.join(folder, f"{base_name}.part{j}"))
    print(f"[CLEANED PARTS] {base_name}.part*")

def scan_and_rebuild(root):
    part_pattern = re.compile(r"(.+)\.part0$")

    for dirpath, _, filenames in os.walk(root):
        for name in filenames:
            if name.endswith(".part0"):
                base = name[:-6]  # remove ".part0"
                base_path = os.path.join(dirpath, base)
                join_parts(base_path)

if __name__ == "__main__":
    scan_and_rebuild(ROOT)
