import json
import os
import time

import serial
from serial import SerialException

try:
    from anthropic import Anthropic
except ModuleNotFoundError:
    Anthropic = None


PORT = "COM11"
BAUD_RATE = 115200

# If this model name is unavailable, you can change it with an environment variable:
# PowerShell example: $env:CLAUDE_MODEL="your_model_name"
MODEL = os.getenv("CLAUDE_MODEL", "claude-sonnet-4-6")

VALID_FACES = ["happy", "sad", "think", "angry", "sleep", "neutral"]

SYSTEM_PROMPT = """
你是一个会控制 OLED 表情屏的小助手。
你必须只返回 JSON，不要 Markdown，不要解释，不要代码块。

JSON 格式必须是：
{
  "reply": "给用户的简短中文回复",
  "face": "happy"
}

face 只能是下面之一：
happy, sad, think, angry, sleep, neutral

情绪规则：
- 用户开心、成功、兴奋、夸奖、好消息 -> happy
- 用户难过、害怕、焦虑、哭、委屈 -> sad
- 用户提问、疑惑、让你解释、问为什么/怎么做 -> think
- 用户生气、烦躁、吐槽、讨厌、崩溃 -> angry
- 用户困、累、想睡觉、晚安 -> sleep
- 其他 -> neutral

reply 要简短、自然、中文优先。
""".strip()


def open_serial():
    """Open the serial port connected to the ESP32-C3."""
    try:
        board = serial.Serial(PORT, BAUD_RATE, timeout=1)
    except SerialException:
        print("Cannot open COM11.")
        print("Please close Arduino Serial Monitor, or check that the ESP32-C3 is on COM11.")
        return None

    print("Connected to ESP32-C3 on COM11.")
    print("Waiting 2 seconds for ESP32 serial initialization...")
    time.sleep(2)
    return board


def ask_claude(client, user_text):
    """Send the user's sentence to Claude and return the raw text response."""
    message = client.messages.create(
        model=MODEL,
        max_tokens=300,
        system=SYSTEM_PROMPT,
        messages=[
            {
                "role": "user",
                "content": user_text,
            }
        ],
    )

    raw_text = ""
    for block in message.content:
        if getattr(block, "type", None) == "text":
            raw_text += block.text

    return raw_text.strip()


def parse_claude_json(raw_text):
    """Parse Claude's JSON. If parsing fails, use a neutral face."""
    try:
        data = json.loads(raw_text)
    except json.JSONDecodeError:
        print("Claude returned invalid JSON:")
        print(raw_text)
        return "", "neutral"

    reply = str(data.get("reply", "")).strip()
    face = str(data.get("face", "neutral")).strip().lower()

    if face not in VALID_FACES:
        face = "neutral"

    return reply, face


def send_face(board, face):
    """Send one face command to ESP32-C3, such as happy\\n."""
    board.write((face + "\n").encode("utf-8"))
    board.flush()


def main():
    if Anthropic is None:
        print("Anthropic SDK is not installed.")
        print("Please run: pip install anthropic")
        return

    api_key = os.getenv("ANTHROPIC_API_KEY")
    if not api_key:
        print("ANTHROPIC_API_KEY is not set.")
        print("Please set the environment variable before running this script.")
        print('PowerShell example: $env:ANTHROPIC_API_KEY="your_api_key_here"')
        return

    board = open_serial()
    if board is None:
        return

    # Official Anthropic API does not need a base URL.
    # API relay services usually provide their own Anthropic-compatible base URL.
    # PowerShell example: $env:ANTHROPIC_BASE_URL="https://your-relay.example.com"
    base_url = os.getenv("ANTHROPIC_BASE_URL")
    if base_url:
        client = Anthropic(api_key=api_key, base_url=base_url)
    else:
        client = Anthropic(api_key=api_key)

    print("Type a sentence. Claude will reply and choose an OLED face.")
    print("Type quit to exit.")

    try:
        while True:
            user_text = input("> ").strip()

            if user_text.lower() == "quit":
                print("Bye.")
                break

            if not user_text:
                continue

            try:
                raw_text = ask_claude(client, user_text)
                reply, face = parse_claude_json(raw_text)
            except Exception as error:
                print("Claude API request failed:")
                print(error)
                reply = ""
                face = "neutral"

            if reply:
                print("Claude:", reply)
            else:
                print("Claude: no reply")

            try:
                send_face(board, face)
                print("face:", face)
                print("sent: success")
            except SerialException:
                print("face:", face)
                print("sent: failed")
                print("Please close Arduino Serial Monitor, or check that the ESP32-C3 is on COM11.")

    except KeyboardInterrupt:
        print("\nBye.")
    finally:
        board.close()


if __name__ == "__main__":
    main()
