#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

struct Article {
    int id;
    std::string title;
    int year;
    std::string authors;
    int citations;
    std::string last_update;
    std::string snippet;

    Article() : id(0), year(0), citations(0) {}
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

    // Verifica se o arquivo foi aberto com sucesso
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo CSV: " << filename << std::endl;
        return articles;
    } else {
        std::cout << "Arquivo CSV aberto com sucesso: " << filename << std::endl;
    }

    int line_number = 1;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        Article article;

        try {
            // ID
            std::getline(ss, token, ';');
            token = removeQuotes(token);
            if (!token.empty()) {
                article.id = std::stoi(token);
            } else {
                throw std::invalid_argument("Campo ID está vazio");
            }

            // Título
            std::getline(ss, token, ';');
            article.title = removeQuotes(token);
            if (article.title.empty()) {
                throw std::invalid_argument("Campo Título está vazio");
            }

            // Ano
            std::getline(ss, token, ';');
            token = removeQuotes(token);
            if (!token.empty()) {
                article.year = std::stoi(token);
            } else {
                throw std::invalid_argument("Campo Ano está vazio");
            }

            // Autores
            std::getline(ss, token, ';');
            article.authors = removeQuotes(token);
            if (article.authors.empty()) {
                throw std::invalid_argument("Campo Autores está vazio");
            }

            // Citações
            std::getline(ss, token, ';');
            token = removeQuotes(token);
            if (!token.empty()) {
                article.citations = std::stoi(token);
            } else {
                throw std::invalid_argument("Campo Citações está vazio");
            }

            // Data de atualização
            std::getline(ss, token, ';');
            article.last_update = removeQuotes(token);
            if (article.last_update.empty()) {
                throw std::invalid_argument("Campo Data de Atualização está vazio");
            }

            // Snippet
            std::getline(ss, token, ';');
            article.snippet = removeQuotes(token);
            if (article.snippet.empty()) {
                throw std::invalid_argument("Campo Snippet está vazio");
            }

            // Adiciona o artigo ao vetor
            articles.push_back(article);
            std::cout << "Artigo carregado da linha " << line_number << ": ID " << article.id << std::endl;
        } catch (const std::invalid_argument &e) {
            std::cerr << "Erro de conversão na linha " << line_number << " do CSV: " << e.what() << std::endl;
            std::cerr << "Conteúdo da linha com erro: " << line << std::endl;
            continue;
        }
        line_number++;
    }
    file.close();
    std::cout << "Total de artigos carregados: " << articles.size() << std::endl;
    return articles;
}

// Função para gravar no arquivo de dados binário
void writeToDataFile(const std::vector<Article> &articles, const std::string &dataFilename) {
    std::ofstream dataFile(dataFilename, std::ios::binary);

    if (!dataFile.is_open()) {
        std::cerr << "Erro ao criar o arquivo de dados. Verifique as permissões ou o caminho." << std::endl;
        return;
    }

    int count = 0; // Contador para verificar quantos artigos foram gravados com sucesso
    for (const auto &article : articles) {
        dataFile.write(reinterpret_cast<const char*>(&article.id), sizeof(article.id));

        // Truncar campos para garantir que eles se encaixem nos tamanhos especificados
        std::string title = article.title.substr(0, 299);  // Limite de 300 caracteres
        dataFile.write(title.c_str(), 300);

        dataFile.write(reinterpret_cast<const char*>(&article.year), sizeof(article.year));

        std::string authors = article.authors.substr(0, 149);  // Limite de 150 caracteres
        dataFile.write(authors.c_str(), 150);

        dataFile.write(reinterpret_cast<const char*>(&article.citations), sizeof(article.citations));

        std::string last_update = article.last_update.substr(0, 19);  // Limite de 20 caracteres
        dataFile.write(last_update.c_str(), 20);

        std::string snippet = article.snippet.substr(0, 1023);  // Limite de 1024 caracteres
        dataFile.write(snippet.c_str(), 1024);

        if (dataFile.fail()) {
            std::cerr << "Erro ao gravar o artigo " << article.id << " no arquivo de dados." << std::endl;
        } else {
            count++; // Incrementa o contador se a gravação foi bem-sucedida
        }
    }

    dataFile.close();
    std::cout << "Total de registros gravados com sucesso: " << count << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::wcerr << L"Uso: " << argv[0] << " <arquivo.csv>" << std::endl;
        return 1;
    }

    std::string csvFilename = argv[1];
    std::vector<Article> articles = readCSV(csvFilename);

    std::cout << "Gravando " << articles.size() << " artigos no arquivo de dados." << std::endl;

    // Grava no arquivo de dados
    writeToDataFile(articles, "data_file.dat");

    std::wcout << L"Upload concluído com sucesso!" << std::endl;
    return 0;
}
