import re
import os

improve = "^[ \t]*// IMPROVE: \[([a-zA-Z0-9_]+)\] (.+)$"
debug = "^[ \t]*// DEBUG: \[([a-zA-Z0-9_]+)\] (.+)$"

def browse_code(directories):
    found = []
    for directory in directories:
        for dirpath, dirnames, files in os.walk(directory):
            for name in files:
                found.append(os.path.join(dirpath, name))
    return found;


def fetch_tag(files, regex):
    for fpath in files:
        f = open(fpath, encoding='utf-8', errors='ignore')
        lines = f.readlines()
        f.close()
        for line in lines:
            m = re.search(regex, line)
            if (m):
                print(f"- [ ] **{m.group(1)}**: {m.group(2)}")


files = browse_code([
    "./basic_buffers", 
    "./VFS"
])

fetch_tag(files, improve)
print("=========")
fetch_tag(files, debug)