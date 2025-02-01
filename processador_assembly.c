#include "./processador.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_REGISTRADORES 32
#define MEMORIA 1024
#define MAX_INSTRUCOES 100

uint32_t registradores[N_REGISTRADORES];

void inicializar_registradores() {
  for (int i = 0; i < 32; i++) {
    registradores[i] = 0;
  }
}

void inicializar(lista_entradas *l, lista_saidas *s, int cap) {
  l->entradas = (instructions *)calloc(cap, sizeof(instructions));
  s->saidas = (instructions_bin *)calloc(cap, sizeof(instructions_bin));
  l->quantidade = 0;
  s->quantidade = 0;
  l->capacidade = cap;
  s->capacidade = cap;
  printf("Listas Inicializadas\n");
}

void finalizar(lista_entradas *l, lista_saidas *s) {
  free(l->entradas);
  free(s->saidas);
  printf("Listas Finalizadas com Sucesso!\n");
}

void inserir_entrada(lista_entradas *l, instructions i) {
  if (l->quantidade < MAX_INSTRUCOES) {
    l->entradas[l->quantidade] = i;
    l->quantidade++;
  } else {
    printf("*** ERRO!!! O Limite de instrucoes e de %d instrucoes ***\n",
           MAX_INSTRUCOES);
  }
}

void ler_arquivo_salvar_na_lista(lista_entradas *l, const char *nome_arquivo) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  if (!arquivo) {
    fprintf(stderr, "Erro ao abrir o arquivo %s.\n", nome_arquivo);
    return;
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), arquivo)) {
    char *instrucao = strtok(buffer, ";");
    while (instrucao) {
      while (isspace((unsigned char)*instrucao)) {
        instrucao++;
      }

      char *end = instrucao + strlen(instrucao) - 1;
      while (end > instrucao && isspace((unsigned char)*end)) {
        end--;
      }
      *(end + 1) = '\0';

      if (strlen(instrucao) > 0) {
        instructions nova_instrucao;
        memset(&nova_instrucao, 0,
               sizeof(instructions)); // Inicializa todos os campos com 0
        strncpy(nova_instrucao.cod_assembly, instrucao,
                sizeof(nova_instrucao.cod_assembly) - 1);

        inserir_entrada(l, nova_instrucao);
      }

      instrucao = strtok(NULL, ";");
    }
  }

  fclose(arquivo);
}

void exibir_lista_assembly(lista_entradas *l, lista_saidas *s, int *PC) {
  if (l->quantidade == 0) {
    printf("Lista de instrucoes vazia.\n");
    return;
  }

  printf("Exibindo as instrucoes armazenadas:\n");
  for (int i = 0; i < l->quantidade; i++) {
    printf("Instrucao Assembly %d:\n", i + 1);
    exibir_informacao(l, i, ASSEMBLY);
    exibir_informacao(s, i, BINARIO);
    printar_tipo_instrucao(l, i);
    printar_mudancas_memoria(l, i);
    printar_pc_assembly(l, i, *PC);
    printar_sinais_assembly(l, i);

    printf("\n");
  }
}

void printar_sinais_assembly(lista_entradas *l, int i) {
  if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "sub") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "lw") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "sw") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "addi") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "beq") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
    printf("ALUOp: %s, RegDst: %c, ALUSrc: %c, MemtoReg: %c, RegWrite: %c, "
           "MemRead: %c, MemWrite: %c, Branch: %c\n",
           l->entradas[i].ALUOp, l->entradas[i].RegDst, l->entradas[i].ALUSrc,
           l->entradas[i].MemtoReg, l->entradas[i].RegWrite,
           l->entradas[i].MemRead, l->entradas[i].MemWrite,
           l->entradas[i].Branch);
  } else {
    printf("Erro!!!: Tipo de instrucao desconhecido");
  }
}

void printar_tipo_instrucao(lista_entradas *l, int i) {
  printf("Tipo de instrucao: %s\n", l->entradas[i].instrucao_tipo);
}

void exibir_informacao(void *lista, int i, Tipo tipo) {
    if (tipo == ASSEMBLY) {
        lista_entradas *l = (lista_entradas *) lista;
        printf("Codigo Assembly: %s\n", l->entradas[i].cod_assembly);
    } else if (tipo == BINARIO) {
        lista_saidas *s = (lista_saidas *) lista;
        printf("Codigo Binario: %s\n", s->saidas[i].cod_binario);
    }
}

void definir_sinais_assembly(lista_entradas *l) {
  for (int i = 0; i < l->quantidade; i++) {
    if (l->entradas[i].formato_tipo == 'R') {
      strcpy(l->entradas[i].ALUOp, "10");
      l->entradas[i].RegDst = '1';
      l->entradas[i].ALUSrc = '0';
      l->entradas[i].MemtoReg = '0';
      l->entradas[i].RegWrite = '1';
      l->entradas[i].MemRead = '0';
      l->entradas[i].MemWrite = '0';
      l->entradas[i].Branch = '0';
    } else if (l->entradas[i].formato_tipo == 'I') {
      formato_I_assembly(l, i);
    }
  }
}

void formato_I_assembly(lista_entradas *l, int i) {
  // lw
  if (strcmp(l->entradas[i].instrucao_tipo, "lw") == 0) {
    strcpy(l->entradas[i].ALUOp, "00");
    l->entradas[i].RegDst = '0';
    l->entradas[i].ALUSrc = '1';
    l->entradas[i].MemtoReg = '1';
    l->entradas[i].RegWrite = '1';
    l->entradas[i].MemRead = '1';
    l->entradas[i].MemWrite = '0';
    l->entradas[i].Branch = '0';
    strcpy(l->entradas[i].instrucao_tipo, "lw");
  }
  // sw
  else if (strcmp(l->entradas[i].instrucao_tipo, "sw") == 0) {
    strcpy(l->entradas[i].ALUOp, "00");
    l->entradas[i].RegDst = 'X';
    l->entradas[i].ALUSrc = '1';
    l->entradas[i].MemtoReg = 'X';
    l->entradas[i].RegWrite = '0';
    l->entradas[i].MemRead = '0';
    l->entradas[i].MemWrite = '1';
    l->entradas[i].Branch = '0';
    strcpy(l->entradas[i].instrucao_tipo, "sw");
  }
  // beq
  else if (strcmp(l->entradas[i].instrucao_tipo, "beq") == 0) {
    strcpy(l->entradas[i].ALUOp, "01");
    l->entradas[i].RegDst = 'X';
    l->entradas[i].ALUSrc = '0';
    l->entradas[i].MemtoReg = 'X';
    l->entradas[i].RegWrite = '0';
    l->entradas[i].MemRead = '0';
    l->entradas[i].MemWrite = '0';
    l->entradas[i].Branch = '1';
    strcpy(l->entradas[i].instrucao_tipo, "beq");
  }

  // addi
  else if (strcmp(l->entradas[i].instrucao_tipo, "addi") == 0) {
    strcpy(l->entradas[i].ALUOp, "10");
    l->entradas[i].RegDst = '0';
    l->entradas[i].ALUSrc = '1';
    l->entradas[i].MemtoReg = '0';
    l->entradas[i].RegWrite = '1';
    l->entradas[i].MemRead = '0';
    l->entradas[i].MemWrite = '0';
    l->entradas[i].Branch = '0';
    strcpy(l->entradas[i].instrucao_tipo, "addi");
  }

  // jump
  else if (strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
    strcpy(l->entradas[i].ALUOp, "00");
    l->entradas[i].RegDst = '0';
    l->entradas[i].ALUSrc = '0';
    l->entradas[i].MemtoReg = '0';
    l->entradas[i].RegWrite = '0';
    l->entradas[i].MemRead = '0';
    l->entradas[i].MemWrite = '0';
    l->entradas[i].Branch = '0';
    strcpy(l->entradas[i].instrucao_tipo, "j");
  } else {
    printf("Instrução desconhecida indice: %d\n %s\n", i,
           l->entradas[i].cod_assembly);
    strcpy(l->entradas[i].instrucao_tipo, "desconhecido");
  }
}

void alteracao_pc_assembly(lista_entradas *l, int *PC) {
  if(*PC <= MEMORIA - 4){
    for (int i = 0; i < l->quantidade; i++) {
      if (strcmp(l->entradas[i].instrucao_tipo, "beq") == 0) {
        if (registradores[l->entradas[i].rs] ==
            registradores[l->entradas[i].rt]) {
          int novo_pc = *PC + 4 + (l->entradas[i].const_or_address * 4);
          *PC = novo_pc;
        } else {
          *PC += 4;
        }
      }

      else if ((strcmp(l->entradas[i].instrucao_tipo, "add") == 0 ||
                strcmp(l->entradas[i].instrucao_tipo, "sub") == 0 ||
                strcmp(l->entradas[i].instrucao_tipo, "lw") == 0 ||
                strcmp(l->entradas[i].instrucao_tipo, "sw") == 0 ||
                strcmp(l->entradas[i].instrucao_tipo, "addi") == 0)) {
        *PC = *PC + 4;
      }

      else if (strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
        if (l->entradas[i].const_or_address * 4 + *PC + 4 <= MEMORIA) {
          int novo_pc = *PC + (l->entradas[i].const_or_address * 4);
          *PC = novo_pc;
        }
      }
    }
  }
}

void printar_pc_assembly(lista_entradas *l, int i, int PC) {
  if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "sub") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "lw") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "sw") == 0 ||
      strcmp(l->entradas[i].instrucao_tipo, "addi") == 0) {
    printf("PC = PC + 4\n");
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "beq") == 0) {
    if (registradores[l->entradas[i].rs] == registradores[l->entradas[i].rt]) {
      printf("Registradores Iguais => PC = PC + 4 + %d\n",
             l->entradas[i].const_or_address * 4);
    } else {
      printf("Registradores Diferentes => PC = PC + 4 \n");
    }
  } else if (strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
    if (((l->entradas[i].const_or_address * 4) + PC) <= MEMORIA) {
      printf("PC = PC + %d\n", l->entradas[i].const_or_address * 4);
    } else {
      printf("ERRO!!!: Jump Supera o Limite de Memoria!!!\n");
    }
  }
}

void alteracao_registrador_assembly(lista_entradas *l) {
  const char *registradores_nome[N_REGISTRADORES] = {
      "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
      "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
      "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
      "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

  char rs_ass[10];
  char rt_ass[10];
  char rd_ass[10];
  int immediate;

  for (int i = 0; i < l->quantidade; i++) {
    if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0 ||
        strcmp(l->entradas[i].instrucao_tipo, "sub") == 0) {
      sscanf(l->entradas[i].cod_assembly, "%*s %s %s %s", rd_ass, rs_ass,
             rt_ass);

      for (int j = 0; j < N_REGISTRADORES; j++) {
        if (strcmp(rd_ass, registradores_nome[j]) == 0) {
          l->entradas[i].rd = j;
        }

        if (strcmp(rs_ass, registradores_nome[j]) == 0) {
          l->entradas[i].rs = j;
        }

        if (strcmp(rt_ass, registradores_nome[j]) == 0) {
          l->entradas[i].rt = j;
        }
      }
    } else if (strcmp(l->entradas[i].instrucao_tipo, "addi") == 0) {
      sscanf(l->entradas[i].cod_assembly, "%*s %s %s %d", rt_ass, rs_ass,
             &immediate);

      for (int j = 0; j < N_REGISTRADORES; j++) {
        if (strcmp(rt_ass, registradores_nome[j]) == 0) {
          l->entradas[i].rt = j;
        }

        if (strcmp(rs_ass, registradores_nome[j]) == 0)
          l->entradas[i].rs = j;
      }

      l->entradas[i].const_or_address = immediate;
    } else if (strcmp(l->entradas[i].instrucao_tipo, "lw") == 0 ||
               strcmp(l->entradas[i].instrucao_tipo, "sw") == 0) {
      sscanf(l->entradas[i].cod_assembly, "%*s %s %d(%[^)])", rt_ass,
             &immediate, rs_ass);

      for (int j = 0; j < N_REGISTRADORES; j++) {
        if (strcmp(rt_ass, registradores_nome[j]) == 0)
          l->entradas[i].rt = j;

        if (strcmp(rs_ass, registradores_nome[j]) == 0)
          l->entradas[i].rs = j;
      }

      l->entradas[i].const_or_address = immediate;
    } else if (strcmp(l->entradas[i].instrucao_tipo, "beq") == 0) {
      sscanf(l->entradas[i].cod_assembly, "%*s %s %s %d", rs_ass, rt_ass,
             &immediate);

      for (int j = 0; j < N_REGISTRADORES; j++) {
        if (strcmp(rt_ass, registradores_nome[j]) == 0)
          l->entradas[i].rt = j;

        if (strcmp(rs_ass, registradores_nome[j]) == 0)
          l->entradas[i].rs = j;
      }

      l->entradas[i].const_or_address = immediate;
    } else if (strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
      sscanf(l->entradas[i].cod_assembly, "%*s %d", &immediate);
      l->entradas[i].const_or_address = immediate;
    }
  }
}

void operacoes_registradores_assembly(lista_entradas *l) {
  inicializar_registradores();
  for (int i = 0; i < l->quantidade; i++) {
    if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0) {
      registradores[l->entradas[i].rd] =
          registradores[l->entradas[i].rs] + registradores[l->entradas[i].rt];
    } else if (strcmp(l->entradas[i].instrucao_tipo, "sub") == 0) {
      registradores[l->entradas[i].rd] =
          registradores[l->entradas[i].rs] - registradores[l->entradas[i].rt];
    } else if (strcmp(l->entradas[i].instrucao_tipo, "addi") == 0) {
      registradores[l->entradas[i].rt] =
          registradores[l->entradas[i].rs] + l->entradas[i].const_or_address;
    }
  }
}

void printar_mudancas_memoria(lista_entradas *l, int i) {
  const char *registradores_nome[33] = {
      "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
      "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
      "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
      "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
  int offset = (l->entradas[i].const_or_address);
  int rd = l->entradas[i].rd;
  int rs = l->entradas[i].rs;
  int rt = l->entradas[i].rt;

  if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0) {
    printf("Executando %s: %s = %s + %s\nResultado: %s = %d\n",
           l->entradas[i].instrucao_tipo, registradores_nome[l->entradas[i].rd],
           registradores_nome[l->entradas[i].rs],
           registradores_nome[l->entradas[i].rt],
           registradores_nome[l->entradas[i].rd],
           registradores[l->entradas[i].rd]);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "sub") == 0) {
    printf("Executando %s: %s = %s - %s\nResultado: %s = %d\n",
           l->entradas[i].instrucao_tipo, registradores_nome[l->entradas[i].rd],
           registradores_nome[l->entradas[i].rs],
           registradores_nome[l->entradas[i].rt],
           registradores_nome[l->entradas[i].rd],
           registradores[l->entradas[i].rd]);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "addi") == 0) {
    printf("Executando %s: %s = %s + %d\nResultado: %s = %d\n",
           l->entradas[i].instrucao_tipo, registradores_nome[l->entradas[i].rt],
           registradores_nome[l->entradas[i].rs],
           l->entradas[i].const_or_address,
           registradores_nome[l->entradas[i].rt],
           registradores[l->entradas[i].rt]);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "lw") == 0) {
    printf("Executando %s: %s[%d] = %s\n", l->entradas[i].instrucao_tipo,
           registradores_nome[rs], l->entradas[i].const_or_address / 4,
           registradores_nome[rt]);
    printf("Carregado o Valor da Memoria no Endereco %s + %d Para o "
           "Registrador %s\n",
           registradores_nome[rt], l->entradas[i].const_or_address,
           registradores_nome[rs]);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "sw") == 0) {
    printf("Executando %s: %s[%d] = %s\n", l->entradas[i].instrucao_tipo,
           registradores_nome[rs], (int)(l->entradas[i].const_or_address / 4),
           registradores_nome[rt]);
    printf(
        "Armazena o Valor do Registrador %s na Memoria no Endereco %s + %d\n",
        registradores_nome[rt], registradores_nome[rs],
        l->entradas[i].const_or_address);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "beq") == 0) {
    printf("Executando %s: if %s == %s, PC = PC + 4 + (%d * 4)\n",
           l->entradas[i].instrucao_tipo, registradores_nome[l->entradas[i].rs],
           registradores_nome[l->entradas[i].rt],
           l->entradas[i].const_or_address);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
    printf("Executando %s: j %d\n", l->entradas[i].instrucao_tipo,
           l->entradas[i].const_or_address);
  } else {
    printf("ERRO! : A instrucao utiliza um registrador invalido\n");
  }
}

int contar_instrucoes(const char *nome_arquivo) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  if (!arquivo) {
    perror("Erro ao abrir o arquivo");
    return -1;
  }

  char buffer[1024];
  int contador_instrucoes = 0;

  while (fgets(buffer, sizeof(buffer), arquivo)) {

    char *instrucao = strtok(buffer, ";");
    while (instrucao) {

      while (isspace(*instrucao))
        instrucao++;
      char *end = instrucao + strlen(instrucao) - 1;
      while (end > instrucao && isspace(*end))
        end--;
      *(end + 1) = '\0';

      if (strlen(instrucao) > 0) {
        contador_instrucoes++;
      }
      instrucao = strtok(NULL, ";");
    }
  }

  fclose(arquivo);
  return contador_instrucoes;
}

void assembly_instruction_type_form(lista_entradas *l) {
  if (l != NULL) {
    for (int i = 0; i < l->quantidade; i++) {
      sscanf(l->entradas[i].cod_assembly, "%s", l->entradas[i].instrucao_tipo);
      if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0 ||
          strcmp(l->entradas[i].instrucao_tipo, "sub") == 0) {
        l->entradas[i].formato_tipo = 'R';
      } else {
        l->entradas[i].formato_tipo = 'I';
      }
    }
  }
}

void preencher_lista_out(lista_entradas *l, lista_saidas *s) {
  char str[33];
  for (int i = 0; i < l->quantidade; i++) {
    if (l->entradas[i].formato_tipo == 'R') {
      preencher_tipo_R(l, s, i);
      s->quantidade++;
    } else if (l->entradas[i].formato_tipo == 'I') {
      preencher_tipo_I(l, s, i);
      s->quantidade++;
    }
  }
}

void preencher_tipo_R(lista_entradas *l, lista_saidas *s, int i) {
  if (strcmp(l->entradas[i].instrucao_tipo, "add") == 0) {
    char rs[6];
    char rt[6];
    char rd[6];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rt, rt, 5);
    intToBinaryString(l->entradas[i].rd, rd, 5);
    strncpy(s->saidas[i].cod_binario, "000000", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rt, 5);
    strncpy(s->saidas[i].cod_binario + 16, rd, 5);
    strncpy(s->saidas[i].cod_binario + 21, "000000", 5);
    strncpy(s->saidas[i].cod_binario + 26, "100000", 6);
  } else if (strcmp(l->entradas[i].instrucao_tipo, "sub") == 0) {
    char rs[6];
    char rt[6];
    char rd[6];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rt, rt, 5);
    intToBinaryString(l->entradas[i].rd, rd, 5);
    strncpy(s->saidas[i].cod_binario, "000000", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rt, 5);
    strncpy(s->saidas[i].cod_binario + 16, rd, 5);
    strncpy(s->saidas[i].cod_binario + 21, "000000", 5);
    strncpy(s->saidas[i].cod_binario + 26, "100010", 6);
  }
}
void preencher_tipo_I(lista_entradas *l, lista_saidas *s, int i) {
  if (strcmp(l->entradas[i].instrucao_tipo, "addi") == 0) {
    char rs[6];
    char rd[6];
    char immediate[17];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rd, rd, 5);
    intToBinaryString(l->entradas[i].const_or_address, immediate, 16);
    strncpy(s->saidas[i].cod_binario, "001000", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rd, 5);
    strncpy(s->saidas[i].cod_binario + 16, immediate, 16);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "lw") == 0) {
    char rs[6];
    char rt[6];
    char immediate[17];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rt, rt, 5);
    intToBinaryString(l->entradas[i].const_or_address, immediate, 16);
    strncpy(s->saidas[i].cod_binario, "100011", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rt, 5);
    strncpy(s->saidas[i].cod_binario + 16, immediate, 16);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "sw") == 0) {
    char rs[6];
    char rt[6];
    char immediate[17];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rt, rt, 5);
    intToBinaryString(l->entradas[i].const_or_address, immediate, 16);
    strncpy(s->saidas[i].cod_binario, "101011", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rt, 5);
    strncpy(s->saidas[i].cod_binario + 16, immediate, 16);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "beq") == 0) {
    char rs[6];
    char rt[6];
    char immediate[17];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rt, rt, 5);
    intToBinaryString(l->entradas[i].const_or_address, immediate, 16);
    strncpy(s->saidas[i].cod_binario, "000100", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rt, 5);
    strncpy(s->saidas[i].cod_binario + 16, immediate, 16);
  }

  else if (strcmp(l->entradas[i].instrucao_tipo, "j") == 0) {
    char rs[6];
    char rt[6];
    char immediate[17];
    intToBinaryString(l->entradas[i].rs, rs, 5);
    intToBinaryString(l->entradas[i].rt, rt, 5);
    intToBinaryString(l->entradas[i].const_or_address, immediate, 16);
    strncpy(s->saidas[i].cod_binario, "000010", 6);
    strncpy(s->saidas[i].cod_binario + 6, rs, 5);
    strncpy(s->saidas[i].cod_binario + 11, rt, 5);
    strncpy(s->saidas[i].cod_binario + 16, immediate, 16);
  }
}

void intToBinaryString(int num, char *binaryString, int bits) {
  for (int i = bits - 1; i >= 0; i--) {
    binaryString[bits - 1 - i] = (num & (1 << i)) ? '1' : '0';
  }
  binaryString[bits] = '\0';
}

void escreverSaidasEmArquivo(lista_saidas *lista, const char *nomeArquivo) {
  FILE *arquivo = fopen(nomeArquivo, "w");

  if (arquivo == NULL) {
    perror("Erro ao abrir o arquivo");
    return;
  }

  for (int i = 0; i < lista->quantidade; i++) {
    fprintf(arquivo, "%s\n", lista->saidas[i].cod_binario);
  }

  fclose(arquivo);
  printf("Os codigos binarios foram salvos em '%s'.\n", nomeArquivo);
}

void Controlador_Principal_Assembly() {
  const char *entrada_binario = "Entrada.txt";
  const char *saida_binario = "Saida.txt";
  int PC = 0;

  lista_entradas lista;
  lista_saidas lista_output;
  inicializar(&lista, &lista_output, contar_instrucoes(entrada_binario));
  ler_arquivo_salvar_na_lista(&lista, entrada_binario);
  assembly_instruction_type_form(&lista);
  definir_sinais_assembly(&lista);
  alteracao_registrador_assembly(&lista);
  operacoes_registradores_assembly(&lista);
  alteracao_pc_assembly(&lista, &PC);
  preencher_lista_out(&lista, &lista_output);
  escreverSaidasEmArquivo(&lista_output, saida_binario);
  exibir_lista_assembly(&lista, &lista_output, &PC);
  printf("PC Atual: %d\n\n", PC);
  finalizar(&lista, &lista_output);
}
