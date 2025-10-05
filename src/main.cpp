#include <bits/stdc++.h>
using namespace std;

/*
 * Canonical Huffman Coding in C++ (C++17) — Pretty Console Output
 *
 * Changes vs. previous version:
 *  - Uniform ASCII tables with headers and aligned columns
 *  - Readable, indented ASCII tree with edge labels (0/1)
 *  - Grouped bitstream (binary grouped by bytes, wrapped by line)
 *  - Grouped hex output (bytes grouped, 16 bytes per line)
 */

struct Node {
    long long freq;
    bool is_leaf;
    unsigned char symbol;     // valid only if is_leaf
    int min_symbol;           // min ASCII code in subtree (for deterministic ties)
    Node *left, *right;
    Node(long long f, unsigned char s)
        : freq(f), is_leaf(true), symbol(s), min_symbol((int)s), left(nullptr), right(nullptr) {}
    Node(Node* L, Node* R)
        : freq(L->freq + R->freq), is_leaf(false), symbol(0), left(L), right(R) {
            min_symbol = min(L->min_symbol, R->min_symbol);
        }
};

struct CmpPtr {
    bool operator()(const Node* a, const Node* b) const {
        if (a->freq != b->freq) return a->freq > b->freq; // min-heap by freq
        return a->min_symbol > b->min_symbol;             // tie-break by min symbol
    }
};

static string disp_symbol(unsigned char c) {
    if (c == ' ') return "' '";
    return string("'") + (char)c + "'";
}

/*** ---------- Generic table printer (ASCII) ---------- ***/
static void print_table(const vector<string>& header,
                        const vector<vector<string>>& rows,
                        const vector<bool>& right_align) {
    size_t cols = header.size();
    vector<size_t> w(cols, 0);
    for (size_t i = 0; i < cols; ++i) w[i] = header[i].size();
    for (const auto& r : rows) {
        for (size_t i = 0; i < cols; ++i) w[i] = max(w[i], r[i].size());
    }
    auto print_sep = [&]() {
        cout << "+";
        for (size_t i = 0; i < cols; ++i) {
            cout << string(w[i] + 2, '-') << "+";
        }
        cout << "\n";
    };
    auto print_row = [&](const vector<string>& r, bool is_header=false) {
        cout << "|";
        for (size_t i = 0; i < cols; ++i) {
            cout << " ";
            if (right_align[i] && !is_header) {
                cout << setw((int)w[i]) << r[i];
            } else {
                cout << left << setw((int)w[i]) << r[i] << right;
            }
            cout << " |";
        }
        cout << "\n";
    };
    print_sep();
    print_row(header, true);
    print_sep();
    for (const auto& r : rows) print_row(r);
    print_sep();
}

/*** ---------- DFS to get non-canonical codes and lengths ---------- ***/
static void dfs_codes(Node* cur, string path,
               unordered_map<unsigned char,string>& noncanon,
               unordered_map<unsigned char,int>& lengths) {
    if (!cur) return;
    if (cur->is_leaf) {
        if (path.empty()) path = "0"; // single symbol case
        noncanon[cur->symbol] = path;
        lengths[cur->symbol] = (int)path.size();
        return;
    }
    dfs_codes(cur->left,  path + "0", noncanon, lengths);
    dfs_codes(cur->right, path + "1", noncanon, lengths);
}

/*** ---------- Pretty ASCII tree with 0/1 edge labels ---------- ***/
static void print_tree_pretty(Node* cur, const string& prefix="", bool is_last=true, const string& edge="") {
    if (!cur) return;
    cout << prefix;
    if (!prefix.empty()) {
        cout << (is_last ? "`- " : "|- ");
        if (!edge.empty()) cout << edge << "-> ";
    }
    if (cur->is_leaf) {
        cout << "[leaf " << disp_symbol(cur->symbol) << " : " << cur->freq << "]\n";
    } else {
        cout << "(node f=" << cur->freq << ")\n";
        string next_prefix = prefix + (is_last ? "   " : "|  ");
        // left (0), right (1)
        if (cur->left || cur->right) {
            if (cur->left && cur->right) {
                print_tree_pretty(cur->left,  next_prefix, false, "0");
                print_tree_pretty(cur->right, next_prefix, true,  "1");
            } else if (cur->left) {
                print_tree_pretty(cur->left,  next_prefix, true, "0");
            } else if (cur->right) {
                print_tree_pretty(cur->right, next_prefix, true, "1");
            }
        }
    }
}

/*** ---------- Canonical codes from lengths ---------- ***/
static map<unsigned char,string> build_canonical(const unordered_map<unsigned char,int>& lengths) {
    vector<pair<unsigned char,int>> items;
    items.reserve(lengths.size());
    for (auto &kv : lengths) items.emplace_back(kv.first, kv.second);
    sort(items.begin(), items.end(), [](auto& a, auto& b){
        if (a.second != b.second) return a.second < b.second; // length asc
        return a.first < b.first;                              // symbol asc
    });
    map<unsigned char,string> canon;
    if (items.empty()) return canon;
    unsigned long long code = 0;
    int curr_len = items[0].second;
    auto to_bits = [](unsigned long long v, int L){
        string s(L, '0');
        for (int i = L-1; i >= 0; --i) {
            s[i] = (v & 1ULL) ? '1' : '0';
            v >>= 1ULL;
        }
        return s;
    };
    for (size_t i = 0; i < items.size(); ++i) {
        auto [sym, L] = items[i];
        if (L > curr_len) {
            code <<= (L - curr_len);
            curr_len = L;
        }
        canon[sym] = to_bits(code, L);
        code += 1ULL;
    }
    return canon;
}

/*** ---------- Bits/Hex grouping ---------- ***/
static string group_bits(const string& bits, int group = 8, int line_group = 64) {
    string out;
    int count_in_line = 0;
    for (size_t i = 0; i < bits.size(); ++i) {
        out.push_back(bits[i]);
        if ((int)((i+1) % group) == 0 && (i+1) != bits.size()) {
            out.push_back(' ');
        }
        count_in_line++;
        if (count_in_line == line_group) {
            out.push_back('\n');
            count_in_line = 0;
        }
    }
    return out;
}

static string bits_to_hex(const string& bits) {
    string padded = bits;
    size_t rem = padded.size() % 8;
    if (rem != 0) padded.append(8 - rem, '0');
    string hex;
    static const char* HEX = "0123456789ABCDEF";
    for (size_t i = 0; i < padded.size(); i += 8) {
        unsigned int byte = 0;
        for (int b = 0; b < 8; ++b) {
            byte = (byte << 1) | (padded[i+b] == '1' ? 1 : 0);
        }
        hex.push_back(HEX[(byte >> 4) & 0xF]);
        hex.push_back(HEX[byte & 0xF]);
    }
    return hex;
}

static string group_hex(const string& hex, int bytes_per_line = 16) {
    string out;
    int bytes_in_line = 0;
    for (size_t i = 0; i < hex.size(); i += 2) {
        if (i + 1 < hex.size()) {
            out.push_back(hex[i]);
            out.push_back(hex[i+1]);
        } else {
            out.push_back(hex[i]);
        }
        bytes_in_line++;
        if (i + 2 < hex.size()) out.push_back(' ');
        if (bytes_in_line == bytes_per_line) {
            out.push_back('\n');
            bytes_in_line = 0;
        }
    }
    return out;
}

int main() {
    ios::sync_with_stdio(false);

    cout << "Escriba la frase que desea procesar: " << endl;

    cin.tie(nullptr);

    string input;
    if (!std::getline(cin, input)) {
        cout << "Error: no input provided (expected a single line with >= 30 ASCII 32-126 chars)\n";
        return 1;
    }

    // Validate input
    if ((int)input.size() < 30) {
        cout << "Error: input must have at least 30 characters.\n";
        return 1;
    }

    for (unsigned char c : input) {
        if (c < 32 || c > 126) {
            cout << "Error: only ASCII 32-126 supported. Invalid char detected.\n";
            return 1;
        }
    }

    // Frequencies
    unordered_map<unsigned char, long long> freq;
    for (unsigned char c : input) freq[c]++;

    // Priority queue build (min-heap)
    priority_queue<Node*, vector<Node*>, CmpPtr> pq;
    vector<unique_ptr<Node>> arena; // manage memory
    arena.reserve(freq.size() * 2 + 5);
    for (auto &kv : freq) {
        arena.emplace_back(new Node(kv.second, kv.first));
        pq.push(arena.back().get());
    }

    if (pq.empty()) {
        cout << "Error: empty input after filtering.\n";
        return 1;
    }
    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        arena.emplace_back(new Node(a, b));
        pq.push(arena.back().get());
    }
    Node* root = pq.top();

    // Non-canonical codes and lengths
    unordered_map<unsigned char,string> noncanon;
    unordered_map<unsigned char,int> lengths;
    dfs_codes(root, "", noncanon, lengths);

    // Canonical codes
    auto canonical = build_canonical(lengths);

    // Encode
    string encoded_bits;
    encoded_bits.reserve(input.size() * 8);
    long long compressed_bits = 0;
    for (unsigned char c : input) {
        const string& code = canonical[(unsigned char)c];
        encoded_bits += code;
        compressed_bits += (long long)code.size();
    }

    long long original_bits = (long long)input.size() * 8LL;
    double ratio = (original_bits == 0) ? 0.0 : (double)compressed_bits / (double)original_bits;
    double reduction = (1.0 - ratio) * 100.0;

    // Sorted orders
    vector<unsigned char> symbols;
    symbols.reserve(freq.size());
    for (auto &kv : freq) symbols.push_back(kv.first);
    sort(symbols.begin(), symbols.end()); // by symbol asc

    // ---- Output ----
    cout.setf(std::ios::fixed);
    cout << setprecision(4);
    cout << "\n=============================================\n";
    cout << "        Canonical Huffman Coding (C++)       \n";
    cout << "=============================================\n\n";

    // Summary
    vector<string> h1 = {"Metric", "Value"};
    vector<vector<string>> r1 = {
        {"Input length (chars)", to_string(input.size())},
        {"Original size (bits)", to_string(original_bits)},
        {"Compressed size (bits)", to_string(compressed_bits)},
        {"Ratio (compressed/original)", to_string(ratio)},
        {"Reduction (%)", to_string(reduction)}
    };
    print_table(h1, r1, {false, true});
    cout << "\n";

    // Frequency table
    vector<string> h2 = {"Symbol", "ASCII", "Freq"};
    vector<vector<string>> r2;
    for (unsigned char s : symbols) {
        r2.push_back({disp_symbol(s), to_string((int)s), to_string(freq[s])});
    }
    cout << "Frequencies (sorted by symbol)\n";
    print_table(h2, r2, {false, true, true});
    cout << "\n";

    // Lengths table
    vector<string> h3 = {"Symbol", "Length"};
    vector<vector<string>> r3;
    for (unsigned char s : symbols) {
        r3.push_back({disp_symbol(s), to_string(lengths[s])});
    }
    cout << "Code Lengths per Symbol\n";
    print_table(h3, r3, {false, true});
    cout << "\n";

    // Codes table
    vector<string> h4 = {"Symbol", "Freq", "Length", "Non-Canonical (Tree)", "Canonical"};
    vector<vector<string>> r4;
    for (unsigned char s : symbols) {
        r4.push_back({disp_symbol(s), to_string(freq[s]), to_string(lengths[s]), noncanon[s], canonical[s]});
    }
    cout << "Codes Table\n";
    print_table(h4, r4, {false, true, true, false, false});
    cout << "\n";

    // Pretty Tree
    cout << "Huffman Tree (indented, edges labeled 0/1)\n";
    cout << "[root]\n";
    print_tree_pretty(root, "", true, "");
    cout << "\n";

    // Bits
    string first128 = encoded_bits.substr(0, min<size_t>(128, encoded_bits.size()));
    cout << "Compressed (first up to 128 bits, showing " << first128.size() << "):\n";
    cout << group_bits(first128, 8, 64) << "\n";
    cout << "Total compressed bits: " << compressed_bits << "\n\n";

    string hex_full = bits_to_hex(encoded_bits);
    cout << "Compressed (hex, padded to bytes; 16 bytes per line):\n";
    cout << group_hex(hex_full, 16) << "\n";

    return 0;
}
