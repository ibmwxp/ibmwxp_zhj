import io
import os

# Imports the Google Cloud client library
from google.cloud import speech
from google.cloud.speech import enums
from google.cloud.speech import types

# Instantiates a client
client = speech.SpeechClient()
myFileHandle = open("C:\\Users\\Jack Zhang\\Desktop\\audio.txt", 'w+')

# The name of the audio file to transcribe
file_name = os.path.join(
    os.path.dirname(__file__),
    'audio.wav')

# Loads the audio into memory
with io.open(file_name, 'rb') as audio_file:
    content = audio_file.read()
    audio = types.RecognitionAudio(content=content)

config = types.RecognitionConfig(
    encoding=enums.RecognitionConfig.AudioEncoding.LINEAR16,
    sample_rate_hertz=44100,
    language_code='en-US')

# Detects speech in the audio file
response = client.recognize(config, audio)

for result in response.results:
    print(format(result.alternatives[0].transcript), file= myFileHandle)
    myFileHandle.close



#system("python C:\\Users\\Jack Zhang\\Desktop\\test.py")
#system("python C:\\\\Users\\\\Jack\\ Zhang\\\\Desktop\\\\test.py")