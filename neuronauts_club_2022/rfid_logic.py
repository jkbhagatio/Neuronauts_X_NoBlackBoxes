# rfid_tag_names:
# "Luis" = "021000005D4F"    # rfid tag serial for Luis
# "Darcey" = "021000001C0E"  # rfid tag serial for Darcey
# "Karin" = "000C84599A4B"   # rfid tag serial for Karin

from pathlib import Path
import numpy as np
import zmq
import whisper
import pyttsx3
import ipdb

remote_ip = "192.168.1.112"
local_ip = "192.168.1.113"
local_port = "5557"
remote_port1 = "5557"
remote_port2 = "5556"

person_room_map = {
    "Lewis": "",
    "Darcey": "",
    "Karin": "",
}

room_person_map = {
    "gym": [],
    "lab": [],
    "med bay": [],
}

context = zmq.Context()
remote_socket1 = context.socket(zmq.SUB)
remote_socket1.connect(f"tcp://{remote_ip}:{remote_port1}")
remote_socket1.setsockopt(zmq.SUBSCRIBE, b"")
remote_socket2 = context.socket(zmq.SUB)
remote_socket2.connect(f"tcp://{remote_ip}:{remote_port2}")
remote_socket2.setsockopt(zmq.SUBSCRIBE, b"")
local_socket = context.socket(zmq.SUB)
local_socket.connect(f"tcp://{local_ip}:{local_port}")
local_socket.setsockopt(zmq.SUBSCRIBE, b"")

poller = zmq.Poller()
poller.register(remote_socket1, zmq.POLLIN)
poller.register(remote_socket2, zmq.POLLIN)
poller.register(local_socket, zmq.POLLIN)

whisper_model = whisper.load_model("base")
# tts = TTS('tts_models/en/blizzard2013/capacitron-t2-c50')
# audio = pyaudio.PyAudio()
pyttsx3_engine = pyttsx3.init()
pyttsx3_engine.setProperty('volume', 0.7)
pyttsx3_engine.setProperty('rate', 170)

name = ""
message = ""
print(f"Set-up complete.")
while True:
    sockets = dict(poller.poll())
    # if we have a message and it's pollin
    if remote_socket1 in sockets and sockets[remote_socket1] == zmq.POLLIN:
        message = remote_socket1.recv_string()
    if remote_socket2 in sockets and sockets[remote_socket2] == zmq.POLLIN:
        message = remote_socket2.recv_string()
    if local_socket in sockets and sockets[local_socket] == zmq.POLLIN:
        message = local_socket.recv_string()
    print(f"Received message: {message}")
    uid, cur_room = message.split("_")
    if "021000005D4F" in uid:
        name = "Lewis"
    elif "021000001C0E" in uid:
        name = "Darcey"
    elif "000C84599A4B" in uid:
        name = "Karin"
    for room in room_person_map.keys():
        if name in room_person_map[room]:
            room_person_map[room].remove(name)
    room_person_map[cur_room].insert(0, name)
    room_person_map[cur_room] = np.unique(room_person_map[cur_room]).tolist()
    print(f"{room_person_map}")
    # Search for wav file
    audio_files = list(Path.cwd().glob("*.wav"))
    # speech to text conversion
    if len(audio_files):
        audio = whisper_model.transcribe(str(audio_files[0]))
        txt = audio["text"]
        # If name in text, output: "{name} is in {room}"
        if any(str in txt for str in list(person_room_map.keys())):
            out = f"{name} is in the {cur_room}"
        # If room in text, output: "{names} are in {room}"
        elif any(str in txt for str in list(room_person_map.keys())):
            out = f"The {cur_room} contains {room_person_map[cur_room]}"
        else:
            out = "I didn't understand that"
        # tts.tts_to_file(text=out, file_path="out.wav")
        pyttsx3_engine.say(out)
        pyttsx3_engine.runAndWait()
        audio_files[0].unlink()
