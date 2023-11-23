#include<string>
#include<vector>
#include <random>
#include <limits>
#include <map>
#include <iostream>
#include <stdio.h>
#include <cstring>

#define BUFFERSIZE 8192
#define PORT 8004
typedef long long ll;


void abort(std::string s){
    std::cout << "Authentication Failed: " << s << std::endl;
    exit(0);
}

ll hash_function(std::vector<ll> key){
    std::hash<uint32_t> h;
    std::size_t ret = vec.size();
    for(auto& i : vec) {
        ret ^= h(i) | i;
    }
    return ret;
    // return 0;
}

ll getrandom(){
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<long long> dis(0, 10000000000000000);
    return dis(gen);
}

struct client{
    ll K;    // id for hn
    ll id;   // SUPI
    ll n;    // seq num
    ll c;    // concealing seq num
    ll an;   // temporary
    ll bn;   // variables
    ll kn,km;
    client(ll K,ll id,ll n,ll km,ll kn)
        :K(K),id(id),n(n),km(km),kn(kn)
    {
        an = id ^ hash_function({km,kn});
        bn = an ^ km ^ kn;
        c = hash_function({km,id});
    }
    std::string syncmessage(){
        std::string message = "*";
        message += std::to_string(an);
        message += "*";
        message += std::to_string(bn);
        message += "*";
        message += std::to_string(hash_function({K,id,c,an,bn,n}));
        message += "*#";
        std::cout << "Sent by Client to Server: an: " << an << " bn: " << bn << " hn: " << hash_function({K,id,c,an,bn,n}) << std::endl; 
        return message;
    }
    std::string desyncmessage(){
        std::string message = "*";
        message += std::to_string(an);
        message += "*";
        message += std::to_string(bn);
        message += "*";
        ll rn = getrandom();
        ll yn = an ^ id ^ rn;
        ll zn = n ^ hash_function({K,rn,yn});
        message += std::to_string(yn);
        message += "*";
        message += std::to_string(zn);
        message += "*";
        message += std::to_string(hash_function({K,id,c,an,bn,n,zn}));
        message += "*#";
        // std::cout << "Sent by Client to Server: an: " << an << " bn: " << bn << " yn: " << yn << " zn: " << (n ^ hash_function({K,rn,yn})) << " hn: " << hash_function({K,id,c,an,bn,n}) << std::endl; 
        return message;
    }
    std::vector<std::string> deserialize(const char* message){
        ll ptr = 0;
        // printf("%s\n",message); 
        while(message[ptr]=='*')ptr++;
        std::vector<std::string> tokens;
        while(message[ptr]!='#'){
            std::string temp = "";
            while(message[ptr]!='*'){
                temp += message[ptr];
                ptr++;
            }
            tokens.push_back(temp);
            ptr++;
        }
        return tokens;
    }
};

struct server{
    std::map<ll,std::pair<ll,ll>> registeredClients;
    ll km;
    ll n;
    ll deln;
    
    server(ll km) :km(km){
        deln = 100;

    }

    void addClient(ll id,ll K,ll n){
        registeredClients[id] = {K,n};
    }

    std::string serialize(std::vector<ll> tokens){
        std::string message = "";
        for(auto token:tokens){
            message += std::to_string(token);
            message += "*";
        }
        message += "#";
        std::cout << "Reply from server to client: alpha: " << tokens[0] << " beta: " << tokens[1] << " eeta: " << tokens[2] << " muu: " << tokens[3] << "\n";
        return message;
    }

    std::vector<std::string> deserialize(const char* message){
        ll ptr = 0;
        // printf("%s\n",message); 
        while(message[ptr]=='*')ptr++;
        std::vector<std::string> tokens;
        while(message[ptr]!='#'){
            std::string temp = "";
            while(message[ptr]!='*'){
                temp += message[ptr];
                ptr++;
            }
            tokens.push_back(temp);
            ptr++;
        }
        return tokens;
    }




};