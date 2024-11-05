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

    // Nome do arquivo de saída de texto
    std::string outputFilename = "articles_output.txt";

    // Carrega a tabela hash do arquivo binário
    std::unordered_map<int, Article> articleHashTable = loadHashTableFromBinary(binaryFilename);

    // Abre o arquivo de saída
    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        std::cerr << "Erro ao criar o arquivo de saída.\n";
        return 1;
    }

    // Contador de artigos exibidos
    int count = 0;

    // Exibe os artigos carregados no arquivo de saída
    for (const auto &pair : articleHashTable) {
        const Article &article = pair.second;
        outputFile << "ID: " << article.id << "\n";
        outputFile << "Title: " << article.title << "\n";
        outputFile << "Year: " << article.year << "\n";
        outputFile << "Authors: " << article.authors << "\n";
        outputFile << "Citations: " << article.citations << "\n";
        outputFile << "Last Update: " << article.last_update << "\n";
        outputFile << "Snippet: " << article.snippet << "\n";
        outputFile << "-----------------------------\n";
        count++;
    }

    // Exibe o total de artigos mostrados no final do arquivo de saída
    outputFile << "Total de artigos exibidos: " << count << "\n";

    // Fecha o arquivo de saída
    outputFile.close();

    std::cout << "Artigos salvos com sucesso em " << outputFilename << "\n";

    return 0;
}
