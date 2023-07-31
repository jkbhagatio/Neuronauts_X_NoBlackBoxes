import zmq
import time

def zmq_client(server_ip, port):
    context = zmq.Context()
    socket = context.socket(zmq.PUB)  # REQ socket type for request-reply pattern
    socket.connect(f"tcp://{server_ip}:{port}")

    message = "Hello, ZeroMQ server!"
    print(f"Sending message: {message}")
    socket.send_string(message)

    # Receive the server's reply
    #reply = socket.recv_string()
    #print(f"Received reply: {reply}")

if __name__ == "__main__":
    server_ip = "192.168.48.75.1"  # Replace with the server's IP address if running
    # remotely
    port = 12345
    zmq_client(server_ip, port)