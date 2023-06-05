# Código para Processamento de Arquivo CSV

Este código em C++ é projetado para processar um arquivo CSV e combiná-lo em um único arquivo final. Ele divide o arquivo em blocos de linhas, processa cada bloco em paralelo e, em seguida, combina as colunas resultantes em um único arquivo final.

## Dependências

Este código depende das seguintes bibliotecas:

- `iostream`
- `fstream`
- `sstream`
- `string`
- `vector`
- `thread`
- `mutex`
- `filesystem`

## Compilação

Certifique-se de ter um compilador C++ compatível instalado em seu sistema. Execute o seguinte comando para compilar o código:

`g++ -std=c++17 arquivo.cpp -o arquivo`

## Uso

Após compilar o código, você pode executar o programa usando o seguinte comando:

`./arquivo`


Certifique-se de ter o arquivo CSV de entrada chamado "nome_do_dataset.csv" no mesmo diretório que o executável.

O programa processará o arquivo CSV, dividindo-o em blocos de linhas e processando cada bloco em paralelo. O tamanho do bloco é definido pela constante `TAMANHO_BLOCO` no código.

O arquivo final combinado será gerado com o nome "arquivo_final.csv" no mesmo diretório.

## Funcionamento

O código lê cada linha do arquivo CSV e divide as linhas em valores separados por vírgulas. Ele armazena os valores em colunas correspondentes e processa cada bloco de colunas em threads separadas.

Cada bloco de colunas é processado da seguinte maneira:

1. Cada coluna é verificada para determinar se contém apenas valores numéricos.
2. Se uma coluna contiver valores não numéricos, ela é processada para adicionar um ID sequencial como uma nova coluna.
3. Os dados processados são adicionados a um arquivo separado para cada coluna.
4. Os arquivos das colunas são combinados em um único arquivo final.

## Considerações

Este código foi escrito com base nas bibliotecas padrão do C++ e assume que o arquivo CSV de entrada possui um cabeçalho na primeira linha. Certifique-se de fornecer um arquivo CSV válido com os dados apropriados.

O programa usa threads para processar várias colunas em paralelo, o que pode melhorar o desempenho em sistemas com vários núcleos de CPU. No entanto, tenha em mente que a concorrência introduz complexidade e pode exigir mecanismos de sincronização adequados, como mutexes, para evitar condições de corrida.


