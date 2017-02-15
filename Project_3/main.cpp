//
//  main.cpp
//  CS472 Project 3
//
//  Created by Maïté CAZENAVE on 28/04/15.
//  Copyright (c) 2015 Maïté CAZENAVE. All rights reserved.
//

#include <iostream>

using namespace std;

int C;

struct Regs_IFID{
    int Inst;
};

struct Regs_IDEX{
    int RegDst;
    int ALUSrc;
    int ALUOp;
    int MemRead;
    int MemWrite;
    int MemToReg;
    int RegWrite;
    int ReadReg1Value;
    int ReadReg2Value;
    int WriteReg_20_16;
    int WriteReg_15_11;
    int Function;
    short int SEOffset;
    string instruction;
    
    
};


struct Regs_EXMEM{
    int MemRead;
    int MemWrite;
    int MemToReg;
    int RegWrite;
    int ALUResult;
    int WriteRegNum;
    int SBValue;
    string instruction;
};

struct Regs_MEMWB{
    int MemToReg;
    int RegWrite;
    int ALUResult;
    int WriteRegNum;
    int LBDataValue;
    string instruction;
};

unsigned int InstructionCache[12]={0xA1020000,0x810AFFFC,0x00831820,0x01263820,0x01224820,0x81180000,0x081510010,0x00624022,0x00000000,0x00000000,0x00000000,0x00000000};

short Main_mem[2048];


int Regs[32];


Regs_IFID Read1={0};
Regs_IFID Write1={0};

Regs_IDEX Read2={0};
Regs_IDEX Write2={0};

Regs_EXMEM Read3={0};
Regs_EXMEM Write3={0};

Regs_MEMWB Read4={0};
Regs_MEMWB Write4={0};



void IF_stage(){
    Write1.Inst=InstructionCache[C];
};

void ID_stage(){
    int instr = Read1.Inst;
    int op_code=(instr & 0xFC000000)>>26;
    
    if(op_code==0x0){
        Write2.RegDst=1;
        Write2.ALUSrc=0;
        Write2.ALUOp=10;
        Write2.MemRead=0;
        Write2.MemWrite=0;
        Write2.MemToReg=0;
        Write2.RegWrite=1;
        Write2.ReadReg1Value=Regs[(instr& 0x03E00000)>>21]; 
        Write2.ReadReg2Value=Regs[(instr& 0x001F0000)>>16];
        Write2.WriteReg_20_16=(instr& 0x001F0000)>>16; 
        Write2.WriteReg_15_11=(instr& 0x0000F800)>>11; 
        Write2.Function=(instr& 0x0000003F); 
        
        switch(Write2.Function){
            case 0x20 : Write2.instruction="add";break;
            case 0x22 : Write2.instruction="sub";break;
            default   :
                if(C>2){
                    Write2={0};
                    Write2.instruction="NOP";
                }
                break;
        }
        
        
    }else{ 
        Write2.ReadReg1Value=Regs[(instr& 0x03E00000)>>21]; 
        Write2.ReadReg2Value=Regs[(instr& 0x001F0000)>>16];
        Write2.WriteReg_20_16=(instr& 0x001F0000)>>16;
        Write2.WriteReg_15_11=(instr& 0x0000F800)>>11;
        Write2.Function=(instr& 0x0000003F); 
        Write2.Function=(instr & 0xFC000000)>>26;
        Write2.SEOffset=(instr & 0x0000FFFF);
        
        switch(Write2.Function){
            case 0x20 :
                Write2.instruction="lb";
                Write2.RegDst=0;
                Write2.ALUSrc=1;
                Write2.ALUOp=00;
                Write2.MemRead=1;
                Write2.MemWrite=0;
                Write2.MemToReg=1;
                Write2.RegWrite=1;
                break;
            case 0x28 :
                Write2.instruction="sb";
                Write2.ALUSrc=1;
                Write2.ALUOp=00;
                Write2.MemRead=0;
                Write2.MemWrite=1;
                Write2.RegWrite=0;
                break;
        }
    }
};

void EX_stage(){
    
    Write3.instruction=Read2.instruction;
    Write3.MemRead=Read2.MemRead;
    Write3.MemWrite=Read2.MemWrite;
    Write3.MemToReg=Read2.MemToReg;
    Write3.RegWrite=Read2.RegWrite;
    
    if(Write3.instruction=="NOP"){
        Write3={0};
        Write3.instruction="NOP";
    }else{
    if(Read2.ALUOp==10){
        Write3.WriteRegNum=Read2.WriteReg_15_11;
        if(Read2.Function==0x20){
            Write3.ALUResult=Read2.ReadReg1Value+Read2.ReadReg2Value;
        }else if(Read2.Function==0x22){
            Write3.ALUResult=Read2.ReadReg1Value-Read2.ReadReg2Value;
        }
        
    }else if(Read2.ALUOp==00){
        Write3.ALUResult=Read2.ReadReg1Value+Read2.SEOffset;
        if(Read2.RegDst==0){
            Write3.WriteRegNum=Read2.WriteReg_20_16;
        }else{
            Write3.SBValue=Read2.ReadReg2Value;
        }
        
    }}
};

void MEM_stage(){
    Write4.instruction=Read3.instruction;
    Write4.ALUResult=Read3.ALUResult;
    Write4.MemToReg=Read3.MemToReg;
    Write4.RegWrite=Read3.RegWrite;
    Write4.WriteRegNum=Read3.WriteRegNum;
    
    if(Write4.instruction=="NOP"){
        Write4={0};
        Write4.instruction="NOP";
    }else{
    if(Read3.MemRead==1){
        Write4.LBDataValue=Main_mem[Read3.ALUResult];
    }
    if(Read3.MemWrite==1){
        Main_mem[Read3.ALUResult]=Read3.SBValue;
    }}
};

void WB_stage(){
    if(Read4.RegWrite==1){
        if(Read4.MemToReg==0){
            Regs[Read4.WriteRegNum]=Read4.ALUResult;
        }else{
            Regs[Read4.WriteRegNum]=Read4.LBDataValue;
        }
    }
   
};


void Print_out_everything(){
    cout<<"CYCLE "<<C+1<<endl;
    cout<<"Registers"<<endl;
    for(int i=0;i<32;i++){
        cout<<"$"<<dec<<i<<" : "<<uppercase<<hex<<Regs[i]<<endl;
    }
    cout<<"\n"<<endl;
    
    cout<<"IF/ID Pipeline Write Version:"<<endl;
    cout<<"Inst = "<<uppercase<<hex<<Write1.Inst<<endl;
    cout<<"\n";
    
    cout<<"IF/ID Pipeline Read Version:"<<endl;
    cout<<"Inst = "<<uppercase<<hex<<Read1.Inst<<endl;
    cout<<"\n"<<endl;
    
    cout<<"ID/EX Pipeline Write Version:"<<endl;
    cout<<"Control: RegDst="<<Write2.RegDst<<", ALUSrc="<<Write2.ALUSrc<<", ALUOp="<<Write2.ALUOp<<", MemRead="<<Write2.MemRead<<", MemWrite="<<Write2.MemWrite<<", MemToReg="<<Write2.MemToReg<<", RegWrite="<<Write2.RegWrite<<" ["<<Write2.instruction<<"]"<<endl;
    cout<<"ReadReg1Value = "<<uppercase<<hex<<Write2.ReadReg1Value<<endl;
    cout<<"ReadReg2Value = "<<uppercase<<hex<<Write2.ReadReg2Value<<endl;
    cout<<"SEOffset = "<<uppercase<<hex<<Write2.SEOffset<<endl;
    cout<<"WriteReg_20_16 = "<<dec<<Write2.WriteReg_20_16<<endl;
    cout<<"WriteReg_15_11 = "<<dec<<Write2.WriteReg_15_11<<endl;
    cout<<"Function = "<<uppercase<<hex<<Write2.Function<<endl;
    cout<<"\n";
    
    cout<<"ID/EX Pipeline Read Version:"<<endl;
    cout<<"Control: RegDst="<<Read2.RegDst<<", ALUSrc="<<Read2.ALUSrc<<", ALUOp="<<Read2.ALUOp<<", MemRead="<<Read2.MemRead<<", MemWrite="<<Read2.MemWrite<<", MemToReg="<<Read2.MemToReg<<", RegWrite="<<Read2.RegWrite<<" ["<<Read2.instruction<<"]"<<endl;
    cout<<"ReadReg1Value = "<<uppercase<<hex<<Read2.ReadReg1Value<<endl;
    cout<<"ReadReg2Value = "<<uppercase<<hex<<Read2.ReadReg2Value<<endl;
    cout<<"SEOffset = "<<uppercase<<hex<<Read2.SEOffset<<endl;
    cout<<"WriteReg_20_16 = "<<dec<<Read2.WriteReg_20_16<<endl;
    cout<<"WriteReg_15_11 = "<<dec<<Read2.WriteReg_15_11<<endl;
    cout<<"Function = "<<uppercase<<hex<<Read2.Function<<endl;
    cout<<"\n"<<endl;
    

    cout<<"EX/MEM Pipeline Write Version:"<<endl;
    cout<<"Control: MemRead="<<Write3.MemRead<<", MemWrite="<<Write3.MemWrite<<", MemToReg="<<Write3.MemToReg<<", RegWrite="<<Write3.RegWrite<<" ["<<Write3.instruction<<"]"<<endl;
    cout<<"ALUResult = "<<uppercase<<hex<<Write3.ALUResult<<endl;
    cout<<"SBValue = "<<uppercase<<hex<<Write3.SBValue<<endl;
    cout<<"WriteRegNum = "<<dec<<Write3.WriteRegNum<<endl;
    cout<<"\n";
    
    cout<<"EX/MEM Pipeline Read Version:"<<endl;
    cout<<"Control: MemRead="<<Read3.MemRead<<", MemWrite="<<Read3.MemWrite<<", MemToReg="<<Read3.MemToReg<<", RegWrite="<<Read3.RegWrite<<" ["<<Read3.instruction<<"]"<<endl;
    cout<<"ALUResult = "<<uppercase<<hex<<Read3.ALUResult<<endl;
    cout<<"SBValue = "<<uppercase<<hex<<Read3.SBValue<<endl;
    cout<<"WriteRegNum = "<<dec<<Read3.WriteRegNum<<endl;
    cout<<"\n"<<endl;
    
    
    cout<<"MEM/WB Pipeline Write Version:"<<endl;
    cout<<"Control: MemToReg="<<Write4.MemToReg<<", RegWrite="<<Write4.RegWrite<<" ["<<Write4.instruction<<"]"<<endl;
    cout<<"ALUResult = "<<uppercase<<hex<<Write4.ALUResult<<endl;
    cout<<"LBValue = "<<uppercase<<hex<<Write4.LBDataValue<<" @ "<<hex<<Write4.ALUResult<<endl;
    cout<<"WriteRegNum = "<<dec<<Write4.WriteRegNum<<endl;
    cout<<"\n";
    
    cout<<"MEM/WB Pipeline Read Version:"<<endl;
    cout<<"Control: MemToReg="<<Read4.MemToReg<<", RegWrite="<<Read4.RegWrite<<" ["<<Read4.instruction<<"]"<<endl;
    cout<<"ALUResult = "<<uppercase<<hex<<Read4.ALUResult<<endl;
    cout<<"LBValue = "<<uppercase<<hex<<Read4.LBDataValue<<" @ "<<hex<<Read4.ALUResult<<endl;
    cout<<"WriteRegNum= "<<dec<<Read3.WriteRegNum<<endl;
    cout<<"\n"<<endl;
}

void Copy_write_to_read(){
    Read1=Write1;
    Read2=Write2;
    Read3=Write3;
    Read4=Write4;
}

int main()
{
    for(int i=0;i<2048;i++){
        Main_mem[i]=i%0x100;
    }
    
    Regs[0]=0x0;
    for(int i=1;i<32;i++){
        Regs[i]=i+0x100;
    }


    for(C=0;C<12;C++){
        IF_stage();
        ID_stage();
        EX_stage();
        MEM_stage();
        WB_stage();
        Print_out_everything();
        Copy_write_to_read();
    }
    return 0;
}
       
/* 
 */
