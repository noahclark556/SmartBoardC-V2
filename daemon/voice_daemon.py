import speech_recognition as sr
import time
import base64
import requests
import simpleaudio as sa
from openai import OpenAI
import threading
import os
import api as api

client = OpenAI(api_key=api.openai_key)

# If using python to start the daemon, set to compiled to 0
COMPILED = True
LISTENER_INTERVAL = .1 # Lower this to .5 (500ms) in production, maybe?
api_key = api.google_api_key
url = f'https://texttospeech.googleapis.com/v1/text:synthesize?key={api_key}'
inSpeechMode = False
tospeak = []
speaking = False
prepend = "VDaemon ->"
output_file = "vdout.qdll"
fullresponse = ""
response_output_file = "response.qdll"
p_threshold = 1.2
newline_padding = 80 # \n after every x characters

if COMPILED:
    print(f"{prepend} Using compiled output file")
    output_file = "./daemon/vdout.qdll"
    response_output_file = "./daemon/response.qdll"
else:
    print(f"{prepend} Using uncompiled output file")
    output_file = "vdout.qdll"
    response_output_file = "response.qdll"

def delete_output_file(output_file):
    if os.path.isfile(output_file):  # Check if the file exists
        os.remove(output_file)  # Delete the file
        print(f"Deleted file: {output_file}")
    else:
        print(f"File not found: {output_file}")

def speak(text):
    global speaking
    # Request payload
    payload = {
        "input": {"text": text},
        "voice": {
            "languageCode": "en-US",
            "name": "en-US-Neural2-H"
        },
        "audioConfig": {
            "audioEncoding": "LINEAR16",
            "speakingRate": 1.2,
            "pitch": 0.5
        }
    }

    try:
        # Make the API request
        response = requests.post(url, json=payload, headers={'Content-Type': 'application/json'})

        if response.status_code == 200:
            response_data = response.json()
            audio_content = response_data.get('audioContent')  # Base64 audio content

            if audio_content:
                speaking = True
                # Decode Base64 to binary MP3 data
                audio_data = base64.b64decode(audio_content)

                # Write to a temporary MP3 file
                with open('output.mp3', 'wb') as audio_file:
                    audio_file.write(audio_data)

                # Play the MP3 file
                wave_obj = sa.WaveObject.from_wave_file('output.mp3')
                play_obj = wave_obj.play()
                play_obj.wait_done()  # Wait for playback to finish
                speaking = False
            else:
                print("No audio content received")
        else:
            print(f"Error: {response.status_code} - {response.text}")
    except Exception as e:
        print(f"Error occurred: {e}")

def getLineParsedResponse():
    global fullresponse
    noln = fullresponse.replace("\n", "")
    new_string = '\n'.join(noln[i:i+newline_padding] for i in range(0, len(noln), newline_padding))
    return new_string
    

def writeFullResponse():
    res = getLineParsedResponse()
    with open(response_output_file, "w") as f:
        f.write(res)
        f.close()

def speakListener():
    global tospeak
    while len(tospeak) > 0:
        writeFullResponse()
        speak(tospeak.pop(0))

def isSpeaking():
    if len(tospeak) == 0 and speaking:
        return True
    if len(tospeak) == 0 and not speaking:
        return False
    if len(tospeak) > 0 and speaking:
        return True
    return False

def enterSilentMode():
    global inSpeechMode
    if isSpeaking():
        return
    with open(output_file, "w") as f:
        f.write("silentmode")
        f.close()
    print("Silent mode enabled")
    inSpeechMode = False

def goBackListener():
    if isSpeaking():
        return
    timer = threading.Timer(7, enterSilentMode)
    timer.start()
    

def eos(text):
    chars_to_check = ['.', '!', '?', '...']
    for char in chars_to_check:
        if char in text:
            return True
    return False

def stream_gpt4_response(command):
    global tospeak
    global fullresponse
    fullresponse = ""
    try:
        current = ""
        # Make a streaming request to the GPT-4 API
        response = client.chat.completions.create(model="gpt-4",  # Use GPT-4 model
        messages=[
            {"role": "system", "content": "You are a helpful assistant."},
            {"role": "user", "content": command},
        ],
        stream=True)

        print("Response:")
        # Process the streamed response
        for chunk in response:
            if chunk.choices[0].delta.content is not None:
                current += chunk.choices[0].delta.content
                fullresponse += chunk.choices[0].delta.content
                if eos(chunk.choices[0].delta.content):
                    tospeak.append(current)
                    speakListener()
                    current = ""
        print("\nStream finished.")
        goBackListener()
    except Exception as e:
        print(f"An error occurred: {e}")

def start_voice_daemon():
    global inSpeechMode
    recognizer = sr.Recognizer()
    mic = sr.Microphone()
    recognizer.pause_threshold = p_threshold
    delete_output_file(output_file)


    print(f"{prepend} Starting voice daemon")
    with mic as source:
        recognizer.adjust_for_ambient_noise(source)
        print(f"{prepend} Listening...")
        while True:
            try:
                audio = recognizer.listen(source)
                command = recognizer.recognize_google(audio)
                print(f"{prepend} Command received: {command}")
                if inSpeechMode:
                    if isSpeaking():
                        continue
                    if(command.lower() == "stop" or command.lower() == "nevermind" or command.lower() == "never mind" or command.lower() == "go back"):
                        enterSilentMode()
                        continue
                    stream_gpt4_response(command)
                if not inSpeechMode and ("hey smart board" in command.lower() or "hey smartboard" in command.lower()):
                    with open(output_file, "w") as f:
                        f.write("speechmode")
                        f.close()
                    print("Speech mode enabled")
                    with open(response_output_file, "w") as f:
                        f.write("Hello, how can I help you today?")
                        f.close()
                    speak("Hello, how can I help you today?")
                    inSpeechMode = True

            except sr.UnknownValueError:
                
                print(f"{prepend} Cant understand audio")
            except sr.RequestError:
                
                print(f"{prepend} Could not request results; {e}")

            time.sleep(LISTENER_INTERVAL)


start_voice_daemon()
# stream_gpt4_response("Write me a 2 paragraph story?")