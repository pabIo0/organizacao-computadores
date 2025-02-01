#ifndef PROCESSADOR_H
#define PROCESSADOR_H
#define N_REGISTRADORES 32
#include <stdbool.h>
#include <stdint.h>

typedef struct instruction {
  char cod_assembly[33];
  char cod_binario[33];
  char formato_tipo;
  char instrucao_tipo[5];
  int rs, rt, rd, const_or_address;
  char ALUOp[3];
  char RegDst, ALUSrc, MemtoReg, RegWrite, MemRead, MemWrite, Branch;
} instructions;

typedef struct instruction_bin {
  char cod_binario[33];
  char formato_tipo;
  char instrucao_tipo[5];
  int rs, rt, rd, const_or_address;

} instructions_bin;

typedef struct ent {
  instructions *entradas;
  int quantidade, capacidade;
} lista_entradas;

typedef struct sai {
  instructions_bin *saidas;
  int quantidade, capacidade;
} lista_saidas;

typedef enum {
    ASSEMBLY,
    BINARIO
} Tipo;

extern uint32_t registradores[N_REGISTRADORES];

/*
Função responsável por chamar todas as outras funções presentes no nosso
simulador de processador monociclo MIPS entrada: não tem saída: não tem
*/
void Controlador_Principal_Assembly();

/*
Função responsável por inicializar os registradores, que nada mais é que um
vetor de inteiros onde os valores salvos nos registradores são salvos em seus
índices equivalentes entrada: não tem saída: não tem
*/
void inicializar_registradores();

/*
Função responsável por inicializar as listas onde são salvas as instruções no
formato de Assembly e Bin 32 bits com todas as suas caracteristicas entrada:
referência para lista de entrada e, referência para lista de saída e, capacidad
e das listas cap saída: não tem
*/
void inicializar(lista_entradas *e, lista_saidas *s, int cap);

/*
Função responsável por finalizar as listas, ou seja, liberar o endereço de
memória que era ocupado por cada lista durante a execução do programa para
evitar vazamentos de memória indesejados Entrada: referência para lista de
entrada e, referência para lista de saída s saída: não tem
*/
void finalizar(lista_entradas *l, lista_saidas *s);

/*
Função responsável por inserir as instruções presentes no arquivo Entrada.txt na
minha lista de entradas l entrada: referência para lista de entrada l,
referência para lista de saída s saída: não tem
*/

void inserir_entrada(lista_entradas *l, instructions i);

/*
Função responsável por exibir a lista de entrada e todas suas particularidades,
como por exemplo, o cod assembly, cod binário, tipo de instrução... entrada:
referência para lista de entrada l saída: não tem
*/
void exibir_lista(lista_entradas *l);

/*
Função responsável por exibir o código assembly e sua versão em binário 
*/
void exibir_informacao(void *lista, int i, Tipo tipo);

/*
Função responsável por chamar todas as funções do simulador de processador que
recebe txt contendo binários e atualizar o PC entrada: não tem saída: não tem
*/
void Controlador_Principal_Binario();

/*
Função que os sinais de controle de cada instrução
entrada: referência para lista de entradas l
saída: true or false
*/
bool definir_sinais_bin(lista_entradas *l);

/*
Função que atualiza o PC com base na instrução que foi tomada
entrada: referência para lista de entradas l, inteiro i, referência para inteiro
PC saída: não tem
*/
void alteracao_pc_bin(lista_entradas *l, int i, int *PC);

/*
Função que com base no tipo da instrução, pega cada parte do código de máquina e
salva nos registradores necessários entrada: referência para lista de entradas
l, inteiro i saída: true or false
*/
bool alteracao_registrador_bin(lista_entradas *l, int i);

/*
Função responsável por realizar operações nos registradores
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
bool operacoes_registradores_bin(lista_entradas *l, int i);

/*
Função responsável por definir o tipo de instrução em formato Assembly
entrada: referência para lista de entradas l
saída: não tem
*/
void assembly_instruction_type_form(lista_entradas *l);

/*
Função responsável por ler o arquivo e salvar as instruções na lista de entradas
entrada: referência para lista de entradas l, nome do arquivo
saída: não tem
*/
void ler_arquivo_salvar_na_lista(lista_entradas *l, const char *nome_arquivo);

/*
Função responsável por processar o formato I da instrução Assembly
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void formato_I_assembly(lista_entradas *l, int i);

/*
Função responsável por imprimir o código Assembly de uma instrução
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_codigo_assembly(lista_entradas *l, int i);

/*
Função responsável por imprimir o código binário de uma instrução
entrada: referência para lista de saídas s, inteiro i
saída: não tem
*/
void printar_codigo_bin(lista_saidas *s, int i);

/*
Função responsável por definir os sinais de controle no formato Assembly
entrada: referência para lista de entradas l
saída: não tem
*/
void definir_sinais_assembly(lista_entradas *l);

/*
Função responsável por atualizar o PC no formato Assembly
entrada: referência para lista de entradas l, referência para inteiro PC
saída: não tem
*/
void alteracao_pc_assembly(lista_entradas *l, int *PC);

/*
Função responsável por imprimir os sinais de controle no formato Assembly
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_sinais_assembly(lista_entradas *l, int i);

/*
Função responsável por imprimir o PC no formato Assembly
entrada: referência para lista de entradas l, inteiro i, inteiro PC
saída: não tem
*/
void printar_pc_assembly(lista_entradas *l, int i, int PC);

/*
Função responsável por alterar os registradores no formato Assembly
entrada: referência para lista de entradas l
saída: não tem
*/
void alteracao_registrador_assembly(lista_entradas *l);

/*
Função responsável por realizar operações nos registradores no formato Assembly
entrada: referência para lista de entradas l
saída: não tem
*/
void operacoes_registradores_assembly(lista_entradas *l);

/*
Função responsável por preencher a lista de saídas com base nas instruções de
entrada entrada: referência para lista de entradas l, referência para lista de
saídas s saída: não tem
*/
void preencher_lista_out(lista_entradas *l, lista_saidas *s);

/*
Função responsável por preencher a lista de saídas com instruções do tipo R
entrada: referência para lista de entradas l, referência para lista de saídas s,
inteiro i saída: não tem
*/
void preencher_tipo_R(lista_entradas *l, lista_saidas *s, int i);

/*
Função responsável por preencher a lista de saídas com instruções do tipo I
entrada: referência para lista de entradas l, referência para lista de saídas s,
inteiro i saída: não tem
*/
void preencher_tipo_I(lista_entradas *l, lista_saidas *s, int i);

/*
Função responsável por converter um número inteiro para uma string binária
entrada: número inteiro, referência para a string binária, número de bits
saída: não tem
*/
void intToBinaryString(int num, char *binaryString, int bits);

/*
Função responsável por escrever as saídas em um arquivo
entrada: referência para lista de saídas s, nome do arquivo
saída: não tem
*/
void escreverSaidasEmArquivo(lista_saidas *lista, const char *nomeArquivo);

/*
Função responsável por exibir a lista de instruções Assembly e suas saídas, além
do PC entrada: referência para lista de entradas l, referência para lista de
saídas s, referência para PC saída: não tem
*/
void exibir_lista_assembly(lista_entradas *l, lista_saidas *s, int *PC);

/*
Função responsável por imprimir os sinais de controle
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_sinais(lista_entradas *l, int i);

/*
Função responsável por imprimir o tipo da instrução
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_tipo_instrucao(lista_entradas *l, int i);

/*
Função responsável por imprimir o código binário de uma instrução
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_codigo_binario(lista_entradas *l, int i);

/*
Função responsável por imprimir o PC de uma instrução
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_pc_bin(lista_entradas *l, int i);

/*
Função responsável por definir o formato da instrução
entrada: referência para lista de entradas l
saída: não tem
*/
void definir_formato(lista_entradas *l);

/*
Função responsável por processar o formato R da instrução
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void formato_R(lista_entradas *l, int i);

/*
Função responsável por imprimir as mudanças de memória
entrada: referência para lista de entradas l, inteiro i
saída: não tem
*/
void printar_mudancas_memoria(lista_entradas *l, int i);

/*
Função responsável por contar o número de instruções em um arquivo
entrada: nome do arquivo
saída: número de instruções
*/
int contar_instrucoes(const char *nome_arquivo);

/*
Função responsável por inserir uma instrução na lista de entradas no formato
binário entrada: referência para lista de entradas l, instrução i saída: não tem
*/
void inserir_entrada_bin(lista_entradas *l, instructions i);

/*
Função responsável por processar uma instrução do tipo I no formato binário
entrada: referência para lista de entradas l, índice da instrução i
saída: não tem
*/
void formato_I_bin(lista_entradas *l, int i);

/*
Função responsável por imprimir as mudanças realizadas na memória durante a
execução entrada: referência para lista de entradas l, índice da instrução i
saída: não tem
*/
void printar_mudancas_memoria_bin(lista_entradas *l, int i);

/*
Função responsável por contar o número de instruções presentes em um arquivo no
formato binário entrada: nome do arquivo saída: número de instruções
*/
int contar_instrucoes_bin(const char *nome_arquivo);

/*
Função responsável por definir o tipo de cada instrução na lista de entradas no
formato binário entrada: referência para lista de entradas l saída: não tem
*/
void assembly_instruction_type_form_bin(lista_entradas *l);

/*
Função responsável por inicializar os registradores utilizados no formato
binário entrada: não tem saída: não tem
*/
void inicializar_registradores_bin();

/*
Função responsável por atualizar o contador de programa (PC) com base na
instrução atual entrada: referência para lista de entradas l, índice da
instrução i, ponteiro para PC saída: não tem
*/
void alteracao_pc_bin(lista_entradas *l, int i, int *PC);

/*
Função responsável por alterar o valor de um registrador com base em uma
instrução entrada: referência para lista de entradas l, índice da instrução i
saída: verdadeiro (true) se a alteração foi bem-sucedida, falso (false) caso
contrário
*/
bool alteracao_registrador_bin(lista_entradas *l, int i);

/*
Função responsável por realizar operações nos registradores com base em uma
instrução entrada: referência para lista de entradas l, índice da instrução i
saída: verdadeiro (true) se a operação foi bem-sucedida, falso (false) caso
contrário
*/
bool operacoes_registradores_bin(lista_entradas *l, int i);

#endif
