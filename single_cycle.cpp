#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <list>
#include <tuple>
using namespace std;

char* Fetch( string* instructions, char* curr_inst); // gets instruction
tuple<string*, string*> Decode(char* curr_inst, string* fields, int pc, string* signals); // Straightforward, decodes instruction & generates signals
tuple<int*, int*> Execute(int* registerfile, int* dmem,string* fields,string* signals); //Does the instruction
tuple<int*, int*>Mem(int* registerfile, int* dmem, string* fields, string* signals); // Needed for sw and lw to work
tuple<int*, int*> Writeback(int* registerfile, int* dmem, string* fields, string* signals); //what does this even do
string* ControlUnit(string opcode, string* signals, string funct); //For the sake of having all required methods declared beforehand

string* AllRegisters = new string[32]; //self explanatory Refer to lines 493-524
int pc = 0 , next_pc = 4, jump_target = 0, branch_target = 0, total_clock_cycles = 0; //Global Instructions
char nextBranch;

int mod(int x, int y){
    while(x>=y){
        x-=y;
    }
    return x;
}
char* StringtoChars(string* str){
    string s = str[(pc/4)-1];
    //cout<<dec<<pc/4<<endl;
    char* chars = new char[32];
    for(int i = 0; i<32; i++){
            chars[i] = s[i];
        }
    //cout<<"In String to Chars "<<chars<<endl;
    return chars;
}
string* zeroFields(string* fields, int x){
    for(int i = 0; i<x; i++){
        fields[i] = "NA";
    }
    return fields;
}
char* getOpcode (char *bits){ //First 6 bits
    int i = 0 ;
    char* opcode = new char[6]; 
    for (i = 0 ; i < 6 ; i++){
        opcode[i] = bits[i];
    }
    return (opcode); 
}
int power(int x, int y){
    int val = 1;
    for(int i = y; i >=1; i--){
        val = val * x;
    }
    return val;
}
int bitsToDec(string input){
    char* bits = new char[input.length()];
    int sum = 0;
    int j = -1;
    for(int i = 0;i<input.length();i++ ){
        bits[i] = input[i];
    }
    for(int i = strlen(bits); i>=0; i-- ){   
        if(bits[i] == 49){
            sum += power(2,j);
        }
        j++;
    }
    return sum; 
}
int bitsToDec2(string input){
    //char* bits = new char[input.length()];
    int sum = 0;
    int j = -1;
    // cout<<input.length()<<endl;
    // for(int i  = 0; i<input.length(); i++){
    //     cout<<input[i];
    // }cout<<endl;
    for(int i = input.length(); i>=0; i-- ){   
        if(input[i] == 49){
            //cout<<j<< dec<<endl;
            //cout<<i<<dec<<" this i"<<endl;
            sum += power(2,j);
        }
        j++;
    }
    return sum; 
}

int* decToBinary(int n){
    // array to store binary number
    int binaryNum[32];
 
    // counter for binary array
    int i = 0;
    while (n > 0) {
 
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
    int* bitsReturned = new int[32];
    for(int l = 0;l<33;l++){
        bitsReturned[l]=2;
    }
    int k = 0;
    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--){
        bitsReturned[k] = binaryNum[j];
        //cout << binaryNum[j];
        k++;
    }
    //cout<<endl;
    for(int j = 32; j>=0; j--){
        //cout<<binaryNum[j]<<endl;
    }
    //cout<<endl;
    return bitsReturned;
}

char* IntToCharBits(int* IntBits){
    char* tempBitsC = new char[32];
    for(int i = 0;i<33; i++){
        if(IntBits[i] == 1){
            tempBitsC[i] = 49;
        }else if(IntBits[i] ==0){
            tempBitsC[i] = 48;
        } 
    }
    char* bitsPc = new char[32];
    for(int i = 32-strlen(tempBitsC);i>=0;i-- ){
        bitsPc[i] = 48;
    }
    int p = 0;
    for(int i = 32-strlen(tempBitsC); i<33; i++){
        bitsPc[i] = tempBitsC[p];
        p++;
    }
    return bitsPc;
    //return tempBitsC;
}
char* signExtend(string input){
    char* bits = new char[input.length()];
    char* extension = new char[32];
    int sum = 0;
    int j = 0;
    for(int i = 0;i<input.length();i++ ){
        bits[i] = input[i];
    }
    //cout<<bits<<endl;
    char MSB = bits[0];
    //cout<<MSB<<endl;
    int k = 0;
    for (int i = 0;i<32; i++ ){
        if (i<=15){
            extension[i] = MSB; //why no work
        }else{
            //cout<<extension<<endl;
            extension[i] = bits[k];
            k++;
        }
    }
    return extension;
}
char* shiftLeft2 (char *extension){//moves bits left 2 spaces 
    char* shift = new char[32];
    int j = 0;
    for(int i = 2; i<32; i++){
        shift[j] = extension[i];
        j++;
    }
    shift[30] = 48;
    shift[31] = 48;

    return shift;
}


char* getRs (char *bits){ //5 bits from 6-10 (R and I)
    int i = 6;
    int j = 0;
    char* rs = new char[5];
    for(i; i<=10; i++){
        rs[j] = bits[i];
        j++;
    }

    return rs;
}
char* getRt ( char *bits){ //5 bits from 11-15 (R and I)
    int i = 11;
    int j = 0; 
    char* rt = new char[5];
    for(i; i<=15; i++){
        rt[j] = bits[i];
        j++;
    }
    return rt;
}
char* getRd ( char *bits){ //5 bits from 16-20 (R)
    int i= 16;
    int j = 0;
    char* rd = new char[5];
    for(i; i<=20; i++){
        rd[j] = bits[i];
        j++;
    }
    return rd;
}

char* getFunct ( char *bits){ // bits from 26-31 (R)
    int i = 26;
    int j = 0; 
    char* funct = new char[6];
    for(i=26; i<32; i++){
        funct[j] = bits[i];
        j++;
    }
    
    return funct;
}

char* getImmediate ( char *bits){ // bits from 16-31 (I)
    int i = 16;
    int j = 0;
    char* inter = new char[16];
    for(i; i<32; i++){
        inter[j] = bits[i];
        j++;
    }
    return inter;
}

char* getAddress ( char *bits){ // 6-31 (J)
    int i = 6;
    int j = 0 ;
    char* addr = new char[26];
    for(i; i<=32; i++){
        addr[j] = bits[i];
        j++;
    }
    return addr; 
}
char* getShamt(char *bits){// 21-25 (R)
    int i = 21;
    int j = 0;
    char* sham = new char[5];
    for(i; i<=25; i++){
        sham[j] = bits[i];
        j++;
    }
    return sham;
}

char* Fetch( string* instructions, char* curr_inst){
    if (pc != 0 ){
        if(nextBranch == 106){ //j=106
            pc = jump_target;
            next_pc = pc+4;
            curr_inst = StringtoChars(instructions);
            nextBranch = 48;
        }
        else if(nextBranch == 98){//b = 98
            //cout<<"branch target"<<endl;
            //cout<<hex<<branch_target<<endl;
           
            pc = branch_target;
            next_pc = pc+4;
            curr_inst = StringtoChars(instructions);
            nextBranch = 48;
        }else{
            pc = next_pc;
            next_pc=pc+4;
            curr_inst = StringtoChars(instructions);
            nextBranch = 48;
        }

    }else{//1st iteration so we start from here
        //cout<<"non branch"<<endl;
        pc = next_pc;
        next_pc=pc+4; //always increments by 4
        
        curr_inst = StringtoChars(instructions);
        
    }
    //cout<<"pc is modified to 0x";
    //cout<<hex<<pc<<""<<endl;
    
    return curr_inst;
}

tuple<string*, string*> Decode(char* curr_inst, string* fields, int pc, string* signals){
    fields[0] = getOpcode(curr_inst);

    if(fields[0] == "000000"){ // R type Inst
        fields[1] = getRs(curr_inst);
        fields[2] = getRt(curr_inst);
        fields[3] = getRd(curr_inst);
        fields[4] = getShamt(curr_inst);
        fields[5] = getFunct(curr_inst);
    }else if(fields[0] == "000010"){// J type
        //cout<<"WE J NO LESS"<<endl;
        fields[7] = getAddress(curr_inst);
        
        int* bits = new int[32];
        char* pcChar32 = new char[32];
        bits = decToBinary(pc);
        pcChar32 = IntToCharBits(bits);
        //cout<<pcChar32<<" this is pc char"<<endl;
        string jumpBits;
        jumpBits.append(1,pcChar32[0]);
        jumpBits.append(1,pcChar32[1]);
        jumpBits.append(1,pcChar32[2]);
        jumpBits.append(1,pcChar32[3]);
        //cout<<jumpBits<<endl;
        jumpBits= jumpBits+fields[7];
        jumpBits=jumpBits + "00";
        //cout<<jumpBits<<endl;
        fields[9] = jumpBits;
        //cout<<fields[9]<<" is calculated jump branch in var fields[9]"<<endl;
        jump_target = bitsToDec2(fields[9]);
        //cout<<jump_target<<dec<<" is our calculated jump target"<<endl;

    }else{                          //I type Inst
        fields[1] = getRs(curr_inst);
        fields[2] = getRt(curr_inst);
        fields[6] = getImmediate(curr_inst);
        if(fields[0] == "000100"){ // Beq
            char* extendedInt = new char[32];
            char* shiftedInt = new char[32];
            int shiftedIntDec = 0;
            extendedInt = signExtend(fields[6]);
            shiftedInt = shiftLeft2(extendedInt);
            //cout<<shiftedInt<<" is shifted INt"<<endl;
            shiftedIntDec = bitsToDec(shiftedInt);
            //cout<<dec<<shiftedIntDec<<dec<<endl;
            branch_target = next_pc + shiftedIntDec;
            //cout<<dec<<branch_target<<endl;
        }
    }
    
    signals = ControlUnit(fields[0], signals, fields[5]);

    return make_tuple(fields, signals); 
}
string* ControlUnit(string opcode, string* signals, string funct){
    if (opcode == "000000"){// R type
        signals[0] = "0";
        signals[1] = "1";
        signals[2] = "0";
        signals[3] = "0";
        signals[4] = "1";
        signals[5] = "0";
        signals[6] = "0";
        signals[7] = "0";
        signals[8] = "10"; 
        if(funct == "100000"){ // add
            signals[9] = "0010";
        }else if(funct == "100010"){ //subtract
            signals[9] = "0110";
        }else if(funct == "100100"){ // AND
            signals[9] = "0000";
        }else if(funct == "100101"){ // OR
            signals[9] = "0001";
        }else if(funct == "101010"){ // SLT
            signals[9] = "0111";
        }else if(funct == "100111"){ // NOR
            signals[9] = "1100";
        }

        
    }else if(opcode == "000010"){ // j
        signals[0] = "1";
        signals[4] = "0";
        signals[5] = "0";
        signals[6] = "0";
        signals[7] = "0";
    }else if(opcode == "100011"){ // lw
        signals[0] = "0";
        signals[1] = "0";
        signals[2] = "1";
        signals[3] = "1";
        signals[4] = "1";
        signals[5] = "1";
        signals[6] = "0";
        signals[7] = "0";
        signals[8] = "00";
        signals[9] = "0010";
    }else if(opcode == "101011"){ // sw
        signals[0] = "0";
        signals[1] = "0";
        signals[2] = "1";
        signals[3] = "1";
        signals[4] = "1";
        signals[5] = "1";
        signals[6] = "0";
        signals[7] = "0";
        signals[8] = "00";
        signals[9] = "0010";
    }else if(opcode == "000100"){ //beq
        
        signals[0] = "0";
        signals[2] = "0";
        signals[4] = "0";
        signals[5] = "0";
        signals[6] = "0";
        signals[7] = "1";
        signals[8] = "01";
        signals[9] = "0110";
    }

    return signals; 
}
tuple<int*, int*> Execute(int* registerfile, int* dmem,string* fields,string* signals){   
    //cout<<AllRegisters[5]<<endl;
    int rs=0, rt =0, rd =0, shamt = 0, imm = 0; 
    //cout<<"in execute"<<endl;
    //cout<<fields[0]<<endl;
    if (fields[0] == "000000"){ // R to execute
        nextBranch = 48;
        rs = bitsToDec2(fields[1]);
        //cout<<"rs "<<dec<<rs<<endl;
        rt = bitsToDec2(fields[2]);
        //cout<<rt<<endl;
        rd = bitsToDec2(fields[3]);
        //cout<<rd<<endl;
        shamt = bitsToDec2(fields[4]);
        if(signals[9] == "0010"){ // add
            registerfile[rd] = registerfile[rs] + registerfile[rt];        
        }else if(signals[9] == "0110"){// sub
            registerfile[rd] = registerfile[rs] - registerfile[rt];
        }else if(signals[9] == "0000"){ // and
            registerfile[rd] = registerfile[rs] & registerfile[rt];
        }else if(signals[9] == "0001"){ // or
            registerfile[rd] = registerfile[rs] || registerfile[rt];
        }else if(signals[9] == "0111"){ // slt
            if(registerfile[rs]<registerfile[rt]){
                registerfile[rd] = 1;
            }else{
                registerfile[rd] = 0;
            }
        }else if(signals[9] == "1100"){ // nor
            registerfile[rd] = !(registerfile[rs] || registerfile[rt]);
        }
        cout<<AllRegisters[rd]<<" is modified to 0x"<<hex<<registerfile[rd]<<endl;
    }
    else if(fields[0] == "000100"){//beq to execute
        rs = bitsToDec2(fields[1]);
        rt = bitsToDec2(fields[2]); 
        if(registerfile[rs] == registerfile[rt]){
            nextBranch = 98;
            pc = branch_target -4;
        }
    }
    else if(fields[0] == "000010"){// J to execute
        nextBranch = 106;
    }
    return make_tuple(registerfile, dmem);
}

tuple<int*, int*> Mem(int* registerfile, int* dmem, string* fields, string* signals){
    int rs=0, rt =0, SEimm = 0; 
    rs = bitsToDec2(fields[1]);
    rt = bitsToDec2(fields[2]); 
    char* extendedInt = new char[32];
    extendedInt = signExtend(fields[6]); 
    SEimm = bitsToDec2(extendedInt);

    if(fields[0] == "101011"){ // sw stores a value into mem file
        //M[R[rs]+SignExtImm] = R[rt] 
        dmem[(registerfile[rs] + SEimm)/4] = registerfile[rt];
        cout<<"memory 0x"<<hex<<(registerfile[rs] + SEimm)<<" is modified to 0x"<<hex<<registerfile[rt]<<endl;
    }
    else if(fields[0] == "100011"){ // lw stores a value from mem to register
        //R[rt] = M[R[rs]+SignExtImm] 
        registerfile[rt] =  dmem[(registerfile[rs] + SEimm)/4];
        cout<<AllRegisters[rt]<<" is modified to 0x"<<hex<<registerfile[rt]<<endl;
    }
    return make_tuple(registerfile, dmem);
}

tuple<int*, int*> Writeback(int* registerfile, int* dmem, string* fields, string* signals){
    total_clock_cycles+= 1;
    cout<<dec<<"total_clock_cycles "<<total_clock_cycles<<" "<<endl;
    return make_tuple(registerfile, dmem);
}

int main(){
    AllRegisters[0] = "$zero";
    AllRegisters[1] = "$at";
    AllRegisters[2] = "$v0";
    AllRegisters[3] = "$v1";
    AllRegisters[4] = "$a0";
    AllRegisters[5] = "$a1";
    AllRegisters[6] = "$a2";
    AllRegisters[7] = "$a3";
    AllRegisters[8] = "$t0";
    AllRegisters[9] = "$t1";
    AllRegisters[10] = "$t2";
    AllRegisters[11] = "$t3";
    AllRegisters[12] = "$t4";
    AllRegisters[13] = "$t5";
    AllRegisters[14] = "$t6";
    AllRegisters[15] = "$t7";
    AllRegisters[16] = "$s0";
    AllRegisters[17] = "$s1";
    AllRegisters[18] = "$s2";
    AllRegisters[19] = "$s3";
    AllRegisters[20] = "$s4";
    AllRegisters[21] = "$s5";
    AllRegisters[22] = "$s6";
    AllRegisters[23] = "$s7";
    AllRegisters[24] = "$t8";
    AllRegisters[25] = "$t9";
    AllRegisters[26] = "$k0";
    AllRegisters[27] = "$k1";
    AllRegisters[28] = "$gp";
    AllRegisters[29] = "$sp";
    AllRegisters[30] = "$fp";
    AllRegisters[31] = "$ra";
    nextBranch = 48;


    //int pc = 0 , next_pc = 4, jump_target = 0, branch_target = 0, total_clock_cycles = 0; //things we needs all set to 0
    string* signals = new string[10]; //opcode then the 10 signals
    int* registerfile = new int[32]; 
    int* dmem = new int[32];

    string* instructions = new string[50]; //holds all instructions max of 50
    char* curr_inst = new char[32]; // is the current instruction we will work with. 1 at a time since single cycle
     
    string* fields = new string[11]; //opcode, rs, rt, rd, shamt, funct, immediate, address, 9 is Calced Jump address, 10 is calced branch address, 11 is free space
    bool sim = true;
    
    registerfile[9] = 32 ; //$t1 = 0x20 or 32 in decimal
    registerfile[10] = 5 ; //$t2 = 0x5 or 5
    registerfile[16] = 112 ; //$s0 = 0x70 or 112 in decimal
    dmem[28] = 5; //0x70 = 0x5  HexAddr = HexVal
    dmem[29] = 16; //0x74 = 0x10

    // READING INPUITS FROM FILE
    int cnt = 0;
    string input;
    string x;
    cin>>x;
    ifstream is(x); //TO USE DIFFERENT TEXT FILE CHANGE THIS
    while (cnt < 50 && is >>input){
        instructions[cnt++] = input;
    }
    //DONE READING FILE
    // for(int i=0;i<cnt; i++){
    //     cout<<instructions[i]<<endl; //test to see if inpuit worked
    // }
    //cout<<dec<<instructions.length()<<endl;
    //cout<<dec<<cnt<<" is instructions count"<<endl;
    int i = 0 ; 
    while(cnt*4 > pc){
        //cout<<dec<<pc<<" is pc"<<endl;
        //cout<<dec<<pc%4<<" is pc mod 4"<<endl;
        fields = zeroFields(fields, 11); //Fills each field with "NA" to zero them all out before a fetch for next instruction
        signals = zeroFields(signals, 9);
        curr_inst = Fetch( instructions,  curr_inst);
    
        // if((cnt*4)<pc)
        //     break;
        tie(fields, signals) = Decode(curr_inst, fields, pc, signals); 
        tie(registerfile, dmem) = Execute(registerfile, dmem, fields, signals);
        tie(registerfile, dmem) = Mem(registerfile, dmem, fields, signals);
        tie(registerfile, dmem) = Writeback(registerfile, dmem, fields, signals);
        // total_clock_cycles+= 1;
        // cout<<dec<<"total_clock_cycles "<<total_clock_cycles<<" :"<<endl;
        cout<<"pc is modified to 0x";
        cout<<hex<<pc<<"\n\n"<<endl;
    }
    //cout<<hex<<pc<<endl;
    cout<<"\nProgram Terminated \nTotal execution time is "<<total_clock_cycles<<" cycles"<<endl;
    //cout<<curr_inst<<endl;
    //cout<<pc<<endl;
    

    return 1;
}