import time

import serial
from serial import SerialException


PORT = "COM11"
BAUD_RATE = 115200


# Each face has a small list of Chinese and English keywords.
# The program checks these words from top to bottom.
KEYWORDS = {
    "happy": ["开心", "好耶", "成功", "牛逼", "happy", "great"],
    "sad": ["难过", "哭", "害怕", "sad", "sorry"],
    "think": ["想", "思考", "为什么", "怎么", "what", "why", "how"],
    "angry": ["生气", "烦", "讨厌", "气死", "angry"],
    "sleep": ["困", "睡", "累", "sleep", "tired"],
}


def decide_face(text):
    """Choose one OLED face command from the user's sentence."""
    lower_text = text.lower()

    for face, words in KEYWORDS.items():
        for word in words:
            if word.lower() in lower_text:
                return face

    return "neutral"


def send_face(board, face):
    """Send one face command to the ESP32-C3."""
    board.write((face + "\n").encode("utf-8"))
    board.flush()


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

    print("Type a Chinese or English sentence.")
    print("Examples: 我好开心 / 烦死了 / 我想睡觉 / how does this work")
    print("Type quit to exit.")

    try:
        while True:
            text = input("> ").strip()

            if text.lower() == "quit":
                print("Bye.")
                break

            if not text:
                continue

            face = decide_face(text)

            try:
                send_face(board, face)
                sent_ok = True
            except SerialException:
                sent_ok = False

            print("sentence:", text)
            print("face:", face)
            print("sent:", "success" if sent_ok else "failed")

            if not sent_ok:
                print("Please close Arduino Serial Monitor, or check that the ESP32-C3 is on COM11.")

    except KeyboardInterrupt:
        print("\nBye.")
    finally:
        board.close()


if __name__ == "__main__":
    main()
