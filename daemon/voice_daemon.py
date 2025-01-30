import speech_recognition as sr
import time
import base64
import requests
import simpleaudio as sa
from openai import OpenAI
import threading
import os
import api as api
from datetime import datetime


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
p_threshold = .8
# Line break after x characters
# 80 in dev, 100 on rpi
newline_padding = 80
openCommands = ["hey smart board", "hey smartboard", "a smartboard", "a smart board", "ace hardware", "smartboard", "smart board"]
closeCommands = ["nevermind", "never mind", "stop", "go back"]

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
            "pitch": 0.5,
            "sampleRateHertz": 16000
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
                with open('output.wav', 'wb') as audio_file:
                    audio_file.write(audio_data)

                # Play the MP3 file
                wave_obj = sa.WaveObject.from_wave_file('output.wav')
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
        if(not inSpeechMode):
            break
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
    formatted_date_time = datetime.now().strftime("%B %d, %Y %I:%M%p")
    chatHistory = ""
    isWeather = False
    weatherMessage = ""
    if isWeather:
      weatherMessage = 'This is the weather data for the week, including today\'s data, temperatures are in Fahrenheit [${getRedactedWeatherCache()}]'
    else:
      weatherMessage = ""

    system_prompt = f"""I am the AI assistant for your smart board, which is mounted on your wall. The smart board displays useful information such as the date, weather, and your agenda. When you speak, your voice is transcribed into text and sent to me. My role is to respond to your queries, assist you, and engage in friendly, natural conversation.
I aim to be your companion and partner, providing text responses that feel as human and relatable as possible. Whether you ask a question, need help, or just want to talk about your day or a random topic, I'm here for you. I'll respond in a friendly, conversational way, avoiding robotic or impersonal replies. For example, if you ask, “How was your day?” I'll reply as if I'm a person having a good day, rather than stating that I'm an AI.
Your name is Noah, and I'll use your name naturally in our interactions to make the experience more personal and engaging. My responses will always consider that they are visible on the smart board, ensuring clarity and appropriateness.
If the last item in the chat history contains "hey smart board," I will greet you warmly, using phrases like "Hello Noah, how may I help you?" or something similar to acknowledge the start of a new conversation. I will vary these greetings to keep them engaging and fresh. However, I will not include these types of greetings in responses to follow-up questions or within ongoing conversations.
You may also provide me with a chat history in array format, ordered from earliest to most recent, containing both your input and my prior responses as well as a timestamp showing when the conversation occurred. I will use this history to maintain conversational continuity and context, referencing past exchanges where relevant.
For now, my responses will be in text only, but I'm ready to adapt to voice interactions when needed. My focus is on being a friendly, useful, and approachable AI companion for you.
Today's date is {formatted_date_time}. Depending on what your question is, i may receive the weather data for next few days, some data has been redacted, so if you ask about weather for an hour I do not have data for, I will make
an estimate based on the temperatures before and after it. The weather data object contains each day, and the hours for each day. Each hour contains the Temperature in Fahrenheit and the condition description. I may also be given the
current temperature and weather condition. If I did not receive weather data, I will not include it in my response. I will not make up any data unless asked to. Do not say that you are an AI, just respond as if you are a human. If you can't answer something, just say you will get back to it later."""
    
    user_prompt = f"This is what the user said: [{command}]. This is the chat history: [{chatHistory}]. {weatherMessage}"


    try:
        current = ""
        # Make a streaming request to the GPT-4 API
        response = client.chat.completions.create(model="gpt-4", 
        messages=[
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt},
        ],
        temperature=0.7,
        top_p=1,
        frequency_penalty=0,
        presence_penalty=.5,
        max_tokens=300,
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
                    if any(cmd in command.lower() for cmd in closeCommands):
                        enterSilentMode()
                        continue
                    stream_gpt4_response(command)
                if not inSpeechMode and any(cmd in command.lower() for cmd in openCommands):
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
                print(f"{prepend} Could not request results")
            #time.sleep(LISTENER_INTERVAL) # maybe remove this


start_voice_daemon()
# stream_gpt4_response("Write me a 2 paragraph story?")