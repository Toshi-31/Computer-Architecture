using namespace std;
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <bitset>
#include <sstream>
#include <stdio.h>

int binary_to_int(string bin_num)
{
    int num = 0;
    int iter = 0;
    for (int i = bin_num.size() - 1; i >= 0; i--)
    {
        num += (bin_num[i] - '0') * pow(2, iter);
        iter++;
    }
    return num;
}
string hex_to_bin(string hexdec)
{
    // Skips "0x" if present at beggining of Hex string
    size_t i = (hexdec[1] == 'x' || hexdec[1] == 'X') ? 2 : 0;
    string ans = "";
    while (hexdec[i])
    {

        switch (hexdec[i])
        {
        case '0':
            ans += "0000";
            break;
        case '1':
            ans += "0001";
            break;
        case '2':
            ans += "0010";
            break;
        case '3':
            ans += "0011";
            break;
        case '4':
            ans += "0100";
            break;
        case '5':
            ans += "0101";
            break;
        case '6':
            ans += "0110";
            break;
        case '7':
            ans += "0111";
            break;
        case '8':
            ans += "1000";
            break;
        case '9':
            ans += "1001";
            break;
        case 'A':
        case 'a':
            ans += "1010";
            break;
        case 'B':
        case 'b':
            ans += "1011";
            break;
        case 'C':
        case 'c':
            ans += "1100";
            break;
        case 'D':
        case 'd':
            ans += "1101";
            break;
        case 'E':
        case 'e':
            ans += "1110";
            break;
        case 'F':
        case 'f':
            ans += "1111";
            break;
        case '.':
            ans += ".";
            break;
        default:
            cout << "\nInvalid hexadecimal digit " << hexdec[i];
        }
        i++;
    }
    return ans;
}

void CacheSim(vector<vector<bool>> &dirtybits,int &hits, int &misses, vector<vector<string>> &cache, bool l, string addr, int sets, int blocks_per_set, int bytes_per_block, bool write_allocate,
              bool write_through, bool lru, int &load_hits, int &load_misses, int &store_hits,
              int &store_misses, int &total_cycles)
{
        // for(int i=0;i<sets;i++){
        //     for(int j=0;j<blocks_per_set;j++){
        //         cout<<i<<" "<<j<<" "<<dirtybits[i][j]<<endl;
        //     }
        // }
        // for(int i=0;i<sets;i++){
        //     for(int j=0;j<blocks_per_set;j++){
        //         cout<<i<<" "<<j<<" "<<cache[i][j]<<endl;
        //     }
        // }
        // set-associative
        int block_offset = log2(bytes_per_block);
        int no_index_bits = log2(sets);
        int no_tag_bits = 32 - block_offset - no_index_bits;
        string addr1 = hex_to_bin(addr);
        string tag_bits = addr1.substr(0, no_tag_bits);
        string index_bits = addr1.substr(no_tag_bits, no_index_bits);
        int index = (binary_to_int(index_bits)) % sets;
        // cout << "index " << index << endl;
        bool hit = false;
        bool empty = false;
        if (lru)
        {
            for (int i = 0; i < blocks_per_set; i++)
            {
                // cout << i << cache[index][i] << " ";
                if (cache[index][i] == tag_bits)
                {
                    hit = true;
                    // if(!write_through){
                    // dirtybits[index][i]=true;}
                    // break;
                    if(!write_through && !l){
                    vector<bool> dirty_replacer(blocks_per_set);
                    dirty_replacer[0] = true;
                    int ptrr1 = 1;
                    int ptrr2 = 0;
                    while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                    {
                        if (ptrr2!=i)
                        {
                            dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                            ptrr1++;
                            ptrr2++;
                        }
                        else
                        {
                            ptrr2++;
                        }
                    }
                    dirtybits[index] = dirty_replacer;
                    }
                    if(!write_through && l){
                    vector<bool> dirty_replacer(blocks_per_set);
                    dirty_replacer[0] = dirtybits[index][i];
                    int ptrr1 = 1;
                    int ptrr2 = 0;
                    while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                    {
                        if (ptrr2!=i)
                        {
                            dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                            ptrr1++;
                            ptrr2++;
                        }
                        else
                        {
                            ptrr2++;
                        }
                    }
                    dirtybits[index] = dirty_replacer;
                    }
                    vector<string> replacee(blocks_per_set);
                    replacee[0] = tag_bits;
                    int ptr1 = 1;
                    int ptr2 = 0;
                    while (ptr1 < blocks_per_set && ptr2 < blocks_per_set)
                    {
                        if (cache[index][ptr2] != tag_bits)
                        {
                            replacee[ptr1] = cache[index][ptr2];
                            ptr1++;
                            ptr2++;
                        }
                        else
                        {
                            ptr2++;
                        }
                    }
                    cache[index] = replacee;
                    break;
                }
            }
            // cout << endl;
        }
        else
        {
            for (int i = 0; i < blocks_per_set; i++)
            {
                if (cache[index][i] == tag_bits)
                {
                    // cout << "fifo hit" << endl;
                    hit = true;
                    if(!write_through && !l){
                    dirtybits[index][i]=true;}
                    break;
                }
            }
        }
        
        if (hit)
        {
            hits++;
            if (l == true)
            {
                // total_cycles++;
                load_hits++;
            }
            else
            {
                // total_cycles++;
                store_hits++;
                // if(write_allocate){

                // }
            }
        }
        if(hit){
            if(write_allocate && write_through && l){
                total_cycles++;
            }
            if(write_allocate && write_through && !l){
                total_cycles+=101;
            }
            if(write_through && !write_allocate && l){
                total_cycles++;
            }
            if(write_through && !write_allocate && !l){
                total_cycles+=101;
            }
            if(!write_through && l){
                total_cycles++;
            }
            if(!write_through && !l){
                total_cycles++;
            }
        }
        else if(!hit)
        {
            misses++;
            if (l)
            {
                load_misses++;
            }
            else
            {
                store_misses++;
            }
            if (!l)
            {
                if (write_allocate)
                {
                    for (int i = 0; i < blocks_per_set; i++)
                    {
                        if (cache[index][i] == "")
                        {
                            empty = true;
                            break;
                        }
                    }

                    if (empty)
                    {
                        for (int i = 0; i < blocks_per_set; i++)
                        {
                            if (cache[index][i] == "")
                            {
                                if (lru)
                                {
                                    if(write_through){
                                        vector<bool> dirty_replacer(blocks_per_set);
                                        dirty_replacer[0] = false;
                                        int ptrr1 = 1;
                                        int ptrr2 = 0;
                                        while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                                        {
                                                dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                                ptrr1++;
                                                ptrr2++;
                                    
                                    }
                                    dirtybits[index] = dirty_replacer;
                                    }
                                    if(!write_through && !l){
                                    vector<bool> dirty_replacer(blocks_per_set);
                                    dirty_replacer[0] = true;
                                    int ptrr1 = 1;
                                    int ptrr2 = 0;
                                    while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                                    {
                                            dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                            ptrr1++;
                                            ptrr2++;
                                    
                                    }
                                    dirtybits[index] = dirty_replacer;
                                    }
                                    vector<string> replacee(blocks_per_set);
                                    replacee[0] = tag_bits;
                                    int ptr1 = 1;
                                    int ptr2 = 0;
                                    while (ptr1 < blocks_per_set && ptr2 < blocks_per_set)
                                    {
                                        replacee[ptr1] = cache[index][ptr2];
                                        ptr1++;
                                        ptr2++;
                                    }
                                    cache[index] = replacee;
                                }
                                else
                                {
                                    cache[index][i] = tag_bits;
                                    if(!write_through){
                                        dirtybits[index][i]=true;
                                    }
                                    else{
                                        dirtybits[index][i]=false;
                                    }
                                }
                                if (l == true)
                                {
                                    if (write_allocate && !write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    if(write_allocate && write_through){
                                        total_cycles+=(25 * (bytes_per_block)) + 1;
                                    }
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                }
                                else
                                {
                                    // store_misses++;
                                    if (write_allocate && write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block)) + 100+1;
                                    }
                                    else if (write_allocate && !write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    else if (!write_allocate)
                                    {
                                        total_cycles += 100; // confirm
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        bool dirty_on=false;
                        if (lru)
                        {
                            if(!write_through && dirtybits[index][blocks_per_set-1]==true){
                                dirty_on=true;
                            }
                            if(!write_through){
                                vector<bool> dirty_replacer(blocks_per_set);
                                dirty_replacer[0]=true;
                                int ptrr1=1;
                                int ptrr2=0;
                                while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                            {
                                dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                ptrr1++;
                                ptrr2++;
                            }
                            dirtybits[index] = dirty_replacer;

                            }
                            else if(write_through){
                                vector<bool> dirty_replacer(blocks_per_set);
                                dirty_replacer[0]=false;
                                int ptrr1=1;
                                int ptrr2=0;
                                while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                            {
                                dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                ptrr1++;
                                ptrr2++;
                            }
                            dirtybits[index] = dirty_replacer;

                            }
                            vector<string> replacee(blocks_per_set);
                            replacee[0] = tag_bits;
                            int ptr1 = 1;
                            int ptr2 = 0;
                            // if(!write_through && dirtybits[index][blocks_per_set-1]==true){
                            //     dirty_on=true;
                            // }
                            while (ptr1 < blocks_per_set && ptr2 < blocks_per_set)
                            {
                                replacee[ptr1] = cache[index][ptr2];
                                ptr1++;
                                ptr2++;
                            }
                            cache[index] = replacee;

                            if (l == true)
                                {
                                    if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+(25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    // if (write_allocate && !write_through)
                                    // {
                                    //     total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    // }
                                    if(write_allocate && write_through){
                                        total_cycles+=(25 * (bytes_per_block)) + 1;
                                    }
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                }
                                else
                                {
                                    // store_misses++;
                                    if (write_allocate && write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block)) + 100+1;
                                    }
                                    else if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    else if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+ (25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    else if (!write_allocate)
                                    {
                                        total_cycles += 100; // confirm
                                    }
                                }
                        }
                        else
                        {
                            if(!write_through && dirtybits[index][0]==true){
                                dirty_on=true;
                            }
                            if(!write_through){
                                vector<bool> dirty_replacer(blocks_per_set);
                            for (int i = 1; i < blocks_per_set; i++)
                            {
                                dirty_replacer[i - 1] = dirtybits[index][i];
                            }
                            dirty_replacer[blocks_per_set - 1] = true;
                            dirtybits[index] = dirty_replacer;
                                // dirtybits[index] = dirty_replacer;
                            }
                            
                            vector<string> replacee(blocks_per_set);
                            for (int i = 1; i < blocks_per_set; i++)
                            {
                                replacee[i - 1] = cache[index][i];
                            }
                            replacee[blocks_per_set - 1] = tag_bits;
                            cache[index] = replacee;
                            // cache[index][0] = tag_bits;
                            if (l == true)
                                {
                                    // load_misses++;
                                    if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+(25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    // if (write_allocate && !write_through)
                                    // {
                                    //     total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    // }
                                    if(write_allocate && write_through){
                                        total_cycles+=(25 * (bytes_per_block)) + 1;
                                    }
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                    // total_cycles += (25 * (bytes_per_block)) + 1;
                                }
                                else
                                {
                                    // store_misses++;
                                    if (write_allocate && write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block)) + 100+1;
                                    }
                                    else if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    else if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+ (25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    else if (!write_allocate)
                                    {
                                        total_cycles += 100; // confirm
                                    }
                                }
                        }
                    }
                }
                else
                {
                    if (l == true)
                                {
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                }
                                else
                                {
                                    if (!write_allocate)
                                    {
                                        total_cycles += 100; // confirm
                                    }
                                }
                }
            }
            else
            {
                for (int i = 0; i < blocks_per_set; i++)
                {
                    if (cache[index][i] == "")
                    {
                        empty = true;
                        break;
                    }
                }
                if (empty)
                {
                    for (int i = 0; i < blocks_per_set; i++)
                    {
                        if (cache[index][i] == "")
                        {
                            if (lru)
                            {
                                if(!write_through && !l){
                                    vector<bool> dirty_replacer(blocks_per_set);
                                    int ptrr1 = 1;
                                    int ptrr2 = 0;
                                    dirty_replacer[0]=false;
                                    while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                                    {
                                            dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                            ptrr1++;
                                            ptrr2++;
                                    
                                    }
                                    dirtybits[index] = dirty_replacer;
                                    }
                                // if(write_through){
                                //     vector<bool> dirty_replacer(blocks_per_set);
                                //     int ptrr1 = 1;
                                //     int ptrr2 = 0;
                                //     dirty_replacer[0]=false;
                                //     while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                                //     {
                                //             dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                //             ptrr1++;
                                //             ptrr2++;
                                    
                                //     }
                                //     dirtybits[index] = dirty_replacer;
                                // }
                                vector<string> replacee(blocks_per_set);
                                replacee[0] = tag_bits;
                                int ptr1 = 1;
                                int ptr2 = 0;
                                while (ptr1 < blocks_per_set && ptr2 < blocks_per_set)
                                {
                                    replacee[ptr1] = cache[index][ptr2];
                                    ptr1++;
                                    ptr2++;
                                }
                                cache[index] = replacee;
                            }
                            else
                            {
                                cache[index][i] = tag_bits;
                            }
                            if (l == true)
                                {
                                    if (write_allocate && !write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    if(write_allocate && write_through){
                                        total_cycles+=(25 * (bytes_per_block)) + 1;
                                    }
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                }
                                else
                                {
                                    if (write_allocate && write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block)) + 100+1;
                                    }
                                    else if (write_allocate && !write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    else if (!write_allocate)
                                    {
                                        total_cycles += 100; // confirm
                                    }
                                }
                            break;
                        }
                    }
                }
                else
                {
                    // cout<<"here"<<endl;
                    bool dirty_on=false;
                    if (lru)
                    {
                        // cout<<"hello"<<endl;
                        if(!write_through && dirtybits[index][blocks_per_set-1]==true){
                            dirty_on=true;
                        }
                        vector<string> replacee(blocks_per_set);
                        // cout<<"dirty_on "<<dirty_on<<endl;
                        if(!write_through){
                                vector<bool> dirty_replacer(blocks_per_set);
                                dirty_replacer[0]=false;
                                int ptrr1=1;
                                int ptrr2=0;
                                while (ptrr1 < blocks_per_set && ptrr2 < blocks_per_set)
                            {
                                dirty_replacer[ptrr1] = dirtybits[index][ptrr2];
                                ptrr1++;
                                ptrr2++;
                            }
                            dirtybits[index] = dirty_replacer;

                            }
                        replacee[0] = tag_bits;
                        int ptr1 = 1;
                        int ptr2 = 0;
                        while (ptr1 < blocks_per_set && ptr2 < blocks_per_set)
                        {
                            replacee[ptr1] = cache[index][ptr2];
                            ptr1++;
                            ptr2++;
                        }
                        cache[index] = replacee;
                        if (l == true)
                                {
                                    if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+(25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    // if (write_allocate && !write_through)
                                    // {
                                    //     total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    // }
                                    if(write_allocate && write_through){
                                        total_cycles+=(25 * (bytes_per_block)) + 1;
                                    }
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                }
                                else
                                {
                                    if (write_allocate && write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block)) + 100+1;
                                    }
                                    else if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    else if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+ (25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    else if (!write_allocate)
                                    {
                                        total_cycles += 100; // confirm
                                    }
                                }
                    }
                    else
                    {
                        if(!write_through && dirtybits[index][0]==true){
                            dirty_on=true;
                        }
                        if(!write_through){
                            vector<bool> dirty_replacer(blocks_per_set);
                            // dirty_replacer[0]=false;
                            for (int i = 1; i < blocks_per_set; i++)
                            {
                                dirty_replacer[i - 1] = dirtybits[index][i];
                            }
                            dirty_replacer[blocks_per_set - 1] = false;
                            dirtybits[index] = dirty_replacer;
                            }
                        vector<string> replacee(blocks_per_set);
                        for (int i = 1; i < blocks_per_set; i++)
                        {
                            replacee[i - 1] = cache[index][i];
                        }
                        replacee[blocks_per_set - 1] = tag_bits;
                        cache[index] = replacee;
                        if (l == true)
                                {
                                    if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    }
                                    if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+(25 * (bytes_per_block));    //check for dirty bit
                                    }
                                    // if (write_allocate && !write_through)
                                    // {
                                    //     total_cycles += (25 * (bytes_per_block))+1;    //check for dirty bit
                                    // }
                                    if(write_allocate && write_through){
                                        total_cycles+=(25 * (bytes_per_block)) + 1;
                                    }
                                    if(!write_allocate){
                                        total_cycles += (25 * (bytes_per_block)) + 1;
                                    }
                                    
                                }
                                else
                                {
                                    
                                    if (write_allocate && write_through)
                                    {
                                        total_cycles += (25 * (bytes_per_block)) + 100+1;
                                    }
                                    else if (write_allocate && !write_through && !dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1;    
                                    }
                                    else if (write_allocate && !write_through && dirty_on)
                                    {
                                        total_cycles += (25 * (bytes_per_block))+1+ (25 * (bytes_per_block));   
                                    }
                                    else if (!write_allocate)
                                    {
                                        total_cycles += 100; 
                                    }
                                }
                    }
                }
            }
        
    }
    // cout<<"hit "<<hit<<endl;
    // cout<<"l "<<l<<endl;
    // cout<<"empty "<<empty<<endl;
    // cout<<"cycles "<<total_cycles<<endl;
    // for(int i=0;i<sets;i++){
    //         for(int j=0;j<blocks_per_set;j++){
    //             cout<<i<<" "<<j<<" "<<dirtybits[i][j]<<endl;
    //         }
    //     }
    //     for(int i=0;i<sets;i++){
    //         for(int j=0;j<blocks_per_set;j++){
    //             cout<<i<<" "<<j<<" "<<cache[i][j]<<endl;
    //         }
    //     }
}

#include <iostream>
#include <cstdlib> 

int main(int argc, char *argv[]) {
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0] << " <cache_size> <block_size> <associativity> <write_policy> <replace_policy> <trace_file>\n";
        return 1;
    }

    int sets = std::atoi(argv[1]);
    int blocks_per_set = std::atoi(argv[2]);
    int bytes_per_block = std::atoi(argv[3]);
    const string write_policy = argv[4];
    const string replace_policy = argv[5];
    const string lru=argv[6];
    const string trace_file = argv[7];
    bool write_alloc;
    if (write_policy=="write-allocate"){
        write_alloc=true;
    }
    else{
        write_alloc=false;
    }
    bool write_thru;
    if(replace_policy=="write-through"){
        write_thru=true;
    }
    else{
        write_thru=false;
    }
    bool lruu;
    if(lru=="lru"){
        lruu=true;
    }
    else{
        lruu=false;
    }
    std::ifstream file(trace_file);
    if (!file.is_open())
    {
        std::cerr << "Error opening file." << std::endl;
    }
    int hits = 0, misses = 0;
    int total_loads = 0, total_stores = 0, load_hits = 0, load_misses = 0, store_hits = 0, store_misses = 0, total_cycles = 0;
    std::string line;
    vector<vector<string>> cache(sets);
    for (int i = 0; i < sets; i++)
    {
        cache[i].resize(blocks_per_set);
    }
    vector<vector<bool>> dirtybits(sets);
    for (int i = 0; i < sets; i++)
    {
        dirtybits[i].resize(blocks_per_set);
    }
    int lines = 0;
    while (std::getline(file, line))
    {
        lines++;
        std::istringstream iss(line);
        char operation;
        std::string address;
        int size;
        if (!(iss >> operation >> address >> size))
        {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue;
        }
        bool l;
        if (operation == 'l')
        {
            l = true;
            total_loads++;
        }
        else
        {
            l = false;
            total_stores++;
        }
        CacheSim(dirtybits, hits, misses, cache, l, address, sets, blocks_per_set, bytes_per_block, write_alloc, write_thru, lruu, load_hits, load_misses, store_hits, store_misses, total_cycles);
    }

    std::cout << "Total Loads: " << total_loads << std::endl;
    std::cout << "Total stores: " << total_stores << std::endl;
    std::cout << "Load Hits: " << load_hits << std::endl;
    std::cout << "Load Misses: " << load_misses << std::endl;
    std::cout << "Store Hits: " << store_hits << std::endl;
    std::cout << "Store Misses: " << store_misses << std::endl;
    std::cout << "Total Cycles: " << total_cycles << std::endl;

    return 0;
    
    return 0;
}
