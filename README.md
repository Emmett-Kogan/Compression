# Bitmask-based Code Compression
For the sake of the professor, I will public my solution after it is overdue and no longer accepted. Overall this was pretty fun to do, and reading his [paper](https://ieeexplore.ieee.org/document/4475257) on bitmask code compression was pretty cool.

## Contributors
1. Emmett Kogan

## Comments
The way the project specifies how to handle the RLE case where say, the next 28 instructions are the same is bad. The document specifies that you should do a format 7 dictionary lookup, followed by a format 1 RLE encoding (for the max 8 length), and repeat that until all 28 have been encoded. So 1+8 + 1+8+ 1+8 + 1. This is worse than what you can just do with RLE followed by RLE. The way I made my decompression, I originally was able to do 1+8+8+8+3 with one dictionary lookup, followed by 3 RLEs that all are the same. This takes up 7+4\*6 (31) bits, whereas the assignment's specifcation would use 4\*7+3*6 (46) bits. If I wanted to implement this the better way, I would just remove the logic in the format 2 handling that checks for if the previous format was also a format 2 instruction, and prevents that.

I didn't make another test case to test absolutley everything (because it would take longer than it did to actually complete the project). But since the only thing I didn't test was one of the {3, 4, 5} format instructions, but since I implemented them all so that they all run with the same cases/code that *should* be fine.
