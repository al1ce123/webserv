#!/usr/bin/env python3.12

import time

def countdown(seconds):
    for i in range(seconds, 0, -1):
        print(f"Time remaining: {i} seconds")
        time.sleep(1)

def main():
    countdown(5)
    print("Done waiting!")

if __name__ == "__main__":
    main()