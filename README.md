# Smart Board OS

**Written by**: Noah Clark 

**Date Created**: 2024-06-01

**Last Updated**: 2025-01-23

**Language**: C/C++/Python

**Target Platform**: Raspberry Pi Zero 2W -> 64-bit Raspberry Pi OS Lite

**Development Environment**: macOS Sequoia 15.2, MacBook M2 Pro 2023 14-inch

**External Hardware**: Jabra Speak 510 (Mic and Speaker), Micro USB to USB 2.0 Cable/Adapter, HDMI Mini to HDMI Cable/Adapter, LCD Panel w/supporting controller. 

## Description
This is a custom "operating system" written in C intended for use with the Raspberry Pi Zero 2W and all newer models that support Raspberry Pi OS Lite (or most lite linux distros). This project uses SDL2 for graphics with the vc4-kms-v3d driver which is a kernel mode setting / 3D graphics driver, as part of the modern linux kernels direct rendering manager stack. 

The entirety of the UI and API calls are written in C. The only exception is the voice daemon which is written in Python. The voice daemon is compiled into an executable for the target platform and is started as a subprocess during the OS initialization process.

The voice daemon will listen for the key phrase "hey smart board". When the key phrase is detected, the daemon will write a command to a file called vdout.qdll. The main application is actively listening for changes to that file, when a change is detected, it will read the command and execute code accordingly (change window view, get weather, etc). These commands are fully customizable, for example, I could add a command that sends some voltage to a GPIO pin on the RPI and turn on a light.

It is HIGHLY recommended to use the Jabra Speak 510 for the microphone and speaker. It is a bluetooth and usb device that connects to the RPI and is very easy to setup. It can pickup voice commands from far away and has a very good speaker. It can be expensive, but I got a used one on ebay for $30 that works great.

## Intent and Predecessors
This project has been ongoing and is considered to be Version 2 of the Smart Board OS. Version 1 was written in Flutter as a Web App. The Web App would load on boot and run in the minimal chromium browser on RPI OS Lite. That version worked well, but only on the newer (and larger) Raspberry Pi models. My goal was for this project to be compact, so in order to make the OS run smoothly on the smaller RPI Zero 2W, I had to write the OS in C and manage memory manually.

## Initial Setup For Development
- [ ] In "/config" create a file called "api.h" and add the following:
```
#define FUNCTION_URL "BELOW_DATABASE_FUNCTION_URL_HERE"
```
- [ ] Create a google cloud function, linked to a firebase database, using the following:
```
export const getSBData = onRequest(async (req, res) => {
    const { userId } = req.body || {};
    try {
        const usersRef = db.collection('users').doc(userId);
        const userSnap = await usersRef.get();
        if (userSnap.exists) {
            const userData = userSnap.data();
            if (userData) {
                res.json(userData); // Send the user data as a JSON response
                return; // Ensure no further code is executed
            }
        }
        // If user does not exist, return a default response
        res.json({ agendas: {}, history: [], note: "" });
    } catch (e) {
        console.error("Error fetching user data:", e);
        res.status(500).json({ error: "Internal Server Error" }); // Send an error response
    }
});
```
- [ ] Create a firestore database in this format:
```
users(collection):
  userId(document):
    agendas<Map<Map<String, String>>>: // Manually insert data 
      ex: 
        {
          "agendas": {
            "2025-01-01": {
               "0800": "Meeting with John Doe",
               "0900": "Meeting with Jane Doe"
            }
          }
        }
    history<Array<String>>: // Auto updated by the program
    note<String>: // Manually insert data 
```
- [ ] Modify or Add /daemon/api.py and add the following:
```
openai_key = "OPENAI_KEY_HERE" // for openai backend
google_api_key = "GOOGLE_API_KEY_HERE" // for text to speech
```
---

## Todos:
### ASAP - (Pressing to ensure core functionality)
- [ ] Handle response text overflow.
- [ ] Too much database data causes the program to crash, especially on linux.
- [ ] Add follow on listening for after "hey smart board"
   - [ ] Ensure timer has started to detect if user is done speaking.
   - [ ] Add filter to ensure bogus words / background noise is not detected.
### Future - (Not Pressing)
- [ ] Switch things in cfg.h to use define, and move things out that shouldn't be for configuration.
- [ ] Let the RPI draw power from the LCD controller, eliminating the need for a separate power cable.
- [ ] Update icons to be more modern, aesthetically pleasing, and consistent. Potentially create custom icon set, but this is not a priority until core functionality is complete.
- [ ] Add date check for agenda and notes.
- [ ] Eventually convert the daemon into a C Array, which is included in the compiled binary.
- [ ] Implement mathematical operation to adjust font and dimensions based on screen resolution.
   - [ ] Need to make element positions relative to previous elements, that way only one value needs to be changed.
- [ ] Base weather data off of device physical location. (It is hardcoded to a placeholder right now, but weather data is accurate for the placeholder location)
- [ ] Build casing for smart board. Potentially 3D print.
   - [ ] Durable and still keep the thin profile.
   - [ ] Space for RPI Zero 2W, with easy access to ports and cables, while still keeping the cables managed.
   - [ ] Space for usb microphone to be plugged in and extend outward so it is somewhere it can hear the user well enough.
   - [ ] Exposed LCD ports as well to switch to a standard monitor (roku, dual monitor, etc).
   - [ ] Cooling system for RPI, needs airflow (holes back casing).
   - [ ] Maintain modern look, use plexiglass or similat where possible, but keep the ugly parts of the LCD panel hidden.
   - [ ] IMPORTANT: Needs a way to protect the sensitive strip on the bottom of the LCD panel. Ideally some sort of space to insert the black strip into, preventing anything from causing it damage.
   - [ ] Summary: Essentially just build a monitor case but have it include space for the RPI Zero 2W, microphone, and HDMI ports to be placed in a way that is functional, aesthetically pleasing and modifyable.

## Notes:
- [ ] Modify ./config/cfg.c and change the windowWidth and windowHeight to match the hdmi_cvt value in 
/boot/config.txt (sudo nano /boot/firmware/config.txt).

## Mac Development Setup

### Install Dependencies
Use Homebrew to install the required libraries:
```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

### Verify SDL2 Installation
Check the installed SDL2 version:
```bash
sdl2-config --version
```

### Compile and Run the Project
Instead of running the following command directly, use the `./build.sh` script to compile:
```bash
gcc -o sb_app $(find . -name "*.c") $(sdl2-config --cflags --libs) \
-lSDL2_ttf -lSDL2_gfx -lcurl -lcjson -lSDL2_image -v && ./sb_app
```
**Note**: Use `-l[package]` in the `gcc` command to include additional libraries.

### Compile and Run the Daemon
Change to the `daemon` directory and run:
```bash
./build.sh
```
**Note**: Will need to pip install dependencies including pyinstaller. Shouldn't be too many needed.
**Note**: Modify build.sh to use "python3 -m PyInstaller --onefile voice_daemon.py" for linux. Case sensitive.

**Production Build Notes**:
- Set the `DEV_BUILD` variable in `build.sh` to `0`.
- Update the `COMPILED` variable in `voice_daemon.py` to `1`.

---

## Raspberry Pi Transfer and Compile

### Install Dependencies
On the Raspberry Pi, install the required libraries:
```bash
sudo apt update && sudo apt install -y \
build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
libsdl2-gfx-dev libcurl4-openssl-dev libcjson-dev

sudo apt install python3-pip
pip install --break-system-packages pyinstaller
pip install --break-system-packages SpeechRecognition
sudo apt install portaudio19-dev
pip install --break-system-packages pyaudio
pip install --break-system-packages simpleaudio
pip install --break-system-packages openai
sudo apt-get install flac
```

### Setup Jabra Speak 510
List recording & playback devices, note device number:
```bash
arecord -l (List recording devices)
aplay -l (List playback devices)
```

Edit /etc/asound.conf to match the device number:
```bash
sudo nano /etc/asound.conf
defaults.pcm.card 1
defaults.ctl.card 1
defaults.pcm.device 0
```

Restart alsa:
```bash
sudo systemctl restart alsa-restore
```

Note: voice_daemon.py has an API call to cloud text to speech, the sample rate is set to 16000 which is required for the Jabra Speak 510 to play back the audio.

### Transfer Source Code from macOS to Raspberry Pi
On your Mac, navigate to the root directory of the project and run:
```bash
scp -r ./* noahclark556@10.0.0.118:/home/noahclark556/SB-OS
```

### Compile and Run the Project on Raspberry Pi
Once transferred, navigate to the root directory of the project on the Raspberry Pi.
Instead of running the following command directly, use the `./build.sh` script to compile:
```bash
gcc -o sb_app $(find . -name "*.c") $(sdl2-config --cflags --libs) \
-lSDL2_ttf -lSDL2_gfx -lcurl -lcjson -lSDL2_image -v && ./sb_app
```

### Compile the Daemon
Navigate to the `daemon` directory and run:
```bash
./build.sh
sudo chmod +x voice_daemon
```
**Note**: Modify build.sh to use "python3 -m PyInstaller --onefile voice_daemon.py" for linux. Case sensitive.

---

## Debugging

### Compile with Debug Symbols
To compile with debug symbols for debugging purposes:
```bash
gcc -g -o sb_app $(find . -name "*.c") $(sdl2-config --cflags --libs) \
-lSDL2_ttf -lSDL2_gfx -lcurl -lcjson -lSDL2_image -v
```

### Debug with GDB
Run the debugger:
```bash
gdb sb_app
run
```

If an error occurs, use:
```bash
backtrace
```

---

## Run the Compiled Program on Startup

1. Create a systemd service file:
   ```bash
   sudo nano /etc/systemd/system/sb_app.service
   ```

2. Add the following configuration:
   ```ini
   [Unit]
   Description=Smart Board Application
   After=multi-user.target

   [Service]
   ExecStart=/home/noahclark556/sb_os/sb_app
   Restart=always
   User=noahclark556
   WorkingDirectory=/home/noahclark556/sb_os
   Environment="SDL_VIDEODRIVER=kmsdrm"
   Environment="DISPLAY=:0"

   [Install]
   WantedBy=multi-user.target
   ```

3. Enable and start the service:
   ```bash
   sudo systemctl enable sb_app.service
   sudo systemctl start sb_app.service
   ```

---

## Raspberry Pi Boot Configuration

For optimal performance, use the following configuration in `/boot/config.txt`:
```ini
hdmi_force_hotplug=1
hdmi_group=2
hdmi_mode=87
hdmi_cvt=1920 1080 60 6 # 1920 1080 60 6 for the new monitor
dtoverlay=vc4-kms-v3d
gpu_mem=128
#disable_fw_kms_setup=1 # Comment this one out
```

---

## Additional Notes
If you encounter issues during setup or runtime, refer to `lin_notes.txt` for troubleshooting tips.
