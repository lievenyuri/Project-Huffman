#include "huffman.h"

// -------------------- Funções de Criar Structs e Printar ----------------------

no_arvore* create_node_arvore(void* data)
{
    no_arvore* new_node = (no_arvore*) malloc(sizeof(no_arvore));
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->data = data;

    return new_node;
}

LISTA* create_list()
{
    LISTA* new_lista = (LISTA*) malloc(sizeof(LISTA));
    new_lista->head =  NULL;
    new_lista->tail = NULL;

    return new_lista;
}

void printar_HuffmanNode(no_arvore* root)
{
    if(root == NULL) return;

    printar_HuffmanNode(root->left);

    HuffmanData* info = (HuffmanData*) root->data;

    printf("Byte: %c | Freq: %ld\n", info->byte, info->frequency);

    printar_HuffmanNode(root->right);
}

void printar_HuffmanData(HuffmanData* data)
{
    printf("[%c] = %ld\n", data->byte, data->frequency);
}

void printar_no_arvore(no_arvore* item)
{
    printar_HuffmanData((HuffmanData*) item->data);
}

void printar_fila_ordenada(LISTA* fila_de_frequencia)
{
    NODE* atual = fila_de_frequencia->head;
    while(atual != NULL)
    {
        printar_no_arvore((no_arvore*) atual->item);
        atual = atual->next;
    }
    printf("\n");
}

// ------------------------------------ Criar Lista de Frequencia ----------------------------------------

void add_end(LISTA* lista, HuffmanData* data)
{
    no_arvore* new_node_arvore = create_node_arvore(data);
    NODE* new_node = (NODE*) malloc(sizeof(NODE));
    new_node->next = NULL;

    if(lista->tail != NULL && lista->head != NULL) lista->tail->next = new_node;
    else
    {
        lista->head = new_node;
        lista->tail = new_node;
    }

    new_node->item = new_node_arvore;
    lista->tail = new_node;
}

void criar_frequencia(unsigned char* texto, ssize_t mapa_frequencia[], ssize_t char_lidos) // CRIA O MAPA DE FREQUENCIA DE UM TEXTO, INCLUINDO O \n E O \0
{
    for(char_lidos; char_lidos >= 0; char_lidos--)
    {
        mapa_frequencia[texto[char_lidos]]++;
    }

    return;
}

void inserir_fila_ordenada(LISTA* fila_de_frequencia, ssize_t mapa_frequencia[])
{
    for(int i = 0; i < 256; i++)
    {
        if(mapa_frequencia[i] > 0)
        {

            HuffmanData* data_atual = (HuffmanData*) malloc(sizeof(HuffmanData));
            data_atual->byte = i;
            data_atual->frequency = mapa_frequencia[i];

            no_arvore* new_no_arvore = create_node_arvore(data_atual);

            inserir_node_ordenado(fila_de_frequencia, new_no_arvore);
        }
    }

    return;
}

// ----------------------------------------- Organizar Árvore --------------------------------------------

void inserir_node_ordenado(LISTA* lista, no_arvore* node_arvore)
{
    NODE* novo_node = (NODE*) malloc(sizeof(NODE));
    novo_node->item = node_arvore;
    novo_node->next = NULL;

    if(lista->head == NULL)
    {
        lista->head = novo_node;
        lista->tail = novo_node;
        return;
    }

    HuffmanData* dados_novo = (HuffmanData*) node_arvore->data;
    HuffmanData* dados_head = (HuffmanData*) ((no_arvore*) lista->head->item)->data;

    if(dados_novo->frequency < dados_head->frequency)
    {
        novo_node->next = lista->head;
        lista->head = novo_node;
        return;
    }

    NODE* atual = lista->head;
    while(atual->next != NULL)
    {
        HuffmanData* dados_proximo = (HuffmanData*) ((no_arvore*) atual->next->item)->data;

        if(dados_proximo->frequency > dados_novo->frequency)
        {
            break;
        }
        atual = atual->next;
    }

    novo_node->next = atual->next;
    atual->next = novo_node;

    if(novo_node->next == NULL)
    {
        lista->tail = novo_node;
    }
}

no_arvore* remover_inicio(LISTA* lista)
{
    if(lista->head == NULL) return NULL;

    NODE* aux = lista->head;
    no_arvore* item_arvore = (no_arvore*) aux->item;

    lista->head = lista->head->next;
    if(lista->head == NULL)
    {
        lista->tail = NULL;
    }

    free(aux);
    return item_arvore;
}

no_arvore* criar_arvore(LISTA* fila_de_frequencia)
{
    if(fila_de_frequencia->head == NULL) return NULL;

    while(fila_de_frequencia->head != fila_de_frequencia->tail)
    {
        no_arvore* esquerdo = remover_inicio(fila_de_frequencia);
        no_arvore* direito = remover_inicio(fila_de_frequencia);

        HuffmanData* dados_esquerdo = (HuffmanData*) esquerdo->data;
        HuffmanData* dados_direito = (HuffmanData*) direito->data;

        HuffmanData* dados_pai = (HuffmanData*) malloc(sizeof(HuffmanData));
        dados_pai->byte = '/';
        dados_pai->frequency = dados_esquerdo->frequency + dados_direito->frequency;

        no_arvore* no_pai = create_node_arvore(dados_pai);
        no_pai->left = esquerdo;
        no_pai->right = direito;

        inserir_node_ordenado(fila_de_frequencia, no_pai);
    }

    no_arvore* raiz_da_arvore = (no_arvore*) fila_de_frequencia->head->item;

    free(fila_de_frequencia->head);
    fila_de_frequencia->head = NULL;
    fila_de_frequencia->tail = NULL;

    return raiz_da_arvore;
}

// ----------------------------------------- Criar Dicionario ----------------------------------------------------

int altura_arvore(no_arvore* raiz)
{
    int esq, dir;
    if(raiz == NULL) return -1;
    else
    {
        esq = altura_arvore(raiz->left) + 1;
        dir = altura_arvore(raiz->right) + 1;

        if(esq > dir) return esq;
        else return dir;
    }
}

char** aloca_dicionario(int colunas)
{
    char** dicionario;

    dicionario = malloc(sizeof(char*) * 256);

    for(int i = 0; i < 256; i++) dicionario[i] = calloc(colunas, sizeof(char));
    
    return dicionario;
}

void gerar_dicionario(char** dicionario, no_arvore* raiz, unsigned char* string, int colunas)
{
    char esquerda[colunas], direita[colunas];

    if(raiz->left == NULL && raiz->right == NULL)
    {
        HuffmanData* data = (HuffmanData*) raiz->data;
        strcpy(dicionario[data->byte], string);
    }
    else
    {
        strcpy(esquerda, string);
        strcpy(direita, string);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_dicionario(dicionario, raiz->left, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->right, direita, colunas);
    }
}

void imprime_dicionario(char** dicionario)
{
    printf("\tDicionario:\n");
    for(int i = 0; i < 256; i++)
    {
        if(strcmp(dicionario[i], "") != 0) printf("\t%3c (Decimal: %3d): %s\n", i, i, dicionario[i]);
    }
    printf("\n");
}

// ---------------------------------------------- Codificar ----------------------------------------------------

char* codificar(char** dicionario, unsigned char* texto, ssize_t bytes_lidos)
{
    char* codigo = calloc((bytes_lidos*8) + 1, sizeof(char));

    for(ssize_t i = 0; i < bytes_lidos; i++)
    {
        strcat(codigo, dicionario[texto[i]]);
    }

    return codigo;
}

// Função que recebe a string de 0 e 1 e vai guardando bit a bit em um byte real
void empacotar_e_escrever(const char* bits_str, FILE* arquivo_saida, unsigned char* bit_buffer, int* bit_count) 
{
    for (int i = 0; bits_str[i] != '\0'; i++) 
    {
        if (bits_str[i] == '1') {
            // Desloca o bit 1 para a posição correta da esquerda para a direita
            *bit_buffer |= (1 << (7 - *bit_count));
        }
        // Se for '0', não precisamos fazer nada porque o buffer já começa zerado
        
        (*bit_count)++;

        // Quando completamos 8 bits, despejamos o byte real no arquivo e resetamos
        if (*bit_count == 8) {
            fputc(*bit_buffer, arquivo_saida);
            *bit_buffer = 0;
            *bit_count = 0;
        }
    }
}

// Função essencial para descarregar o último byte se ele não ficou totalmente cheio
void flush_bits(FILE* arquivo_saida, unsigned char* bit_buffer, int* bit_count) 
{
    if (*bit_count > 0) {
        // Grava o byte incompleto (o resto dele estará preenchido com zeros)
        fputc(*bit_buffer, arquivo_saida);
        *bit_buffer = 0;
        *bit_count = 0;
    }
}

// ----------------------------------------- Leitura Universal de Arquivos -------------------------------------------

void ler_arquivo(unsigned char* nome_arquivo)
{
    FILE* arquivo = fopen(nome_arquivo, "rb");
    unsigned char buffer[4096];

    size_t bytes_lidos = fread(buffer, 1, sizeof(buffer), arquivo);
}

int criar_frequencia_universal(const char* nome_arquivo, ssize_t mapa_frequencia[])
{
    FILE* arquivo = fopen(nome_arquivo, "rb");
    if(arquivo == NULL)
    {
        printf("Erro ao abrir o arquivo %s.\n", nome_arquivo);
        return 0;
    }

    unsigned char buffer[4096];
    size_t bytes_lidos;
    int total_do_arquivo = 0;

    while((bytes_lidos = fread(buffer, 1, sizeof(buffer), arquivo)) > 0)
    {
        for(size_t i = 0; i < bytes_lidos; i++)
        {
            mapa_frequencia[buffer[i]]++;
            total_do_arquivo++;
        }
    }

    return total_do_arquivo;
}

// ----------------------------- Gerador de Testes de Codificação ------------------------------

void gerar_teste_nulo() 
{
    FILE* f = fopen("teste_nulo.bin", "wb");
    unsigned char dados[] = {'A', 'B', 'C', 0, 'X', 'Y', 'Z'};
    fwrite(dados, 1, 7, f);
    fclose(f);
}

void gerar_teste_alfabeto() 
{
    FILE* f = fopen("teste_alfabeto.bin", "wb");
    for (int i = 0; i < 256; i++) {
        fputc(i, f); // Grava o byte de valor i
    }
    fclose(f);
}

void gerar_teste_grande()
{
    FILE* f = fopen("teste_grande.bin", "wb");
    for (int i = 0; i < 10000; i++) {
        fputc('A', f);
    }
    fclose(f);
}

void gerar_teste_bmp()
{
    FILE* f = fopen("imagem_teste.bmp", "wb");
    if (f == NULL) {
        printf("Erro ao criar o arquivo BMP de teste.\n");
        return;
    }

    // Cabeçalho do Arquivo BMP (File Header - 14 bytes)
    unsigned char fileHeader[14] = {
        'B', 'M',               // Assinatura que define um arquivo BMP
        0x36, 0x75, 0x00, 0x00, // Tamanho total do arquivo (30.054 bytes -> 0x7536)
        0, 0, 0, 0,             // Reservado
        0x36, 0, 0, 0           // Onde começam os pixels (offset de 54 bytes)
    };

    // Cabeçalho de Informação da Imagem (DIB Header - 40 bytes)
    unsigned char dibHeader[40] = {
        40, 0, 0, 0,            // Tamanho deste cabeçalho (40 bytes)
        100, 0, 0, 0,           // Largura da imagem (100 pixels)
        100, 0, 0, 0,           // Altura da imagem (100 pixels)
        1, 0,                   // Quantidade de planos (Sempre 1)
        24, 0,                  // Bits por Pixel (24-bit: 3 bytes por pixel - RGB)
        0, 0, 0, 0,             // Compressão (0 = Nenhuma)
        0, 0, 0, 0,             // Tamanho dos dados brutos (Pode ser 0 se não houver compressão)
        0, 0, 0, 0,             // Resolução horizontal
        0, 0, 0, 0,             // Resolução vertical
        0, 0, 0, 0,             // Número de cores na palete
        0, 0, 0, 0              // Cores importantes
    };

    // Escreve os 54 bytes de cabeçalhos obrigatórios
    fwrite(fileHeader, 1, 14, f);
    fwrite(dibHeader, 1, 40, f);

    // Escreve os dados dos pixels (100x100 = 10.000 pixels)
    // O formato BMP armazena os pixels de baixo para cima e na ordem BGR (Blue, Green, Red)
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            unsigned char r = (x * 255) / 100; // Gradiente horizontal de vermelho
            unsigned char g = (y * 255) / 100; // Gradiente vertical de verde
            unsigned char b = 150;             // Azul fixo para dar um tom lilás/ciano

            fputc(b, f); // Escreve Azul
            fputc(g, f); // Escreve Verde
            fputc(r, f); // Escreve Vermelho
        }
    }

    fclose(f);
    printf("Imagem de teste 'imagem_teste.bmp' gerada com sucesso (30.054 bytes)!\n");
}

void limpar_ecra() 
{
    // \033[H  -> Move o cursor para o canto superior esquerdo (posição 1,1)
    // \033[2J -> Limpa o ecrã inteiro
    printf("\033[H\033[2J");
    fflush(stdout); // Garante que o comando é enviado imediatamente
}

// ----------------------------------------- Decodificar -----------------------------------------

void decodificar(no_arvore* raiz)
{
    if(raiz == NULL)
    {
        printf("\n[Erro] Nenhuma árvore de Huffman encontrada na memória.\n");
        printf("Por favor, compacte um arquivo primeiro para gerar a árvore!\n");
        return;
    }

    FILE* arquivo_comprimido = fopen("texto_comprimido.txt", "rb");

    if(arquivo_comprimido == NULL)
    {
        printf("Erro ao abrir o arquivo texto_comprimido.txt .\n");
        return;
    }

    char nome_original[256];
    size_t tamanho_original = 0;
    if(fscanf(arquivo_comprimido, "%255s %ld\n", nome_original, &tamanho_original) != 2)
    {
        printf("Erro ao ler o cabeçalho do arquivo comprimido.\n");
        fclose(arquivo_comprimido);
        return;
    }

    char nome_saida[300];
    sprintf(nome_saida, "descompactado_%s", nome_original);
    FILE* arquivo_descomprimido = fopen(nome_saida, "wb");

    if(arquivo_descomprimido == NULL)
    {
        printf("Erro ao criar o arquivo de saida descompactado.\n");
        fclose(arquivo_comprimido);
        return;
    }

    no_arvore* atual = raiz;
    unsigned char buffer_leitura[4096];
    size_t bytes_lidos;
    long bytes_restaurados = 0;

    while(bytes_restaurados < tamanho_original && (bytes_lidos = fread(buffer_leitura, 1, sizeof(buffer_leitura), arquivo_comprimido)) > 0)
    {
        for(size_t i = 0; i < bytes_lidos; i++)
        {
            unsigned char byte_atual = buffer_leitura[i];

            for(int bit_pos = 0; bit_pos < 8; bit_pos++)
            {
                int bit = (byte_atual >> (7 - bit_pos)) & 1;

                if(bit == 0) atual = atual->left;
                else if(bit == 1) atual = atual->right;

                if(atual->left == NULL && atual->right == NULL)
                {
                    HuffmanData* data = (HuffmanData*) atual->data;

                    fputc(data->byte, arquivo_descomprimido);
                    bytes_restaurados++;
                    
                    atual = raiz;

                    if(bytes_restaurados == tamanho_original)
                    {
                        break;
                    }
                }
            }
            if(bytes_restaurados == tamanho_original) break;
        }
    }

    fclose(arquivo_comprimido);
    fclose(arquivo_descomprimido);

    printf("Arquivo decodificado e restaurado com sucesso em '%s' !\n", nome_saida);
}