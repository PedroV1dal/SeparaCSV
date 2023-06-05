#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>

namespace fs = std::filesystem;

std::mutex mutex;

void add_to_file(const std::vector<std::string>& dados, const std::string& nomeArquivo) {
    /*
    Função para adicionar os dados ao arquivo    
    */
    std::ofstream arquivo(nomeArquivo, std::ios::app);
    if (!arquivo) {
        std::cerr << "Erro ao abrir o arquivo " << nomeArquivo << std::endl;
        return;
    }

    // Adiciona cada valor dos dados ao arquivo, seguido por uma nova linha
    for (const auto& valor : dados) {
        arquivo << valor << "\n";
    }

    arquivo.close();
}

void add_id(std::vector<std::string>& dados) {
    /*
    Adiciona ID (número sequencial) como uma coluna
    */

    for (size_t i = 0; i < dados.size(); ++i) {
        if (i == 0) {
            dados[i] = "id," + dados[i];  // Adiciona "id" na primeira coluna
        } else {
            dados[i] = std::to_string(i) + "," + dados[i];  // Adiciona números sequenciais nas colunas subsequentes
        }
    }
}

void process_column(const std::vector<std::string>& coluna, const std::string& nomeArquivo) {
    /*
    Função para processar uma coluna e adicionar os dados a um arquivo
    */
    std::vector<std::string> dados;
    dados.reserve(coluna.size());

    // Copia os valores da coluna para o vetor de dados, ignorando a primeira linha
    for (size_t i = 1; i < coluna.size(); ++i) {
        dados.push_back(coluna[i]);
    }

    add_id(dados);

    add_to_file(dados, nomeArquivo);
}

bool verify_column(const std::vector<std::string>& coluna) {
    /*
    Verifica se o conteúdo das primeiras linhas de uma coluna é numérico
    */
    for (size_t i = 1; i < coluna.size(); ++i) {
        // Tenta converter cada valor para um número
        try {
            std::stod(coluna[i]);
        } catch (const std::exception&) {
            // Se ocorrer uma exceção ao tentar converter para número, significa que o valor não é numérico
            return false;
        }
    }
    // Se todos os valores forem numéricos, retorna true
    return true;
}

void combine_columns(const std::vector<std::vector<std::string>>& colunas, const std::string& nomeArquivoFinal) {
    std::ofstream arquivoFinal(nomeArquivoFinal);  // Abre o arquivo final para escrita
    if (!arquivoFinal) {
        std::cerr << "Erro ao criar o arquivo final." << std::endl;
        return;
    }

    const size_t TAMANHO_BLOCO = 1000;

    for (size_t linha = 0; linha < TAMANHO_BLOCO; ++linha) {
        bool primeiraColuna = true;
        for (size_t coluna = 0; coluna < colunas.size(); ++coluna) {
            std::string nomeArquivo = "coluna_" + std::to_string(coluna) + ".csv";
            std::ifstream arquivoColuna(nomeArquivo);

            if (!arquivoColuna) {
                std::cerr << "Erro ao abrir o arquivo da coluna " << coluna << std::endl;
                continue;
            }

            std::string valor;
            for (size_t i = 0; i <= linha; ++i) {
                if (!std::getline(arquivoColuna, valor)) {
                    // Se não houver mais linhas no arquivo da coluna, sair do loop
                    break;
                }
            }

            // Verifica se a coluna contém apenas strings vazias
            if (valor.find_first_not_of(",") != std::string::npos) {
                if (!primeiraColuna) {
                    arquivoFinal << ",";  // Adiciona vírgula como separador entre colunas
                }
                arquivoFinal << valor;
                primeiraColuna = false;
            }

            arquivoColuna.close();
        }

        arquivoFinal << std::endl;  // Pula para a próxima linha no arquivo final
    }

    arquivoFinal.close();

    std::cout << "Arquivo final gerado com sucesso!" << std::endl;
}



int main() {
    // Abertura do arquivo CSV
    std::ifstream arquivoCSV("dataset_simples.csv");
    if (!arquivoCSV) {
        std::cerr << "Erro ao abrir o arquivo CSV." << std::endl;
        return 1;
    }

    const size_t TAMANHO_BLOCO = 1000;  // Tamanho do bloco de linhas a ser processado
    std::string nomeArquivoFinal = "arquivo_final.csv";

    std::vector<std::vector<std::string>> colunas;  // Vetor para armazenar as colunas
    std::string linha;
    size_t linhaAtual = 0;

    // Ignorar a primeira linha (cabeçalho)
    if (std::getline(arquivoCSV, linha)) {
        linhaAtual++;
    }

    // Lê cada linha do arquivo CSV
    while (std::getline(arquivoCSV, linha)) {
        std::stringstream ss(linha);
        std::string valor;
        size_t coluna = 0;

        // Divide a linha em valores separados por vírgula
        while (std::getline(ss, valor, ',')) {
            if (colunas.size() <= coluna) {
                colunas.push_back(std::vector<std::string>());
            }
            colunas[coluna].push_back(valor);  // Armazena o valor na coluna correspondente
            coluna++;
        }

        linhaAtual++;

        // Verifica se o tamanho do bloco foi atingido ou se chegou ao final do arquivo
        if (linhaAtual % TAMANHO_BLOCO == 0 || arquivoCSV.eof()) {
            std::vector<std::thread> threads;

            // Processa cada coluna em uma thread separada
            for (size_t i = 0; i < colunas.size(); ++i) {
                std::string nomeArquivo = "coluna_" + std::to_string(i) + ".csv";
                if (!verify_column(colunas[i])) {
                    threads.emplace_back(process_column, std::ref(colunas[i]), nomeArquivo);
                }
            }

            // Aguarda todas as threads concluírem
            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }

            colunas.clear();  // Limpa o vetor de colunas para o próximo bloco
        }
    }

    arquivoCSV.close();

    // Combine os arquivos das colunas em um único arquivo final
    combine_columns(colunas, nomeArquivoFinal);

    return 0;
}