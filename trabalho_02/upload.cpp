#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

// Estrutura de dados para representar um artigo
struct Article {
    int id;
    std::string title;
    int year;
    std::string authors;
    int citations;
    std::string last_update;
    std::string snippet;
};

// Função para remover aspas de uma string
std::string removeQuotes(const std::string &str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// Função para processar uma linha CSV e dividir nas colunas considerando aspas e ponto-e-vírgula
std::vector<std::string> parseCSVLine(const std::string &line) {
    std::vector<std::string> result;
    std::stringstream ss;
    bool inQuotes = false;

    for (char ch : line) {
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ';' && !inQuotes) {
            result.push_back(removeQuotes(ss.str()));
            ss.str("");
            ss.clear();
        } else {
            ss << ch;
        }
    }
    result.push_back(removeQuotes(ss.str()));
    return result;
}

// Função para ler o CSV e carregar dados em um vetor de artigos
std::vector<Article> readCSV(const std::string &filename) {
    std::vector<Article> articles;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo CSV.\n";
        return articles;
    }

    while (std::getline(file, line)) {
        std::vector<std::string> tokens = parseCSVLine(line);
        if (tokens.size() != 7) continue;

        Article article;
        try {
            article.id = std::stoi(tokens[0]);
            article.title = tokens[1];
            article.year = std::stoi(tokens[2]);
            article.authors = tokens[3];
            article.citations = std::stoi(tokens[4]);
            article.last_update = tokens[5];
            article.snippet = tokens[6];
            articles.push_back(article);
        } catch (const std::exception &e) {
            std::cerr << "Erro ao processar linha: " << line << "\n";
        }
    }
    return articles;
}

// Função para gravar a tabela hash em um arquivo binário
void saveHashTableToBinary(const std::unordered_map<int, Article> &hashTable, const std::string &filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo binário para escrita.\n";
        return;
    }

    for (const auto &pair : hashTable) {
        const Article &article = pair.second;

        file.write(reinterpret_cast<const char*>(&article.id), sizeof(article.id));

        int titleLength = article.title.size();
        file.write(reinterpret_cast<const char*>(&titleLength), sizeof(titleLength));
        file.write(article.title.c_str(), titleLength);

        file.write(reinterpret_cast<const char*>(&article.year), sizeof(article.year));

        int authorsLength = article.authors.size();
        file.write(reinterpret_cast<const char*>(&authorsLength), sizeof(authorsLength));
        file.write(article.authors.c_str(), authorsLength);

        file.write(reinterpret_cast<const char*>(&article.citations), sizeof(article.citations));

        int lastUpdateLength = article.last_update.size();
        file.write(reinterpret_cast<const char*>(&lastUpdateLength), sizeof(lastUpdateLength));
        file.write(article.last_update.c_str(), lastUpdateLength);

        int snippetLength = article.snippet.size();
        file.write(reinterpret_cast<const char*>(&snippetLength), sizeof(snippetLength));
        file.write(article.snippet.c_str(), snippetLength);
    }

    file.close();
    std::cout << "Tabela hash salva em arquivo binario com sucesso.\n";
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.csv>" << std::endl;
        return 1;
    }

    // Lê os artigos do arquivo CSV
    std::string csvFilename = argv[1];
    std::vector<Article> articles = readCSV(csvFilename);

    // Criação da tabela hash
    std::unordered_map<int, Article> articleHashTable;
    for (const auto &article : articles) {
        articleHashTable[article.id] = article;
    }

    // Salva a tabela hash em arquivo binário
    std::string binaryFilename = "articles.dat";
    saveHashTableToBinary(articleHashTable, binaryFilename);

    return 0;
}
