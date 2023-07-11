import argparse
import os


def main():
    parser = argparse.ArgumentParser(
        prog="Include Generator",
        description="Creates a list of includes for all files in a given directory and subdirectories",
    )
    parser.add_argument("path")
    args = parser.parse_args()

    if not os.path.isdir(args.path):
        print(f"{args.path} is not a directory")

    prefix = os.path.relpath(args.path, 'include')

    for root, dirs, files in os.walk(args.path):
        for header in files:
            if header.endswith(".hpp") or header.endswith(".h"):
                full_header = os.path.join(root, header)
                rel_path = os.path.relpath(full_header, args.path)
                print(f"#include <{prefix}/{rel_path}>")


if __name__ == "__main__":
    main()
