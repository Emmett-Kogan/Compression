# Bitmask-based Code Compression
For the sake of the proffessor, I will public my solution after it is overdue and no longer accepted. But this seems like it would be fun to do in C, and then do again in Rust when I go to learn that next week.

## Contributors
1. Emmett Kogan

## Useful snippets for later
```
    // Get each line of the input file
    char buffer[34];
    while(fgets(buffer, 34, fptr_in)) {
        // If "xxxx", then need to load dictionary feon file
        if (!strncmp(buffer, "xxxx", 4)) {
            load_dictionary();
            // We also no longer need to read lines
            break;
        }
        
        uint32_t tmp = btoi(buffer, 32);
        printf("%032b\n", tmp);
        lines.push_back(tmp);
        if(counts.find(tmp) != counts.end())
            counts[tmp]++;
        else
            counts[tmp] = 1;
    }

    // If compressing then need to initialize the dictionary
    if (arg == 1) {
        write_dictionary();
    }


        for (int i = 0; i < 8; i++)
        printf("%03b\n", i);

```
