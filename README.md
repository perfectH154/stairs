# ESP32-C3 OLED Face Controller

This project controls a cute SSD1306 OLED face on an ESP32-C3 through USB serial.

The ESP32 only receives simple serial commands:

```text
happy
sad
think
angry
sleep
neutral
```

The Python scripts run on the computer and send those commands to the board.

## Hardware

- Board: ESP32-C3 Dev Module
- OLED: SSD1306, 128x32
- I2C address: `0x3C`
- SDA: GPIO5
- SCL: GPIO6
- Serial port: `COM11`
- Baud rate: `115200`

The Arduino sketch keeps the required I2C setup:

```cpp
Wire.begin(5, 6);
```

## Files

- `esp32_c3_oled_serial_face/esp32_c3_oled_serial_face.ino`
  - Arduino sketch for the ESP32-C3 OLED face.
- `send_face_command.py`
  - Manual serial control. Type `happy`, `sad`, `think`, `angry`, `sleep`, or `neutral`.
- `send_face_by_text.py`
  - Local keyword-based emotion detection. Type a Chinese or English sentence.
- `claude_face_chat.py`
  - Claude/CrazyRouter chat version. The model returns a reply and a face state.

## Arduino IDE Settings

Check these before uploading:

- Board: `ESP32C3 Dev Module`
- Port: `COM11`
- USB CDC On Boot: `Enabled`
- Serial Monitor baud rate: `115200`

Required Arduino libraries:

- Adafruit SSD1306
- Adafruit GFX Library
- Wire

## Upload The Arduino Sketch

Open this file in Arduino IDE:

```text
esp32_c3_oled_serial_face/esp32_c3_oled_serial_face.ino
```

Upload it to the ESP32-C3. Then open Serial Monitor and test:

```text
happy
sad
think
angry
sleep
neutral
```

The board should echo:

```text
face = happy
```

## Python Setup

Install dependencies:

```powershell
pip install -r requirements.txt
```

If `COM11` cannot be opened, close Arduino Serial Monitor first.

## Manual Serial Control

```powershell
python .\send_face_command.py
```

Then type:

```text
happy
sad
think
angry
sleep
neutral
quit
```

## Local Keyword Emotion Control

```powershell
python .\send_face_by_text.py
```

Example inputs:

```text
wo hao kai xin
fan si le
wo xiang shui jiao
how does this work
```

You can also type Chinese directly in a UTF-8 terminal.

## Claude / CrazyRouter Chat Control

Install the Anthropic SDK first:

```powershell
pip install anthropic
```

Set your API key as an environment variable. Do not write the key into code.

For official Anthropic:

```powershell
$env:ANTHROPIC_API_KEY="your_api_key_here"
python .\claude_face_chat.py
```

For CrazyRouter or another Anthropic-compatible relay:

```powershell
$env:ANTHROPIC_API_KEY="your_crazyrouter_key_here"
$env:ANTHROPIC_BASE_URL="https://cn.crazyrouter.com"
$env:CLAUDE_MODEL="claude-opus-4-6"
python .\claude_face_chat.py
```

The model must return JSON like this:

```json
{
  "reply": "Nice, the OLED face is happy now.",
  "face": "happy"
}
```

The Python script prints `reply` and sends `face` to the ESP32.

## Notes

- The ESP32 does not connect to Wi-Fi.
- The ESP32 does not call any AI API.
- API calls only happen on the computer through Python.
- API keys should stay in environment variables, not in files.
