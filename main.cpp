#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include <unordered_map>
#include <vector>
#include <set>
#include <string>

#include <iostream>

#define INPUT_C "original.txt"
#define OUTPUT_C "cout.txt"
#define INPUT_D "compressed.txt"
#define OUTPUT_D "dout.txt"
#define DICTIONARY_SIZE 16

#define ERREXIT(s) { fprintf(stderr, s); exit((-1)); }

using std::unordered_map, std::vector, std::set, std::string, std::stoi;
using namespace std;

void compress(FILE *const fptr_in, FILE *const fptr_out);
void decompress(FILE *const fptr_in, FILE *const fptr_out);
uint32_t btoi(const char *const str, uint32_t len);

// Note that this does not include the 3 bit format identifier, it's just the rest of the format
uint32_t FORMAT_LENGTH[8] = {32, 3, 13, 9, 9, 9, 14, 4};

int main(int argc, char **argv) 
{
    if (argc < 2)
        ERREXIT("Missing cmd line arg\n");

    // Open input/ouput files depending on mode
    FILE *fp_in, *fp_out; 
    int arg = atoi(argv[1]);
    switch(arg) {
    case 1: // Compress
        compress(fp_in = fopen(INPUT_C, "r"), fp_out = fopen(OUTPUT_C, "w+"));
        break;
    case 2: // Decompress
        decompress(fp_in = fopen(INPUT_D, "r"), fp_out = fopen(OUTPUT_D, "w+"));
        break;
    default:
        ERREXIT("Bad cmd line arg\n");
    }

    fclose(fp_in);
    fclose(fp_out);
    return 0;
}

void compress(FILE *const fp_in, FILE *const fp_out) 
{
    vector<uint32_t> lines;
    unordered_map<uint32_t, uint32_t> counts;

    // Get each line of the input file
    char buffer[34];
    while(fgets(buffer, 34, fp_in)) {
        uint32_t tmp = btoi(buffer, 32);
        lines.push_back(tmp);
        if (counts.find(tmp) != counts.end())
            counts[tmp]++;
        else
            counts[tmp] =1;
    }
    
    // Make dictionary
    uint32_t dictionary[16];

    // For each entry in the dictionary
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        // Get set of lines with max count
        uint32_t max_count = 0;
        set<uint32_t> max_lines;
        for (auto count : counts) {
            if (count.second > max_count)
                max_lines.clear();
            if (count.second >= max_count) {
                max_count = count.second;
                max_lines.insert(count.first);
            }
        }

        // If there is a tie, select whichever occurs first
        if (max_lines.size() > 1) {
            // Find the line that occurs first
            int min_index = lines.size();
            for (auto line : max_lines)
                for (int j = 0; j < min_index; j++)
                    if (lines[j] == line && j < min_index)
                        min_index = j; 

            // Add that line to the dictionary and remove entry from hash map
            dictionary[i] = lines[min_index];
            counts.erase(lines[min_index]);
        } else {
            // Bind value to dictionary and remove entry from hash map
            dictionary[i] = *max_lines.begin();
            counts.erase(*max_lines.begin());
            
        }
    }

    // Now the actual compression
    for (auto line : lines) {

    }
    // What this will probably look like:
        // In order of which one compresses most
            // Check if we can compress like this
                // If we can, compress using that method
                // note for RLE I might have to mess with which line im on, and count ahead
        // Add compressed text to large string
        // repeat until out of lines

        // When checking for the bit differences, may be best to search through dictionary
        // xor the dictionary values with the line being compressed and count bits that are high
        // select the compression with the minimum overhead that is compatible


    // For now I think I want this to just append to a large string like in decompress
    // And then later I can write that to a file so that I have newlines all in the correct places

    // Something like this should work, where bstr is the big string
    // string bstr;
    // for (unsigned long i = 0; i < bstr.length(); i++) {
    //     if (i % 32 == 0 && i)
    //         fputc('\n', fp_out);
    //     fputc(bstr[i], fp_out);
    // }
    // fputs("xxxx\n", fp_out);
    // for (int i = 0; i < 16; i++)
    //     fprintf(fp_out, "%.032b\n", dictionary[i]);

}

void decompress(FILE *const fp_in, FILE *const fp_out) 
{
    string bstr;
    uint32_t dictionary[16];

    // Read just compressed text data into one string
    char buffer[34];
    while (fgets(buffer, 34, fp_in)) {
        #ifdef DEBUG
        printf("%s", buffer);
        #endif
        
        if (!strncmp(buffer, "xxxx", 4))
            break;
        for (int i = 0; i < 34; i++)
            if (buffer[i] == '0' || buffer[i] == '1')
                bstr += buffer[i];
    }

    // Load dictionary
    for (int i = 0; i < 16; i++) {
        fgets(buffer, 34, fp_in);
        dictionary[i] = btoi(buffer, 32);

        #ifdef DEBUG
        printf("%.032b\n", dictionary[i]);
        #endif
    }

    if (fgets(buffer, 34, fp_in))
        ERREXIT("Should have reached EOF but didn't");

    // Note that when there are offsets for bitshifting, technically underflow is possible
    // But since the values that would cause underflow don't make sense, e.g. in 4 bit mismatch case
    // it doesn't make sense for the start bit to be the LSB, I'm not going to boundscheck

    // Then parse the bigass string and decompress
    uint32_t prev_instruction = 0, offset = 0, length = bstr.length();
    while (offset < length) {

        #ifdef DEBUG
        printf("Offset:   %d\n", offset);
        #endif

        // Get 3 bits starting at index
        uint8_t format = stoi(bstr.substr(offset, 3), 0, 2), print_flag = 1;
        offset += 3; // move index to start of rest of encoding

        #ifdef DEBUG
        printf("Format:   %d\n", format);
        printf("Encoding: %s\n", bstr.substr(offset,FORMAT_LENGTH[format]).c_str());
        #endif

        switch(format) {
        case 0:
            if (length - offset < 32) {
                print_flag = 0;
                break;
            }

            prev_instruction = stoul(bstr.substr(offset,32).c_str(), 0, 2);
            break;
        case 1:
        {
            // next three bits as an int, +1, and print prev_instruction that many times
            int repeat = stoi(bstr.substr(offset, 3).c_str(), 0, 2)+1;
            printf("Repeat: %d\n", repeat);
            for (int i = 0; i < repeat; i++)
                fprintf(fp_out, "%.032b\n", prev_instruction);
            print_flag = 0;
            break;
        }
        case 2:
        {
            uint32_t start = stoi(bstr.substr(offset, 5).c_str(), 0, 2);
            uint32_t bm = stoi(bstr.substr(offset+5, 4).c_str(), 0, 2);
            uint32_t dict_val = dictionary[stoi(bstr.substr(offset+9,4).c_str(), 0, 2)];
            prev_instruction = dict_val^(bm<<(32-start-4));
            break;
        }
        case 3:
        case 4:
        case 5:
        {
            // Nested ternary making sense lol
            uint32_t bm = (format == 3 ? 0b1 : (format == 4 ? 0b11 : 0b1111));
            uint32_t width = (format == 3 ? 1 : (format == 4 ? 2 : 4));
            uint32_t start = stoi(bstr.substr(offset, 5).c_str(), 0, 2);
            uint32_t dict_val = dictionary[stoi(bstr.substr(offset+5, 4).c_str(), 0, 2)];
            prev_instruction = dict_val^(bm<<(32-start-width));
            break;
        }
        case 6:
        {
            uint32_t ml1 = stoi(bstr.substr(offset, 5).c_str(), 0, 2);
            uint32_t ml2 = stoi(bstr.substr(offset+5, 5).c_str(), 0, 2);
            uint32_t dict_val = dictionary[stoi(bstr.substr(offset+10,4).c_str(), 0, 2)];
            prev_instruction = dict_val^(0b1<<(32-ml1-1));
            prev_instruction ^= (0b1<<(32-ml2-1));
            break;
        }
        case 7:
            prev_instruction = dictionary[stoi(bstr.substr(offset,4).c_str(), 0, 2)];
            break;
        default:
            ERREXIT("Something went very very wrong");
        }

        if (print_flag)
            fprintf(fp_out, "%.032b\n", prev_instruction);

        offset += FORMAT_LENGTH[format];
    }
}

uint32_t btoi(const char *const str, uint32_t len) {
    uint32_t i = 0;
    uint32_t acc = 0;
    while (isdigit(str[i]) && i < len) {
        acc += (str[i] == '1' ? 1<<(31-i) : 0);
        i++;
    }

    return acc;
}
