#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <string>
#include <cstring>
#include <windows.h>
#include <shellapi.h> // Include for ShellExecute

using namespace std;

class Document {
public:
    int id;
    string filePath;
    string content;

    Document(int _id, const string& _filePath, const string& _content) : id(_id), filePath(_filePath), content(_content) {}
};

class SearchEngine {
public:
    unordered_map<string, vector<pair<int, int>>> invertedIndex;
    vector<Document> documents;
    vector<string> searchHistory;

    void addDocument(const string& content, const string& filePath) {
        int docId = documents.size();
        documents.emplace_back(docId, filePath, content);

        // Tokenize and build inverted index
        stringstream ss(content);
        string token;
        int index = 0;
        while (ss >> token) {
            transform(token.begin(), token.end(), token.begin(), ::tolower);
            invertedIndex[token].push_back({docId, index});
            index++;
        }
    }

    void loadDocumentsFromFile(const string& fileName) {
        string extension = fileName.substr(fileName.find_last_of(".") + 1);
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        if (extension == "docx") {
            // Use Python script to extract text from Word file without opening a new window
            string command = "pythonw D:/kmdev/Documents/extract_text_from_docx.py " + fileName;
            system(command.c_str());

            // Wait for the Python script to finish before proceeding
            Sleep(2000); // Adjust the delay as needed

            // Read the extracted text
            ifstream file("output.txt");
            if (file.is_open()) {
                stringstream buffer;
                buffer << file.rdbuf();
                addDocument(buffer.str(), fileName);
                file.close();
            } else {
                cerr << "Error opening file: output.txt" << endl;
            }
        } else {
            ifstream file(fileName);
            if (file.is_open()) {
                stringstream buffer;
                buffer << file.rdbuf();
                addDocument(buffer.str(), fileName);
                file.close();
            } else {
                cerr << "Error opening file: " << fileName << endl;
            }
        }
    }

    void loadFilePathsFromConfig() {
        ifstream configFile("config.txt");
        if (configFile.is_open()) {
            string fileName;
            while (getline(configFile, fileName)) {
                loadDocumentsFromFile(fileName);
            }
            configFile.close();
        } else {
            cerr << "Error opening config file: config.txt" << endl;
        }
    }

    void displaySearchHistory() {
        cout << "Search History:" << endl;
        for (const string& query : searchHistory) {
            cout << query << endl;
        }
    }

    void displayDocumentNames() {
        cout << "Document Names:" << endl;
        for (const Document& doc : documents) {
            cout << "Document " << doc.id << ": " << getFileName(doc.filePath) << endl;
        }
    }

    void performSearch() {
        string query;
        cout << "Enter search query: ";
        getline(cin, query);

        // Display search history
        searchHistory.push_back(query);

        // Get relevant documents
        vector<int> results = search(query);
        if (results.empty()) {
            cout << "No references related to the search" << endl;
            return;
        }

        // Calculate BM25 scores for relevant documents
        vector<pair<int, double>> bm25Scores;
        for (int docId : results) {
            double score = calculateBM25Score(query, docId);
            bm25Scores.push_back({docId, score});
        }

        // Sort scores in descending order
        sort(bm25Scores.begin(), bm25Scores.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

        // Display the content of the top-ranked document
        int topRankedDocId = bm25Scores.front().first;
        cout << "Top Ranked Document:" << endl;
        cout << "Document " << topRankedDocId << ": " << getFileName(documents[topRankedDocId].filePath) << endl;
        cout << "Content related to the search query:" << endl;
        cout << documents[topRankedDocId].content << endl;
    }

    void browseInBrowser(const string& query) {
        // Add query to search history
        searchHistory.push_back(query);

        // You can implement browsing functionality here
        string searchUrl = "https://www.google.com/search?q=" + query;
        ShellExecuteA(NULL, "open", searchUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

private:
    string getFileName(const string& path) {
        size_t found = path.find_last_of("/\\");
        return path.substr(found + 1);
    }

    vector<int> search(const string& query) {
        vector<int> result;
        string lowerQuery = query;
        transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        auto it = invertedIndex.find(lowerQuery);
        if (it != invertedIndex.end()) {
            for (const auto& pair : it->second) {
                result.push_back(pair.first);
            }
        }
        return result;
    }

    int countTermFrequency(const string& content, const string& term) {
        int frequency = 0;
        size_t pos = 0;
        while ((pos = content.find(term, pos)) != string::npos) {
            ++frequency;
            pos += term.length();
        }
        return frequency;
    }

    double calculateBM25Score(const string& query, int docId) {
        // Tokenize query
        istringstream iss(query);
        vector<string> tokens{istream_iterator<string>{iss}, istream_iterator<string>{}};

        // Calculate BM25 score for the document
        double score = 0.0;
        for (const string& term : tokens) {
            string lowerTerm = term;
            transform(lowerTerm.begin(), lowerTerm.end(), lowerTerm.begin(), ::tolower); // Transform to lowercase
            int termFrequency = countTermFrequency(documents[docId].content, lowerTerm);

            // Calculate document frequency (df) and inverse document frequency (idf)
            int df = invertedIndex[lowerTerm].size();
            double idf = log((documents.size() - df + 0.5) / (df + 0.5)); // Fully qualify log function

            // BM25 parameters
            double k1 = 1.5;
            double b = 0.75;
            double K = k1 * ((1 - b) + b * (static_cast<double>(documents[docId].content.length()) / 1000));

            // Calculate term score
            score += idf * ((termFrequency * (k1 + 1)) / (termFrequency + K));
        }

        return score;
    }
};

int main() {
    SearchEngine searchEngine;

    // Load file names from the configuration file
    searchEngine.loadFilePathsFromConfig();

    while (true) {
        cout << "\nMenu:\n1. Perform Search\n2. Display Document Names\n3. Display Search History\n4. Browse in Browser\n5. Exit\nEnter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore(); // Consume the newline character left in the input buffer

        switch (choice) {
            case 1:
                searchEngine.performSearch();
                break;
            case 2:
                searchEngine.displayDocumentNames();
                break;
            case 3:
                searchEngine.displaySearchHistory();
                break;
            case 4: {
                string query;
                cout << "Enter search query to browse: ";
                cin.ignore(); // Consume the newline character left in the input buffer
                getline(cin, query);
                searchEngine.browseInBrowser(query);
                break;
            }
            case 5:
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
