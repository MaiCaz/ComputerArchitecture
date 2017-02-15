//
//  main.cpp
//  CS472 Project 2
//
//  Created by Maïté CAZENAVE on 23/03/15.
//  Copyright (c) 2015 Maïté CAZENAVE. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    
    //INITIALIZATION
    
    short Main_mem[2048];;
    for(int i=0;i<2048;i++){
            Main_mem[i]=i%0x100;    //Initialisation of the MM by filling it with 0x0 to 0xFF (best value in a byte)
    }
    
   /* for (int i=0;i<2048;i++){
        cout <<hex<<uppercase<<Main_mem[i]<<endl; //Display Memory
    }
    */
    
    struct lign_cache{ // Structure with
        int Slot;
        int Tag;
        int Valid;
        int Data[16];
    };
    
    lign_cache Cache[16];
    
    for(int i=0;i<16;i++){
    
    Cache[i]={i,0,0,{0}};

    }
    
    ifstream myfile("/Users/MaiCaz/Desktop/Project2.txt"); //Open the file containing the user's commands
    
    if(myfile){ // Opening successful
        
        string line; 
        
        while(getline(myfile,line)){   // While it's not the end of the file (a line exists)
    
     char entry;    
     short address;
     int data;
     
    //cout<<"(R)ead,(W)rite or (D)isplay Cache ?"<<endl;
            myfile>>entry; //entry takes the value of the file
    
    //Read
    if(entry=='R'){
        
        //cout<<"What address would you like to read ?"<<endl;
        myfile>>hex>>address;
        
        int Block_offset=address&0x000F;        //Offset : Bits 0 to 3
        int Block_slot=(address&0x00F0)>>4;     //Slot : Bits 4 to 7
        int Block_tag=(address&0xFF00)>>8;      //Tag : Bit 8 to 15
        
    if(Cache[Block_slot].Tag==Block_tag && Cache[Block_slot].Valid==1){//Cache Hit(Valid=1 and Tag address=TagCache)
            
        cout<<"At that byte there is the value "<<hex<<uppercase<<Cache[Block_slot].Data[Block_offset]<<"(Cache Hit)."<<endl; // Display the offset at the requested address
            
        }else if(Cache[Block_slot].Valid==1){ // Cache Miss
            
            short old_address=((Cache[Block_slot].Tag<<8)+(Block_slot<<4)); 
            
            int dirty_bit=0;
            
            //Try to find if there is a dirty bit ?
            for(int i=0;i<16;i++){  
                if(Cache[Block_slot].Data[i]==Main_mem[old_address+i]){ 
                    dirty_bit=dirty_bit;
                }else{
                    dirty_bit=dirty_bit+1;
                }}
            
            if(dirty_bit!=0){ //If there is a dirty_bit then copy the Cache Block to the memory at the older address
                
                for(int i=0;i<16;i++){
                    Main_mem[old_address+i]=Cache[Block_slot].Data[i]; //Import all the block from cache to the MM(O-F)
                }
                
                
            }
            
            short address_begin=address&0xFFF0;  //Departure address of the block that we want to import from the MM
            
            for(int i=0;i<16;i++){
                Cache[Block_slot].Data[i]=Main_mem[address_begin+i]; //Import all the block to the cache (O-F)
            }
            
            
            Cache[Block_slot].Tag=Block_tag;    //Update Tag to the request Tag
            
            cout<<"At that byte there is the value "<<hex<<uppercase<<Cache[Block_slot].Data[Block_offset]<<"(Cache Miss)."<<endl;
            
            
            
            } else {//Cache Miss
                
            short address_begin=address&0xFFF0;  //Departure address of the block that we want to import from the MM
            
            for(int i=0;i<16;i++){
                Cache[Block_slot].Data[i]=Main_mem[address_begin+i]; //Import all the block to the cache (O-F)
            }
            
            Cache[Block_slot].Valid=1;          //Update Valid to 1
            Cache[Block_slot].Tag=Block_tag;    //Update Tag to the request Tag
            
            cout<<"At that byte there is the value "<<hex<<uppercase<<Cache[Block_slot].Data[Block_offset]<<"(Cache Miss)."<<endl;
        }
        cout<<"\n";
        
    } else if(entry=='W'){ //Write
        
        //cout<<"What address would you like to write?"<<endl;
        myfile>>hex>>address;
        //cout<<"What data would you like to write at that address?"<<endl;
        myfile>>hex>>data;
        
        int Block_offset=(address&0x000F);
        int Block_slot=(address&0x00F0)>>4;
        int Block_tag=(address&0xFF00)>>8;
        
        if(Cache[Block_slot].Tag==Block_tag && Cache[Block_slot].Valid==1){ //Cache Hit
            Cache[Block_slot].Data[Block_offset]=data; //Import requested data to the resquested offset
            cout<<"Value "<<hex<<data<<" has been written to address "<<hex<<address<<"(Cache Hit)."<<endl;
            
        }else if(Cache[Block_slot].Valid==1){ // Cache Miss
                
                short old_address=((Cache[Block_slot].Tag<<8)+(Block_slot<<4));
                
                int dirty_bit=0;
                
                //Try to find if there is a dirty bit ?
                for(int i=0;i<16;i++){
                    if(Cache[Block_slot].Data[i]==Main_mem[old_address+i]){
                        dirty_bit=dirty_bit;
                    }else{
                        dirty_bit=dirty_bit+1;
                    }}
                
                if(dirty_bit!=0){ //If there is a dirty_bit then copy the Cache Block to the memory at the older address
                    
                    for(int i=0;i<16;i++){
                        Main_mem[old_address+i]=Cache[Block_slot].Data[i]; //Import all the block from cache to MM(O-F)
                    }
                    
                    
                }
                
                short address_begin=address&0xFFF0;  //Departure address of the block that we want to import from MM
                
                for(int i=0;i<16;i++){
                    Cache[Block_slot].Data[i]=Main_mem[address_begin+i]; //Import all the block to the cache (O-F)
                }
                
                
                Cache[Block_slot].Tag=Block_tag;            //Update Tag to the requested Tag
                Cache[Block_slot].Data[Block_offset]=data;  //Update Data
            
                cout<<"Value "<<hex<<uppercase<<data<<" has been written to address "<<hex<<uppercase<<address<<"(Cache Miss)."<<endl;
                               
            }else{ //Cache miss
            
                short address_begin=address&0xFFF0;//Departure address (for copy)
                
            for(int i=0;i<16;i++){
                Cache[Block_slot].Data[i]=Main_mem[address_begin+i];//Copy the Block from MM to Cache
            }
                
            Cache[Block_slot].Valid=1;                  //Update Valid
            Cache[Block_slot].Tag=Block_tag;            //Update Tag
            Cache[Block_slot].Data[Block_offset]=data;  //Update Data
            cout<<"Value "<<hex<<uppercase<<data<<" has been written to address "<<hex<<uppercase<<address<<"(Cache Miss)."<<endl;
                    }
        cout<<"\n";
    
    }else if(entry=='D'){ //Display
        
            cout<<"Slot "<<"Valid "<<"Tag   "<<"Data"<<endl;
            for(int i=0;i<16;i++){
                cout<<" "<<hex<<uppercase<<Cache[i].Slot<<"     "<<hex<<uppercase<<Cache[i].Valid<<"    "<<hex<<uppercase<<Cache[i].Tag<<"    ";
                for(int j=0;j<16;j++){
                    cout<<hex<<uppercase<<Cache[i].Data[j];
                }
                cout<<endl;
            }
        cout<<"\n";
    }else{ // No read/write/display
        cout<<"Error"<<endl;}
        
    }}else{
            cout<<"ERROR OPENING FILE"<<endl;
        }

    
    //cout << "Hello, World!\n";
    return 0;
}
