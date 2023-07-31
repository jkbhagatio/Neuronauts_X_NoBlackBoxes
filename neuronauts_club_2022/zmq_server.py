import zmq

def zmq_server(ip, port):
    context = zmq.Context()
    socket = context.socket(zmq.SUB)  # REP socket type for request-reply pattern
    socket.connect(f"tcp://{ip}:{port}")
    socket.setsockopt(zmq.SUBSCRIBE, b"")

    while True:
        message = socket.recv_string()  # socket.recv()
        print(f"Received message: {message}")

        # Send a reply to the client
        # socket.send_string("Acknowledged")

if __name__ == "__main__":
    ip = "172.24.66.67"
    port = "5557"
    print(f"Starting ZeroMQ subscriber to {ip}:{port}")
    zmq_server(ip, port)
