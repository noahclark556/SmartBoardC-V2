import subprocess
import time

last_pressed = {}
DEBOUNCE_TIME = 0.7

def volume_up():
    subprocess.run(["amixer", "set", "PCM", "10%+"], check=True)

def volume_down():
    subprocess.run(["amixer", "set", "PCM", "10%-"], check=True)

def mute():
    subprocess.run(["amixer", "cset", "numid=5", "off"], check=True)

def unmute():
    subprocess.run(["amixer", "cset", "numid=5", "on"], check=True)

def handle_keypress(button):
    global last_pressed

    current_time = time.time()
    if button in last_pressed and (current_time - last_pressed[button]) < DEBOUNCE_TIME:
        return

    last_pressed[button] = current_time
    try:
        if button == "KEY_RESTART":
            print("Restart button pressed! Restarting sb service...")
            subprocess.run(["sudo", "systemctl", "restart", "sb_app.service"], check=True)
        elif button == "KEY_CHANNELUP":
            print("Channel Up pressed! Unmuting...")
            unmute()
        elif button == "KEY_CHANNELDOWN":
            print("Channel Down pressed! Muting...")
            mute()
        elif button == "KEY_VOLUMEUP":
            print("Volume Up pressed!")
            volume_up()
        elif button == "KEY_VOLUMEDOWN":
            print("Volume Down pressed!")
            volume_down()
        elif button == "KEY_0":
            print("Number 0 pressed! Shutting down...")
            subprocess.run(["sudo", "shutdown", "now"], check=True)
        elif button == "KEY_1":
            print("Number 1 pressed! Restarting raspberry pi...")
            subprocess.run(["sudo", "reboot"], check=True)
        elif button == "KEY_2":
            print("Number 2 pressed!")
        else:
            print(f"Unhandled key: {button}")
    except Exception as e:
        print(f"Error handling keypress: {e}")

def listen_for_remote():
    process = subprocess.Popen(["irw"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    print("Listening for remote key presses...")

    for line in process.stdout:
        parts = line.split()
        print(parts)
        if len(parts) > 1:
            button = parts[2]
            handle_keypress(button)

if __name__ == "__main__":
    listen_for_remote()
