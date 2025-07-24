# Huffman Compressor & Decompressor

This project provides a simple command-line tool for compressing and decompressing files using Huffman coding in C++.  
It consists of two main functionalities:
- **Compression:** Encodes a file using Huffman coding to reduce its size.
- **Decompression:** Decodes a previously compressed file back to its original form.

## Usage

Compile the code using a C++ compiler (e.g., g++):

```sh
g++ -o huffman test.cpp
```

### Compress a file

```sh
huffman compress <input_file> <output_file>
```

- `<input_file>`: Path to the file you want to compress.
- `<output_file>`: Path where the compressed file will be saved.

### Decompress a file

```sh
huffman decompress <input_file> <output_file>
```

- `<input_file>`: Path to the compressed file.
- `<output_file>`: Path where the decompressed file will be saved.

## Example

```sh
huffman compress sample.txt sample.huff
huffman decompress sample.huff sample_decoded.txt
```

## Notes

- The program works with binary files.
- Make sure you have read/write permissions for the specified files.
- The code requires a C++11 compatible compiler or newer.

## Files

- `test.cpp`: Main source code containing both compressor and decompressor classes.

## License

This project is provided for educational purposes.