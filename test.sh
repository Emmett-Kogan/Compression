make

# Testing compression
timeout 10s ./SIM 1
if [[ $? -eq 0 ]]; then
    diff -w -B cout.txt compressed.txt
    if [[ $? -ne 0 ]]; then
        echo "Compression output does not match given output"
    fi
fi

# Testing decompression
timeout 10s ./SIM 2
if [[ $? -eq 0 ]]; then
    diff -w -B dout.txt original.txt
    if [[ $? -ne 0 ]]; then
        echo "Decompression output does not match given output"
    fi
fi

make clean
