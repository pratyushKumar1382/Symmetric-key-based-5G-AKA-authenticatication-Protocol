// client.cpp

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>   
#include "utils.h"

// Function to send a message to the server
void sendMessage(int clientSocket, const char* message) {
    send(clientSocket, message, strlen(message), 0);
}

// Function to receive a message from the server
int receiveMessage(int clientSocket, char* buffer, int bufferSize) {
    return recv(clientSocket, buffer, bufferSize, 0);
}


int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP address
    serverAddress.sin_port = htons(PORT);

    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return -1;
    }

    std::cout << "Connected to server" << std::endl;

    // Communication loop
    char buffer[BUFFERSIZE];
    char response[BUFFERSIZE];
    std::string reply;


    // auth code
    client mobile(3452345,234562345,1010,567890,234546);

    

    std::cout << "Desync/Sync Mode? ";
    // auto start = std::chrono::high_resolution_clock::now();
    std::string mode;
    std::getline(std::cin, mode);
    reply = mode;
    if(mode == "Desync"){
        reply += mobile.desyncmessage();
        strcpy(response,reply.c_str());
        sendMessage(clientSocket, response);


    }
    else if(mode == "Sync") {
        
        reply += mobile.syncmessage();
        strcpy(response,reply.c_str());
        sendMessage(clientSocket, response);
    } 
    else{
        abort("Illegal registration type.");
    }
    mobile.n++;
    int bytesRead = receiveMessage(clientSocket, buffer, sizeof(buffer));
    if(bytesRead == 0){
        abort("Auth Failure.");
    }
    // std::cout << "Received from server: " << buffer << std::endl;
    std::vector<std::string> tokens = mobile.deserialize(buffer);
    // for(auto s:tokens){
    //     std::cout << s << " ";
    // }
    // std::cout << std::endl;
    ll fn_ = mobile.c ^ stoll(tokens[0]);
    ll an_ = stoll(tokens[2]) ^ hash_function({fn_,mobile.c});
    ll bn_ = stoll(tokens[3]) ^ hash_function({mobile.c,fn_});
    ll seskey = hash_function({mobile.K,fn_,stoll(tokens[2]),stoll(tokens[3]),(mobile.n)+1});
    ll beta_ = hash_function({seskey,an_,bn_,mobile.id,mobile.c});
    // std::cout << "seskey: " << seskey << " " << an_ << " " << bn_ << " " << mobile.id << " " << mobile.c << "\n";
    if(beta_ != stoll(tokens[1])){
        abort("Illegal SN.");
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "Authentication Successful\n";
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
 
    // std::cout << "Time taken to authenticate: "
    //      << duration.count() << " microseconds" << std::endl; 


    






    // auth completed
    while (true) {
        // Get user input
        std::cout << "Enter message (or 'exit' to quit): ";
        std::cin.getline(response, sizeof(response));

        // Send message to the server
        sendMessage(clientSocket, response);
        mobile.n++;

        if (strcmp(response, "exit") == 0) {
            break;
        }

        // Receive response from the server
        int bytesRead = receiveMessage(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            std::cerr << "Connection closed by server" << std::endl;
            break;
        }

        // Display server's response
        buffer[bytesRead] = '\0';  // Null-terminate the received data
        std::cout << "Server response: " << buffer << std::endl;
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
