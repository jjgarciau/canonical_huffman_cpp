# Canonical Huffman Coding (C++17)

**Algorithms and Data Structures — Practice II**

> Complete implementation of *canonical Huffman*: tree construction, code-length extraction, derivation of canonical codes, compression, compression ratio, and the required visualizations (frequencies, lengths, code tables, tree, compressed output).

## Author
- Juan José García Urrego

## How to Build and Run

- Open the project in CLion with C++17 enabled.
- Navigate to `huffman_canonical_cpp/src/main.cpp`.
- Click **Run**.
- Type any text whose length is **30 characters or more**.

```bash
echo "this is an example text with more than thirty characters!!!" | ./canonical_huffman
```

## Example Output (partial)

```
=== Canonical Huffman Coding ===

Input length: 60 chars
Original size: 480 bits
Compressed size: 312 bits
Ratio: 0.6500   |   Reduction: 35.0000%

Frequency Table (sorted by symbol)
Symbol | ASCII | Freq
----------------------
  ' '      32    9
  'a'      97    12
  'e'     101    11
  'n'     110    6
...
```

It also includes:
- Code-length table per symbol
- **Non-canonical** (tree) and **canonical** code tables
- Textual tree representation (preorder with parentheses)
- First 128 bits of the compressed output and the total number of bits
- Hexadecimal string for the compressed data (byte-padded)

## Design and Decisions

- **Data structures:** `priority_queue` with a deterministic comparator `(ascending frequency, ascending min symbol in subtree)` for reproducibility.
- **Code lengths:** DFS over the tree to record each leaf’s (symbol’s) depth. Single-symbol edge case: assign length 1.
- **Canonical codes:** sort by `(length asc, symbol asc)`; the first code for a given length `L` is `L` zero bits. For the next symbol, increment the integer; when the length increases, left-shift to extend with zeros.
- **Compression:** direct bit concatenation following the original sequence.
- **Visualization:** tables sorted by symbol for consistent reading.

## Test Cases

`tests/cases.txt` contains 3 lines (one case per line):  
1) **Many spaces**  
2) **Few distinct symbols**  
3) **Approximately uniform distribution**

## Requirements Covered

- Construction of the (non-canonical) Huffman tree.
- Extraction of code lengths and derivation of **canonical codes**.
- Compression using canonical codes.
- Compression ratio and percentage reduction.
- Minimal visualizations: tables (frequencies, lengths, codes), textual tree, first 128 bits and total size.
- Portability: compiles with `g++ -std=c++17` without external dependencies.
- Error handling: empty input, < 30 characters, or outside ASCII 32–126.

## Defense Video

url

## Organized Output
- ASCII tables aligned for frequencies, lengths, and codes.
- ASCII tree with indentation and 0/1 edge labels.
- Bits grouped by 8 and wrapped every 64 bits.
- Hex grouped by bytes, 16 bytes per line.
