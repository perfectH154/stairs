import time

import serial
from serial import SerialException


PORT = "COM11"
BAUD_RATE = 115200
VALID_COMMANDS = ["happy", "sad", "think", "angry", "sleep", "neutral"]


def main():
    try:
        board = serial.Serial(PORT, BAUD_RATE, timeout=1)
    except SerialException:
        print("Cannot open COM11.")
        print("Please close Arduino Serial Monitor, or check that the ESP32-C3 is on COM11.")
        return

    print("Connected to ESP32-C3 on COM11.")
    print("Waiting 2 seconds for serial initialization...")
    time.sleep(2)

    print("Type a command: happy / sad / think / angry / sleep / neutral")
    print("Type quit to exit.")

    try:
        while True:
            command = input("> ").strip().lower()

            if command == "quit":
                print("Bye.")
                break

            if command not in VALID_COMMANDS:
                print("Unknown local command. Try: happy / sad / think / angry / sleep / neutral")
                continue

            board.write((command + "\n").encode("utf-8"))
            print("sent:", command)

            # Read one reply line from the board, such as: face = happy
            reply = board.readline().decode("utf-8", errors="ignore").strip()
            if reply:
                print("board:", reply)

    except KeyboardInterrupt:
        print("\nBye.")
    finally:
        board.close()


if __name__ == "__main__":
    main()
