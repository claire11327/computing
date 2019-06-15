#include <typeinfo>
#include <iostream>
#include <fstream>
#include <cstring>  
#include <cstdio>
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
        map<long,block> S ; //long > tag in this set
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
    int byteoffset;
    int wordoffset;
    long temp;
    long out;
    bool LRU;


    int lastv = 0;

    map<int, set> cache; //long > index
    map<int, set>::iterator iter;
    map<long, block>::iterator iter_2;

    fileIn >> cacheSize;
    fileIn >> blockSize;
    fileIn >> associativity; // (associativity,N) = (0,1)/(1,4)/(2,block num)
    fileIn >> replacePolicy;

    switch(associativity){
        case 0:
            byteoffset = 0;
            break;
        case 1:
            byteoffset = 2;
            break;
        case 2:
            byteoffset = log2(blockNumber);
            break;
        default:
            byteoffset = 0;
            break;
    }
        
    blockNumber = log2(cacheSize*1024 / blockSize);
    indexNumber = (blockNumber / pow(2,byteoffset));
    wordoffset  =  log2(blockSize);
    tagNumber   = 32 - byteoffset - wordoffset - indexNumber;
    cout << "offset = " << byteoffset << endl;
    cout << "blockNumber = " << blockNumber << endl;
    cout << "indexNumber = " << indexNumber << endl;
    cout << "tagNumber = " << tagNumber << endl; // (tag is a tagNumber bits num)
    // block num = cacheSize / blockSize (toatal blockNumber blocks)
    // set index num = cacheSize / (blockSize * N); (index is a indexNumber bits num)

    //set index
    for(int i = 0; i < pow(2,indexNumber); i++){
        set s;
        s.blockNum = 0;
        cache[i] = s;
    }

    


    cout << "--------------------------\n" ; 


    while(fileIn >> hex >> temp){
        lastv = out;
        long a = temp;

        long tag; 
        int temp2 = pow(2,32 - tagNumber);
        int index = (temp % temp2)/pow(2,byteoffset+wordoffset);
        tag = temp / temp2;

        iter = cache.find(index);
        iter_2 = cache[index].S.find(tag);


        if(iter_2 != cache[index].S.end()){
            //find tag in set
            cache[index].S[tag].refer = true;
            out = -1;
        }
        else{
            if(cache[index].blockNum == pow(2,byteoffset)){
                //no space in set, need to replace
                if(replacePolicy == 0){
                    //FIFO
                    for(iter_2 = cache[index].S.begin(); iter_2 != cache[index].S.end(); iter_2++){
                        if(iter_2->second.access == 0){
                            out = iter_2->first;
                            cache[index].S.erase(out);
                            block b;
                            b.valid = true;
                            b.access = cache[index].blockNum - 1;
                            cache[index].S[tag] = b;
                            break;
                        }
                        else{
                            iter_2->second.access = iter_2->second.access - 1;
                        }
                    }
                }
                else if(replacePolicy == 1 || replacePolicy == 2){
                    //LRU
                    int accesstemp = 0;
                    LRU = false;
                    for(iter_2 = cache[index].S.begin(); iter_2 != cache[index].S.end(); iter_2++){
                        cout << iter_2->first << endl;
                        if(iter_2->second.refer == 0){
                            LRU = true;             
                            accesstemp = iter_2->second.access;
                            break;
                        }
                    }
                    if(!LRU){
                        iter_2 = cache[index].S.begin();             
                    }   
                    
                    out = iter_2->first;
                    fileOut << "----" << endl;  


             
                    cache[index].S.erase(out);

                    block b;
                    b.valid = true;
                    b.access = cache[index].blockNum;
                    cache[index].S[tag] = b;
                    for(iter_2 = cache[index].S.begin(); iter_2 != cache[index].S.end(); iter_2++){
                        int i = 1;
                        if(iter_2->second.access > accesstemp){
                            iter_2->second.access = iter_2->second.access - 1;                            
                        }                        
                        iter_2->second.refer = false;
                        i ++;
                        if(i > 4){
                            fileOut << "t i = " << i <<endl;
                        }
                    }
                }
            }
            else{
                //still has space in set
                block b;
                b.valid = true;
                b.access = cache[index].blockNum;
                cache[index].S[tag] = b;
                cache[index].blockNum ++ ;                   
                out = -1;
            }
        }

        //find set index 
        //cache[index].S.find(tag)
        //  [if cache[index].S.find(tag) == cache[index].find.end()]
        //      [if cache[index].blockNum == blockNum - 1]
        //          [full > need to replace]
        //      [else ]
        //          [cache[index].S[tag] = temp 's tag]
        //          [cache[index].blockNum ++]
        //  [else ]
        //      [if cache[index].S[tag].refer] set refer to true

            //cout << "victim = " << out  << "\n" << endl;
            if(out == lastv && out != -1){
                cout << "same = " << out   << endl;
                cout << tag << endl;
                cout << index << endl;
                cout << "\n";   
            }
            fileOut << "out = " << out << endl;
            cout << endl;
    }
}
