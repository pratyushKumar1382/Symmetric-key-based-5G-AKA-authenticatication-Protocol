// server.cpp

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include "utils.h"



// Function to send a message to the client
void sendMessage(int clientSocket, const char* message) {
    send(clientSocket, message, strlen(message), 0);
}

// Function to receive a message from the client
int receiveMessage(int clientSocket, char* buffer, int bufferSize) {
    return recv(clientSocket, buffer, bufferSize, 0);
}

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Bind the socket to an address and port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening for connections" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << PORT <<"..." << std::endl;

    // Accept a connection
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressLength);

    if (clientSocket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Connection accepted from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    // Communication loop
    char buffer[BUFFERSIZE];
    char response[BUFFERSIZE];
    // auth code
    server host(567890);
    // add random clients for simulaion purpose
    for(ll i=0;i<100000;i++){
        ll x = getrandom(),y = getrandom(),z = getrandom();
        host.addClient(x,y,z);
    }


    host.addClient(234562345,3452345,1005);


    int bytesRead = receiveMessage(clientSocket, buffer, sizeof(buffer));
    // std::cout << "Received from client: " << buffer << std::endl;
    std::vector<std::string> tokens = host.deserialize(buffer);
    // for(auto temp:tokens) std::cout << temp << " ";
    // std::cout << std::endl;
    ll kn = stoll(tokens[1]) ^ stoll(tokens[2]) ^ host.km;
    ll id = stoll(tokens[1]) ^ hash_function({host.km,kn});
    ll c = hash_function({host.km,id});
    if(!host.registeredClients.count(id)){
        // abort
        abort("Malacious UE");
    }
    auto temp = host.registeredClients[id];
    ll K = temp.first,n_ = temp.second;
    if(tokens[0]=="Desync"){
        // an,bn,yn,zn,hn
        ll rn = stoll(tokens[3]) ^ stoll(tokens[1]) ^ id;
        ll n = stoll(tokens[4]) ^ hash_function({K,rn,stoll(tokens[3])});
        bool flag = false;
        while(n){
            if(hash_function({K,id,c,stoll(tokens[1]),stoll(tokens[2]),n,stoll(tokens[4])}) == stoll(tokens[5])){
                n_ = n;
                flag = true;
                break;
            }
            n--;
        }
        if(!flag){
            // abort
            abort("Malacious UE.");
        }

    }
    else if(tokens[0]=="Sync"){
        bool flag = false;
        for(ll n = n_+host.deln;n >= n_ ;n--){
            if(hash_function({K,id,c,stoll(tokens[1]),stoll(tokens[2]),n})==stoll(tokens[3])){
                n_ = n;
                flag = true;
                break;
            }
        }
        if(!flag){
            // abort
            abort("Malacious UE.");
        }
    }
    else{

        abort("Illegal request by client");

    }

    n_ = n_ + 1;
    ll kn_= getrandom(),fn_ = getrandom();
    ll an_ = id ^ hash_function({host.km,kn_});
    ll bn_ = an_ ^ host.km ^ kn_;
    ll eeta = hash_function({fn_,c}) ^ an_;
    ll muu = hash_function({c,fn_}) ^ bn_;
    ll alpha = c ^ fn_;
    ll seskey = hash_function({K,fn_,eeta,muu,n_+1});
    ll beta = hash_function({seskey,an_,bn_,id,c});
    strcpy(response,host.serialize({alpha,beta,eeta,muu}).c_str());
    // printf("%s\n",response);
    // std::cout << "seskey: " << seskey << " " << an_ << " " << bn_ << " " << id << " " << c << "\n";
    


    sendMessage(clientSocket,response);  


 
    // auth completed

    std::cout << "Authentication Successful" << std::endl;
    while (true) {
        // Receive data from the client
        int bytesRead = receiveMessage(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            std::cerr << "Connection closed by client" << std::endl;
            break;
        }

        // Display received message
        buffer[bytesRead] = '\0';  // Null-terminate the received data
        std::cout << "Received from client: " << buffer << std::endl;
        // std::cout << "Length of message recieved: " << bytesRead << std::endl;

        // Send a response back to the client
        std::cout << "Enter response (or 'exit' to quit): ";
        std::cin.getline(response, sizeof(response));

        // const char* response = "Message received";
        sendMessage(clientSocket, response);
        n_++;
    }

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
