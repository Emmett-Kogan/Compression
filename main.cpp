#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include <unordered_map>
#include <vector>
#include <set>

#define INPUT_C "original.txt"
#define OUTPUT_C "cout.txt"
#define INPUT_D "compressed.txt"
#define OUTPUT_D "dout.txt"
#define DICTIONARY_SIZE 16

#define ERREXIT(s) { fprintf(stderr, s); exit((-1)); }

using std::unordered_map, std::vector, std::set;

void compress(FILE *fptr_in, FILE *fptr_out);
void decompress(FILE *fptr_in, FILE *fptr_out);
uint32_t btoi(const char *const str, uint32_t len);

// I think it would be more helpful to have compress and decompress
// act as their own programs pretty much. For compress, it makes sense to
// treat each line individually and rebuild from there
// but for decompress, it would probably be better to just make a parser that
// runs on a really long binary string
// so instead of parseing them in main, just opening the correct files and calling
// compress/decompress to actually parse the input file might be better
// this would also prevent me from needing global variables to do this


int main(int argc, char **argv) {
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

void compress(FILE *fp_in, FILE *fp_out) {
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
}

void decompress(FILE *fp_in, FILE *fp_out) {

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
