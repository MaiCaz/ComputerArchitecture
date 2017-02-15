//
//  main.cpp
//  CS472 Project 1
//
//  Created by Maïté CAZENAVE on 18/02/15.
//  Copyright (c) 2015 Maïté CAZENAVE. All rights reserved.
//
#include <iostream>
#include <string>

using namespace std;

int main()
{
    // Array of hexadecimal addresses 
    unsigned int address_hexa[11]={0x022DA822,0x8EF30018,0x12A70004,0x02689820,0xAD930018,0x02697824,0xAD8FFFF4,0x018C6020, 0x02A4A825, 0x158FFFF6, 0x8E59FFF0};
    
    int start=0x7A060; // Departure address
    
    // Declaration of variables
    short int offset;
    int _function;
    int op_code;
    int target_address;
    int register1,register2,register3;
    string instruction;
    
    for(int i=0;i<11;i++){ // one address at a time
        
        cout<<"\n\t";
        cout<<hex<<uppercase<<start<<"\t"; // Display address of the instruction

        op_code=(address_hexa[i] & 0xFC000000)>>26;// Op_code : Bits 26 to 31
        
        
        if(op_code==0x0){ // R Format
            
            register1=(address_hexa[i] & 0x03E00000)>>21; //Source Register : Bits 21 to 25 
            register2=(address_hexa[i] & 0x001F0000)>>16; //Source Register : Bits 16 to 20
            register3=(address_hexa[i] & 0x0000F800)>>11; //Destination Register : Bits 11 to 15
            _function=(address_hexa[i] & 0x0000003F);     //Function : Bits 0 to 5 
            
            switch(_function){ // Find the correct instruction thanks to function bits
                case 0x20 : instruction="add";break;
                case 0x22 : instruction="sub";break;
                case 0x24 : instruction="and";break;
                case 0x25 : instruction="or ";break;
                default   : instruction="error";break;
            }

            // Display the line of the decoding instruction
            cout<<instruction<<dec<<" $"<<register1<<", $"<<register2<<", $"<<register3;
            //"dec" allows to display register in decimal
            
            
        }else{ // I Format
            
            register1=(address_hexa[i] & 0x03E00000)>>21; //Source Register : Bits 21 to 25  
            register2=(address_hexa[i] & 0x001F0000)>>16; //Source/Destination Register : Bits 16 to 20
            offset=(address_hexa[i] & 0x0000FFFF);        //Offset : Bits 0 to 15
            
            switch(op_code){ // Find the correct instruction thanks to the op_code bits
                case 0x2A : instruction="slt";break; 
                case 0x23 : instruction="lw ";break;
                case 0x2B : instruction="sw ";break;
                case 0x4  : instruction="beq";break;
                case 0x5  : instruction="bne";break;
                default   : instruction="error";break;
            }
            
            // Branches case
            if(instruction=="beq" || instruction=="bne" || instruction=="slt"){
                
                offset=offset<<2; 
                target_address=start+4+offset;
                
                // Display the line of the decoding instruction
                cout<<instruction<<dec<<" $"<<register1<<", $"<<register2<<"        "<<hex<<target_address;
                // "hex" allows to display the value in hexadecimal
                
            }else if(instruction != "error"){ // Otherwise except if it's an error
                
                // Display the line of the decoding instruction 
                cout<<instruction<<dec<<" $"<<register2<<","<<" "<<offset<<"($"<<register1<<")";
                
            }
        }
        
        cout<<endl;
        start=start+4; // Incrementation of the departure address with 4 bits (size of the instruction)
    }
    return 0;
}
