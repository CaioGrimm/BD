#include <iostream>
#include <fstream>
#include <string>
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

// Função para ler uma string de um arquivo binário (inclui o comprimento da string)
std::string readString(std::ifstream &file) {
    int length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    std::string str(length, '\0');
    file.read(&str[0], length);
    return str;
}

// Função para ler a tabela hash de um arquivo binário
std::unordered_map<int, Article> loadHashTableFromBinary(const std::string &filename) {
    std::unordered_map<int, Article> hashTable;
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo binário para leitura.\n";
        return hashTable;
    }

    while (file.peek() != EOF) { // Continua lendo até o final do arquivo
        Article article;

        // Lê os dados de cada campo
        file.read(reinterpret_cast<char*>(&article.id), sizeof(article.id));
        article.title = readString(file);
        file.read(reinterpret_cast<char*>(&article.year), sizeof(article.year));
        article.authors = readString(file);
        file.read(reinterpret_cast<char*>(&article.citations), sizeof(article.citations));
        article.last_update = readString(file);
        article.snippet = readString(file);

        // Adiciona o artigo à tabela hash
        hashTable[article.id] = article;
    }

    file.close();
    return hashTable;
}

int main() {
    // Nome do arquivo binário
    std::string binaryFilename = "articles.dat";

    // Carrega a tabela hash do arquivo binário
    std::unordered_map<int, Article> articleHashTable = loadHashTableFromBinary(binaryFilename);

    // Contador de artigos exibidos
    int count = 0;

    // Exibe os artigos carregados
    for (const auto &pair : articleHashTable) {
        const Article &article = pair.second;
        std::cout << "ID: " << article.id << "\n";
        std::cout << "Title: " << article.title << "\n";
        std::cout << "Year: " << article.year << "\n";
        std::cout << "Authors: " << article.authors << "\n";
        std::cout << "Citations: " << article.citations << "\n";
        std::cout << "Last Update: " << article.last_update << "\n";
        std::cout << "Snippet: " << article.snippet << "\n";
        std::cout << "-----------------------------\n";
        count++;
    }

    // Exibe o total de artigos mostrados
    std::cout << "Total de artigos exibidos: " << count << "\n";

    return 0;
}
