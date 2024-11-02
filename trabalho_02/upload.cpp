#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <functional>

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

// Função para remover aspas duplas de uma string
std::string removeQuotes(const std::string &str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// Função para ler o arquivo CSV e carregar os dados em um vetor de artigos
std::vector<Article> readCSV(const std::string &filename) {
    std::vector<Article> articles;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo CSV: " << filename << std::endl;
        return articles;
    }

    int line_number = 1; // Para rastrear qual linha está causando problemas
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        Article article;

        try {
            // ID
            std::getline(ss, token, ';');
            token = removeQuotes(token);
            if (!token.empty()) {
                article.id = std::stoi(token);  // Converte para inteiro
            } else {
                throw std::invalid_argument("Campo ID esta vazio");
            }

            // Título
            std::getline(ss, article.title, ';');
            article.title = removeQuotes(article.title);

            // Ano
            std::getline(ss, token, ';');
            token = removeQuotes(token);
            if (!token.empty()) {
                article.year = std::stoi(token);  // Converte para inteiro
            } else {
                throw std::invalid_argument("Campo Ano está vazio");
            }

            // Autores
            std::getline(ss, article.authors, ';');
            article.authors = removeQuotes(article.authors);

            // Citações
            std::getline(ss, token, ';');
            token = removeQuotes(token);
            if (!token.empty()) {
                article.citations = std::stoi(token);  // Converte para inteiro
            } else {
                throw std::invalid_argument("Campo Citacoes esta vazio");
            }

            // Data de atualização
            std::getline(ss, article.last_update, ';');
            article.last_update = removeQuotes(article.last_update);

            // Snippet
            std::getline(ss, article.snippet, ';');
            article.snippet = removeQuotes(article.snippet);

            // Adiciona o artigo ao vetor
            articles.push_back(article);

        } catch (const std::invalid_argument &e) {
            std::cerr << "Erro de conversao na linha " << line_number << ": " << e.what() << std::endl;
            std::cerr << "Conteudo da linha com erro: " << line << std::endl;
            continue;  // Ignora esta linha e passa para a próxima
        }
        line_number++;
    }

    file.close();
    return articles;
}

// Função para gravar os dados em um arquivo binário organizado por hashing
void writeDataFile(const std::vector<Article> &articles, const std::string &dataFilename) {
    std::unordered_map<int, Article> hashTable;
    for (const auto &article : articles) {
        hashTable[article.id] = article;  // Armazena no map, simulando uma tabela hash
    }

    std::ofstream dataFile(dataFilename, std::ios::binary);
    if (!dataFile.is_open()) {
        std::cerr << "Erro ao criar o arquivo de dados." << std::endl;
        return;
    }

    for (const auto &pair : hashTable) {
        const Article &article = pair.second;
        dataFile.write(reinterpret_cast<const char*>(&article.id), sizeof(article.id));
        
        std::string title = article.title.substr(0, 299);
        dataFile.write(title.c_str(), 300);

        dataFile.write(reinterpret_cast<const char*>(&article.year), sizeof(article.year));
        
        std::string authors = article.authors.substr(0, 149);
        dataFile.write(authors.c_str(), 150);

        dataFile.write(reinterpret_cast<const char*>(&article.citations), sizeof(article.citations));
        
        std::string last_update = article.last_update.substr(0, 19);
        dataFile.write(last_update.c_str(), 20);

        std::string snippet = article.snippet.substr(0, 1023);
        dataFile.write(snippet.c_str(), 1024);
    }
    dataFile.close();
    std::cout << "Arquivo de dados gravado com sucesso." << std::endl;
}

// Estrutura básica de B+Tree para o índice
class BPlusTree {
public:
    void insert(int key, int position);
    int search(int key);
    // Funções adicionais, como remoção, impressão, etc.
    void display();
    
private:
    std::unordered_map<int, int> indexMap;  // Mapeamento simplificado de chave para posição
};

void BPlusTree::insert(int key, int position) {
    indexMap[key] = position;  // Insere chave e posição no mapa
}

int BPlusTree::search(int key) {
    if (indexMap.find(key) != indexMap.end()) {
        return indexMap[key];
    }
    return -1; // Retorna -1 se a chave não for encontrada
}

void BPlusTree::display() {
    for (const auto& pair : indexMap) {
        std::cout << "Chave: " << pair.first << ", Posicao: " << pair.second << std::endl;
    }
}

// Função para criar o índice primário (B+Tree) com base no ID
void createPrimaryIndex(const std::vector<Article> &articles, BPlusTree &bTree) {
    int position = 0;
    for (const auto &article : articles) {
        bTree.insert(article.id, position);
        position++;
    }
}

// Função para criar o índice secundário (B+Tree) com base no Título
void createSecondaryIndex(const std::vector<Article> &articles, BPlusTree &bTree) {
    int position = 0;
    for (const auto &article : articles) {
        int key = std::hash<std::string>{}(article.title);  // Usa hash do título como chave
        bTree.insert(key, position);
        position++;
    }
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.csv>" << std::endl;
        return 1;
    }

    std::string csvFilename = argv[1];
    std::cout << "Lendo o arquivo CSV: " << csvFilename << std::endl;

    std::vector<Article> articles = readCSV(csvFilename);
    if (articles.empty()) {
        std::cerr << "Erro: Nenhum artigo foi carregado do arquivo CSV." << std::endl;
        return 1;
    }
    std::cout << "Total de artigos carregados: " << articles.size() << std::endl;

    // Cria o arquivo de dados usando hashing
    std::cout << "Gravando o arquivo de dados..." << std::endl;
    writeDataFile(articles, "data_file.dat");

    // Verificação de criação dos índices primário e secundário
    std::cout << "Criando o índice primário..." << std::endl;
    BPlusTree primaryIndex;
    createPrimaryIndex(articles, primaryIndex);
    std::cout << "Índice primário criado com sucesso." << std::endl;

    std::cout << "Criando o índice secundário..." << std::endl;
    BPlusTree secondaryIndex;
    createSecondaryIndex(articles, secondaryIndex);
    std::cout << "Índice secundário criado com sucesso." << std::endl;

    std::cout << "Upload concluído com sucesso!" << std::endl;
    return 0;
}

