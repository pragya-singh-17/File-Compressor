#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <fstream>
using namespace std;

struct HuffmanNode{
    char data;
    unsigned freq;
    HuffmanNode *left, *right;

    HuffmanNode(char data, int freq): data(data), freq(freq), left(nullptr), right(nullptr) {}
    HuffmanNode(char data, int freq, HuffmanNode* left, HuffmanNode* right): data(data), freq(freq), left(left), right(right) {}

    ~HuffmanNode(){
        delete left;
        delete right;
    }
};

struct CompareNodes{
    bool operator()(HuffmanNode* l, HuffmanNode* r){
        return l->freq > r->freq;
    }
};

class HuffmanCompressor {
private:
    string inputFilePath;
    string outputFilePath;
    HuffmanNode* root;
    map<char, string> huffmanCodes;

    void buildHuffmanTree(map<char, int>& frequency) {
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> minHeap;

        for (map<char, int>::const_iterator it = frequency.begin(); it != frequency.end(); ++it) {
            minHeap.push(new HuffmanNode(it->first, it->second));
        }

        if (minHeap.empty()) {
            cout << "No characters to encode." << endl;
            return;
        }

        while (minHeap.size() > 1) {
            HuffmanNode *left = minHeap.top();
            minHeap.pop();
            HuffmanNode *right = minHeap.top();
            minHeap.pop();

            HuffmanNode *top = new HuffmanNode('\0', left->freq + right->freq);
            top->left = left;
            top->right = right;

            minHeap.push(top);
        }

        root = minHeap.top();
        // The root of the Huffman tree is now available in 'root'
    }

    void generateHuffmanCodes(HuffmanNode* node, const string &str, map<char, string>& huffmanCodes) {
        if (!node) return;

        if (node->data != '\0') {
            huffmanCodes[node->data] = str;
        }

        generateHuffmanCodes(node->left, str + "0", huffmanCodes);
        generateHuffmanCodes(node->right, str + "1", huffmanCodes);
    }

    void writeHuffmanCodesToFile(ostream& out, HuffmanNode* node) {

        if(!node) return;
        if(node->data != '\0') {
            out.put('1');
            out.put(node->data);
        }
        else {
            out.put('0');
            writeHuffmanCodesToFile(out, node->left);
            writeHuffmanCodesToFile(out, node->right);
        }
    }

public:
    HuffmanCompressor(const string& input, const string& output): inputFilePath(input), outputFilePath(output), root(nullptr) {}

    ~HuffmanCompressor() {
        delete root;
    }

    void compress() {
        ifstream inputFile(inputFilePath , ios::binary);
        if(!inputFile.is_open()) {
            cerr << "Error opening input file: " << inputFilePath << endl;
            return;
        }
        map<char, int> frequency;
        char ch;

        while (inputFile.get(ch)) {
            frequency[ch]++;
        }
        inputFile.close();

        buildHuffmanTree(frequency);
        generateHuffmanCodes(root, "", huffmanCodes);

        ofstream outputFile(outputFilePath, ios::binary);
        if(!outputFile.is_open()) {
            cerr << "Error opening output file: " << outputFilePath << endl;
            return;
        }

        long long totalChars = 0;
        for (map<char, int>::const_iterator it = frequency.begin(); it != frequency.end(); ++it) {
            totalChars += it->second;
        }
        outputFile.write(reinterpret_cast<const char*>(&totalChars), sizeof(totalChars));
        writeHuffmanCodesToFile(outputFile, root);

        inputFile.open(inputFilePath, ios::binary);
        string encodedString = "";
        char character;
        while (inputFile.get(character)) {
            encodedString += huffmanCodes[character];
        }

        // Convert string of '0's and '1's to actual bits and write to file
        char buffer = 0;
        int count = 0;
        for (char bit : encodedString) {
            buffer = buffer << 1;
            if (bit == '1') {
                buffer = buffer | 1;
            }
            count++;
            if (count == 8) {
                outputFile.put(buffer);
                buffer = 0;
                count = 0;
            }
        }
        if (count > 0) {
            buffer = buffer << (8 - count);
            outputFile.put(buffer);
        }

        inputFile.close();
        outputFile.close();
        cout << "File compressed successfully!" << std::endl;
    }
};

class HuffmanDecompressor {
private:
    string inputFilePath;
    string outputFilePath;
    HuffmanNode* root;

    HuffmanNode* readTree(istream& in) {
        char bit;
        in.get(bit);
        if (bit == '1') {
            char data;
            in.get(data);
            return new HuffmanNode(data, 0); // Frequency doesn't matter for decompression
        } else {
            HuffmanNode* left = readTree(in);
            HuffmanNode* right = readTree(in);
            return new HuffmanNode('\0', 0, left, right);
        }
    }

public:
    HuffmanDecompressor(const string& input, const string& output): inputFilePath(input), outputFilePath(output), root(nullptr) {}

    ~HuffmanDecompressor() {
        delete root;
    }

    void decompress() {
        ifstream inputFile(inputFilePath, ios::binary);
        if (!inputFile) {
            cerr << "Error: Cannot open input file!" << endl;
            return;
        }

        long long totalChars;
        inputFile.read(reinterpret_cast<char*>(&totalChars), sizeof(totalChars));

        root = readTree(inputFile);

        ofstream outputFile(outputFilePath, ios::binary);
        if (!outputFile) {
            cerr << "Error: Cannot open output file!" << std::endl;
            return;
        }

        HuffmanNode* currentNode = root;
        char byte;
        long long decodedChars = 0;
        while (inputFile.get(byte) && decodedChars < totalChars) {
            for (int i = 7; i >= 0; --i) {
                if(decodedChars >= totalChars) break;

                // Get the i-th bit of the byte
                bool bit = (byte >> i) & 1;

                if (bit) { // bit is 1, go right
                    currentNode = currentNode->right;
                } else { // bit is 0, go left
                    currentNode = currentNode->left;
                }

                // If it's a leaf node, we found a character
                if (currentNode->left == nullptr && currentNode->right == nullptr) {
                    outputFile.put(currentNode->data);
                    decodedChars++;
                    currentNode = root; // Go back to the root for the next character
                }
            }
        }

        inputFile.close();
        outputFile.close();
        cout << "File decompressed successfully!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << endl;
        cerr << "  For compression: " << argv[0] << " compress <input_file> <output_file>" << endl;
        cerr << "  For decompression: " << argv[0] << " decompress <input_file> <output_file>" << endl;
        return 1;
    }

    string mode = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

    if (mode == "compress") {
        HuffmanCompressor compressor(inputFile, outputFile);
        compressor.compress();
    } else if (mode == "decompress") {
        HuffmanDecompressor decompressor(inputFile, outputFile);
        decompressor.decompress();
    } else {
        cerr << "Error: Invalid mode. Use 'compress' or 'decompress'." << endl;
        return 1;
    }

    return 0;
}