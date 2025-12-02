#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <chrono>

using namespace std;

// Параметри з розрахунків для 1 млн елементів і 1% помилки
const int N = 1000000;       // Очікувана кількість елементів
const int BLOOM_SIZE = 9600000; // Розмір масиву
const int K_HASHES = 7;      // Кількість хеш-функцій

class BloomFilter {
private:
    vector<unsigned short> bitArray; // Використовуємо лічильники замість bool

    // Хеш-функція 1 (поліноміальна з P=31)
    size_t hash1(const string& s) {
        unsigned long long hash = 0;
        for (char c : s) {
            hash = (hash * 31 + c);
        }
        return hash;
    }

    // Хеш-функція 2 (поліноміальна з P=37)
    size_t hash2(const string& s) {
        unsigned long long hash = 0;
        for (char c : s) {
            hash = (hash * 37 + c + 1);
        }
        return hash;
    }

public:
    BloomFilter() {
        bitArray.resize(BLOOM_SIZE, 0);
    }

    void add(const string& s) {
        size_t h1 = hash1(s);
        size_t h2 = hash2(s);

        for (int i = 0; i < K_HASHES; i++) {
            size_t index = (h1 + i * h2) % BLOOM_SIZE;
            if (bitArray[index] < 65535) {
                bitArray[index]++;
            }
        }
    }

    void remove(const string& s) {
        // Перевіряємо чи елемент взагалі може бути в фільтрі перед видаленням
        if (!contains(s)) return;

        size_t h1 = hash1(s);
        size_t h2 = hash2(s);

        for (int i = 0; i < K_HASHES; i++) {
            size_t index = (h1 + i * h2) % BLOOM_SIZE;
            if (bitArray[index] > 0) {
                bitArray[index]--;
            }
        }
    }

    // Перевірка наявності
    bool contains(const string& s) {
        size_t h1 = hash1(s);
        size_t h2 = hash2(s);

        for (int i = 0; i < K_HASHES; i++) {
            size_t index = (h1 + i * h2) % BLOOM_SIZE;
            if (bitArray[index] == 0) {
                return false;
            }
        }
        return true;
    }

    double loadFactor() {
        int setBits = 0;
        for (auto b : bitArray) {
            if (b > 0) setBits++;
        }
        return (double)setBits / BLOOM_SIZE;
    }
};

int main() {

    ifstream inputFile("test_data.txt");

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open test_data.txt. " << endl;
        return 1;
    }

    BloomFilter filter;
    char operation;
    string word;

    cout << "Bloom Filter initialized." << endl;
    cout << "Size: " << BLOOM_SIZE << " counters." << endl;
    cout << "Hash functions: " << K_HASHES << endl;
    cout << "Processing..." << endl;

    auto start = chrono::high_resolution_clock::now();

    while (inputFile >> operation && operation != '#') {
        inputFile >> word;

        switch (operation) {
        case '+':
            filter.add(word);
            break;
        case '-':
            filter.remove(word);
            break;
        case '?':
            if (filter.contains(word)) {
                cout << word << ": Y" << endl;
            }
            else {
                cout << word << ": ---------------" << endl;
            }
            break;
        default:
            break;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Processing completed in " << duration.count() << " ms." << endl;

    cout << "\n Manual Check" << endl;
    string tests[] = { "apple", "test", "bloom", "filter", "randomword123","toremove" };

    filter.add("bloom");
    filter.add("filter");
    string remWord = "toremove";
    filter.add(remWord);
    for (const string& t : tests) {
        cout << "Contains '" << t << "'? " << (filter.contains(t) ? "Yes (probably)" : "No") << endl;
    }

    cout << "\nRemoval Demonstration" << endl;
    

    
    cout << "Added '" << remWord << "'. Contains? " << (filter.contains(remWord) ? "Yes" : "No") << endl;

    filter.remove(remWord);
    cout << "Removed '" << remWord << "'. Contains? " << (filter.contains(remWord) ? "Yes" : "No") << endl;

    return 0;
}