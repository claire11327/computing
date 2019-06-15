#include <typeinfo>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>  
#include <cstdio>
#include <ctime>
#include <cmath>
#include <map>

using namespace std;

class block{
    public:
        bool valid;
        bool refer;
        int access;//turn
    block(){
        this->valid = false;
        this->refer = false;
        this->access = 0;
    }
}; 
class set{
    public:
        map<unsigned int,block> S ; //unsigned int > tag in this set
        int blockNum = 0;
    
};

int main(int argc, char *argv[]){

    //unfinish file read and write
    fstream fileIn;
    fstream fileOut;

    fileIn.open(argv[1],ios::in);
    fileOut.open(argv[2],ios::out);

    //instruction read
    int cacheSize;
    int blockSize;
    int associativity;
    int replacePolicy;

    int blockNumber;
    int indexNumber;
    int tagNumber;
    int offset;
    int N;
    int out;
    bool LRU;
    
    unsigned int temp; // command



    map<unsigned int , set> cache; //long > index
    map<unsigned int , set>::iterator iter;
    map<unsigned int, block>::iterator iter_2;

    fileIn >> cacheSize;
    fileIn >> blockSize;
    fileIn >> associativity; // (associativity,N) = (0,1)/(1,4)/(2,block num)
    fileIn >> replacePolicy;
    
    blockNumber = (cacheSize*1024 / blockSize);
    
    switch(associativity){
        case 0:
            N = 1;
            break;
        case 1:
            N = 4;
            break;
        case 2:
            N = blockNumber;
            srand(time(NULL));
            break;
        default:
            N = 0;
            break;
    }
    indexNumber = log2(blockNumber / N);
    offset = log2(blockSize);
    tagNumber   = 32 - offset - indexNumber;

    //set index
    for(int i = 0; i < pow(2,indexNumber); i++){
        set s;
        s.blockNum = 0;
        cache[i] = s;
    }
    int k = 0;

    while(fileIn >> hex >> temp){

        
        int temp2 = pow(2,32 - tagNumber);
        unsigned int  index = (temp % temp2)/pow(2,offset);
        unsigned int  tag = temp / temp2;

        iter = cache.find(index);
        if(iter == cache.end()){
            fileOut << "out of index" << endl;
        }
        iter_2 = cache[index].S.find(tag);

        if(k == 10 && replacePolicy == 2){
            for(iter_2 = cache[index].S.begin();iter_2 != cache[index].S.end(); iter_2 ++){
                iter_2->second.refer = false;
            }
        }



        if(iter_2 != cache[index].S.end()){
            //find tag in set
            cache[index].S[tag].refer = true;
            out = -1;
        }
        else{
            //didn't find tag in set
            //do replacement
            iter_2 = cache[index].S.begin();
            unsigned int  erasetag = iter_2->first;
            unsigned int  eraseacc = iter_2->second.access;
            if(cache[index].blockNum == N){
                //no space in set, need to replace
                if(replacePolicy == 0){
                    //FIFO
                    for(iter_2 = cache[index].S.begin(); iter_2 != cache[index].S.end(); iter_2++){
                        if(iter_2->second.access == 0){
                            erasetag = iter_2->first;
                            eraseacc = iter_2->second.access;
                            break;
                        }
                    }
                }
                else if(replacePolicy == 1 ){
                    //LRU
                    bool LRU = false;
                    for(iter_2 = cache[index].S.begin();iter_2 != cache[index].S.end(); iter_2 ++){
                        if(iter_2->second.refer == false){
                            erasetag = iter_2->first;
                            eraseacc = iter_2->second.access;
                            LRU = true;
                            break;
                        }
                    }
                    if(!LRU){
                        for(iter_2 = cache[index].S.begin();iter_2 != cache[index].S.end(); iter_2 ++){
                            iter_2->second.refer = false;
                        }
                    }
                }
                else if(replacePolicy == 2){
                    //random
                    int i = 0;
                    int j = rand() % N;
                    for(iter_2 = cache[index].S.begin();iter_2 != cache[index].S.end(); iter_2 ++){
                        if(i == j){
                            erasetag = iter_2->first;
                            eraseacc = iter_2->second.access;
                            break;
                        }
                        i ++ ;
                    }
                }

                cache[index].S.erase(erasetag);
                cache[index].blockNum --;
            }
            else{
                //still has space in set
                erasetag = -1;
                eraseacc = pow(2, N);
            }

            // set new block for new set 
            block b;
            b.valid =  true;
            b.access = cache[index].blockNum + 1;
            cache[index].S[tag] = b;
            cache[index].blockNum ++;
            //set new access
            for(iter_2 = cache[index].S.begin();iter_2 != cache[index].S.end(); iter_2 ++){
                if(iter_2->second.access > eraseacc){
                    iter_2->second.access = iter_2->second.access - 1;
                }
            }
            out = erasetag;
            
        }
        
        fileOut << out << endl;
    }
}
