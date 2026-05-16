from socket import *

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# Create a UDP socket
# Notice the use of SOCK_DGRAM for UDP packets
serverSocket = socket(AF_INET, SOCK_DGRAM)

# Assign IP address and port number to socket
serverSocket.bind(('', 9419))

serverSocket.settimeout(1)

isConnected = False

print("Waiting for connection...")
while True:

    # Receive the client packet along with the address it is coming from
    while not isConnected:
        try:
            message, address = serverSocket.recvfrom(1024)
            isConnected = True
            print(message.decode('utf-8'))
        except TimeoutError:
            isConnected = False

    #c =  " ".join(sys.argv[1:]) + "\n"
    c = input(bcolors.HEADER +"MacetoHuerto> ") + "\n"
    status = serverSocket.sendto(bytes(c, 'utf-8'), address)

    data = ""
    while (not "??END??" in data) and (isConnected):
        try:
            message, address = serverSocket.recvfrom(1024)
            data += message.decode('utf-8')
        except TimeoutError:
            isConnected = False

    if isConnected:
        print(bcolors.OKGREEN +  data.replace("??END??",""))
    else:
        print(bcolors.FAIL + "Error, MacetoHuerto disconnected.")
        print(bcolors.OKGREEN + "Waiting for connection...")

