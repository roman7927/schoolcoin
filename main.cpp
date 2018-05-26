// began with JS https://www.youtube.com/watch?v=zVqczFZr124
// continued with JS https://lhartikk.github.io/jekyll/update/2017/07/14/chapter1.html
// continue with section "Storing the blockchain"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <strings.h>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include "sha256.h"
#include "client.h"

// Networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class Transaction {
private:
    string fromAddress;
    string toAddress;
    int amount;
public:
    Transaction(string fromAddress, string toAddress, int amount);
    Transaction(const Transaction &t);
    string getFromAddress();
    string getToAddress();
    int getAmount();
    void displayTransaction();
};

Transaction::Transaction(string fromAddress, string toAddress, int amount) {
    this->fromAddress = fromAddress;
    this->toAddress = toAddress;
    this->amount = amount;
}

Transaction::Transaction(const Transaction &t) {
    this->fromAddress = t.fromAddress;
    this->toAddress = t.toAddress;
    this->amount = t.amount;
}

string Transaction::getFromAddress() {
    return this->fromAddress;
}

string Transaction::getToAddress() {
    return this->toAddress;
}

int Transaction::getAmount() {
    return this->amount;
}

void Transaction::displayTransaction() {
    cout << "  From " << this->fromAddress << " to " << this->toAddress << " $" << this->amount << endl;
}

class Block {
private:
    int index;
    int timestamp;
    vector<Transaction> transactions;
    string previousHash;
    string hash;
    int nonce = 0;
public:
    Block(int index, int timestamp, vector<Transaction> transactions, string previousHash = "null");
    int getIndex();
    string getHash();
    string getPreviousHash();
    string calculateHash();
    vector<Transaction> getTransactions();
    void displayBlock();
    void mineBlock(int difficulty);
};

Block::Block(int index, int timestamp, vector<Transaction> transactions, string previousHash) {
    this->index = index;
    this->timestamp = timestamp;
    this->transactions = transactions;
    this->previousHash = previousHash;
    this->hash = this->calculateHash();
}

int Block::getIndex() {
    return this->index;
}

string Block::getHash() {
    return this->hash;
}

string Block::getPreviousHash() {
    return this->previousHash;
}

string Block::calculateHash() {
    return sha256(to_string(this->index) + this->previousHash + to_string(this->timestamp) + to_string(this->nonce));
}

vector<Transaction> Block::getTransactions() {
    return this->transactions;
}

void Block::displayBlock() {
    int ts = this->timestamp;
    time_t ts2 = ts;
    tm *gmtm = gmtime(&ts2);
    string dt = asctime(gmtm);
    dt.erase(remove(dt.begin(), dt.end(), '\n'), dt.end());

    cout << "Block index " << this->index << ", timestamp " << dt << ", previous hash " << this->previousHash
         << ", hash " << this->hash << endl;

    vector<Transaction> t = this->transactions;

    for(int i=0; i < t.size(); i++) {
        t[i].displayTransaction();
    }
}

void Block::mineBlock(int difficulty) {
    while(this->hash.substr(0, difficulty) != string(difficulty, '0')) {
        this->nonce++;
        this->hash = this->calculateHash();
    }
}

class BlockChain {
private:
    map<int,Block> chain;
    int difficulty;
    vector<Transaction> pendingTransactions;
    int miningReward;
public:
    BlockChain(int difficulty, int miningReward);
    Block createGenesisBlock();
    Block getLatestBlock();
    void displayBlockChain();
    bool isChainValid();
    void minePendingTransactions(string miningRewardAddress);
    void createTransaction(Transaction t);
    int getBalanceOfAddress(string address);
    int getHeight();
};

BlockChain::BlockChain(int difficulty, int miningReward) {
    this->difficulty = difficulty;
    this->miningReward = miningReward;
    this->chain.insert(pair<int,Block>(0,this->createGenesisBlock()));
}

Block BlockChain::createGenesisBlock() {
    int now = time(0);

    vector<Transaction> genesisTransaction;
    genesisTransaction.push_back(Transaction("ALIENS","satoshi",50));

    return Block(0,now,genesisTransaction);
}

Block BlockChain::getLatestBlock() {
    map<int,Block>::iterator it=chain.end();
    it--;
    return it->second;
}

void BlockChain::displayBlockChain() {
    for(map<int,Block>::iterator it=chain.begin(); it!=chain.end(); ++it) {
        it->second.displayBlock();
    }
}

bool BlockChain::isChainValid() {
    for(map<int,Block>::iterator it=chain.begin(); it!=chain.end(); ++it) {
        Block currentBlock = it->second;
        map<int,Block>::iterator it2=it--;
        Block previousBlock = it2->second;
        if(currentBlock.getHash() != currentBlock.calculateHash()){
            return 0;
        }
        if(currentBlock.getPreviousHash() != previousBlock.getHash()){
            return 0;
        }
    }
    return 1;
}

void BlockChain::minePendingTransactions(string miningRewardAddress) {

    int now = time(0);

    if(this->pendingTransactions.size() == 0) {
        cout << "There are no pending transactions, so I'm not going to mine" << endl;
    }

    int nextIndex = this->getLatestBlock().getIndex() + 1;

    Block x(nextIndex,now,this->pendingTransactions,this->getLatestBlock().getHash());
    x.mineBlock(this->difficulty);

    this->chain.insert(pair<int,Block>(nextIndex,x));
    this->pendingTransactions.clear();

    Transaction y("ALIENS", miningRewardAddress, this->miningReward);
    this->pendingTransactions.push_back(y);
}

void BlockChain::createTransaction(Transaction t) {
    if(this->getBalanceOfAddress(t.getFromAddress()) < t.getAmount()) {
        cout << "Not enough coins! Trying to send " << t.getAmount() << " but " << t.getFromAddress() << " has only "
             << this->getBalanceOfAddress(t.getFromAddress()) << endl;
    } else {
        cout << "Transaction approved! Sent " << t.getAmount() << " from " << t.getFromAddress() << " to "
             << t.getToAddress() << endl;
        this->pendingTransactions.push_back(t);
    }
}

int BlockChain::getBalanceOfAddress(string address) {
    int balance = 0;
    for(map<int,Block>::iterator it=chain.begin(); it!=chain.end(); ++it) {
        Block b = it->second;
        vector<Transaction> t = b.getTransactions();

        for(int j=0; j < t.size(); j++) {

            if (t[j].getFromAddress() == address) {
                balance -= t[j].getAmount();
            }

            if (t[j].getToAddress() == address) {
                balance += t[j].getAmount();
            }
        }
    }
    return balance;
}

int BlockChain::getHeight() {
    return this->getLatestBlock().getIndex()+1;
}

int main() {

    int sSocket;
    int sSocketListen;
    int sSocketAccept;
    int sRemoteSocket;
    unsigned int len = sizeof(sockaddr_in);
    struct sockaddr_in sSocketAddr;
    struct sockaddr_in sRemoteSocketAddr;
    char sMessage[] = "Welcome!";
    int sSent, sRcvd = 0;
    struct in_addr addr;

    if ((sSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cout << "Error creating a socket..." << endl;
        return -1;
    }

    sSocketAddr.sin_family = AF_INET;
    sSocketAddr.sin_port = 84;
    sSocketAddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&sSocketAddr.sin_zero,8);

    bind(sSocket, (struct sockaddr*) &sSocketAddr, len);

    if ((sSocketListen = listen(sSocket, 5)) == -1) {
        cout << "Error listening on server port " << sSocketAddr.sin_port << endl;
        return -1;
    }

    while (1) {

        cout << "test1" << endl;

        sRemoteSocket = accept(sSocket, (struct sockaddr*) &sRemoteSocketAddr, &len);

        cout << "test2" << endl;

        if (sRemoteSocket == -1) {
            cout << "Error accepting on server port " << sSocketAddr.sin_port << endl;
            return -1;
        }

        sSent = send(sRemoteSocket, sMessage, strlen(sMessage), 0);

        cout << "Sent " << sSent << " bytes to client " << inet_ntoa(sRemoteSocketAddr.sin_addr) << endl;

        //close(sRemoteSocket);
    }



    return 0;

    test();

    BlockChain ourCoin(4,1);

    cout << "Balance of satoshi is: " << ourCoin.getBalanceOfAddress("satoshi") << endl;

    ourCoin.createTransaction(Transaction("satoshi","roman",100));
    ourCoin.createTransaction(Transaction("satoshi","roman",10));

    ourCoin.minePendingTransactions("roman");

    ourCoin.createTransaction(Transaction("roman","dmitry",5));

    ourCoin.minePendingTransactions("roman");
    ourCoin.minePendingTransactions("roman");

    cout << endl;

    ourCoin.displayBlockChain();

    cout << endl;

    cout << "Balance of satoshi is: " << ourCoin.getBalanceOfAddress("satoshi") << endl;
    cout << "Balance of roman is: " << ourCoin.getBalanceOfAddress("roman") << endl;
    cout << "Balance of dmitry is: " << ourCoin.getBalanceOfAddress("dmitry") << endl;

    cout << "Height of blockchain is: " << ourCoin.getHeight() << endl;

    return 0;
}