#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FAVORITOS 10

typedef struct {
    int id;
    char titulo[50];
    int duracao; // em minutos
} Video;

typedef struct {
    int id;
    char nome[50];
    int favoritos[MAX_FAVORITOS]; // IDs dos vídeos favoritos
    int qtd_favoritos;
} Usuario;

// Funções auxiliares
void cadastrarVideo(FILE *fp);
void cadastrarUsuario(FILE *fp);
void listarVideos(FILE *fp);
void listarUsuarios(FILE *fp, FILE *fp_videos);
void atualizarVideo(FILE *fp);
void atualizarUsuario(FILE *fp);
void removerRegistro(const char *arquivo, int id, size_t tamanhoRegistro);
void relatorioUsuarios(FILE *fpUsuarios, FILE *fpVideos);

int main() {
    int opcao;
    FILE *fp_videos, *fp_usuarios;

    fp_videos = fopen("videos.dat", "rb+");
    if (!fp_videos) fp_videos = fopen("videos.dat", "wb+");

    fp_usuarios = fopen("usuarios.dat", "rb+");
    if (!fp_usuarios) fp_usuarios = fopen("usuarios.dat", "wb+");

    do {
        printf("\nMenu:\n");
        printf("1. Cadastrar Video\n");
        printf("2. Cadastrar Usuario\n");
        printf("3. Listar Videos\n");
        printf("4. Listar Usuarios\n");
        printf("5. Atualizar Video\n");
        printf("6. Atualizar Usuario\n");
        printf("7. Remover Video\n");
        printf("8. Remover Usuario\n");
        printf("9. Relatorio Usuarios e seus videos favoritos\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);

        switch(opcao) {
            case 1: cadastrarVideo(fp_videos); break;
            case 2: cadastrarUsuario(fp_usuarios); break;
            case 3: listarVideos(fp_videos); break;
            case 4: listarUsuarios(fp_usuarios, fp_videos); break;
            case 5: atualizarVideo(fp_videos); break;
            case 6: atualizarUsuario(fp_usuarios); break;
            case 7: {
                int id;
                printf("ID do video a remover: ");
                scanf("%d", &id);
                removerRegistro("videos.dat", id, sizeof(Video));
                break;
            }
            case 8: {
                int id;
                printf("ID do usuario a remover: ");
                scanf("%d", &id);
                removerRegistro("usuarios.dat", id, sizeof(Usuario));
                break;
            }
            case 9: relatorioUsuarios(fp_usuarios, fp_videos); break;
        }
    } while (opcao != 0);

    fclose(fp_videos);
    fclose(fp_usuarios);
    return 0;
}

void cadastrarVideo(FILE *fp) {
    Video v;
    printf("ID: "); scanf("%d", &v.id);
    printf("Titulo: "); getchar(); fgets(v.titulo, 50, stdin); v.titulo[strcspn(v.titulo, "\n")] = 0;
    printf("Duracao (min): "); scanf("%d", &v.duracao);
    fseek(fp, 0, SEEK_END);
    fwrite(&v, sizeof(Video), 1, fp);
}

void cadastrarUsuario(FILE *fp) {
    Usuario u;
    printf("ID: "); scanf("%d", &u.id);
    printf("Nome: "); getchar(); fgets(u.nome, 50, stdin); u.nome[strcspn(u.nome, "\n")] = 0;
    printf("Quantidade de favoritos: "); scanf("%d", &u.qtd_favoritos);
    for (int i = 0; i < u.qtd_favoritos; i++) {
        printf("ID do video favorito %d: ", i+1);
        scanf("%d", &u.favoritos[i]);
    }
    fseek(fp, 0, SEEK_END);
    fwrite(&u, sizeof(Usuario), 1, fp);
}

void listarVideos(FILE *fp) {
    Video v;
    rewind(fp);
    printf("\nLista de Videos:\n");
    while (fread(&v, sizeof(Video), 1, fp)) {
        printf("ID: %d | Titulo: %s | Duracao: %d min\n", v.id, v.titulo, v.duracao);
    }
}

void listarUsuarios(FILE *fp, FILE *fp_videos) {
    Usuario u;
    rewind(fp);
    printf("\nLista de Usuarios:\n");
    while (fread(&u, sizeof(Usuario), 1, fp)) {
        printf("ID: %d | Nome: %s | Favoritos: ", u.id, u.nome);
        for (int i = 0; i < u.qtd_favoritos; i++) {
            printf("%d ", u.favoritos[i]);
        }
        printf("\n");
    }
}

void atualizarVideo(FILE *fp) {
    int id;
    Video v;
    rewind(fp);
    printf("ID do video a atualizar: ");
    scanf("%d", &id);

    while (fread(&v, sizeof(Video), 1, fp)) {
        if (v.id == id) {
            printf("Novo Titulo: "); getchar(); fgets(v.titulo, 50, stdin); v.titulo[strcspn(v.titulo, "\n")] = 0;
            printf("Nova Duracao: "); scanf("%d", &v.duracao);
            fseek(fp, -sizeof(Video), SEEK_CUR);
            fwrite(&v, sizeof(Video), 1, fp);
            break;
        }
    }
}

void atualizarUsuario(FILE *fp) {
    int id;
    Usuario u;
    rewind(fp);
    printf("ID do usuario a atualizar: ");
    scanf("%d", &id);

    while (fread(&u, sizeof(Usuario), 1, fp)) {
        if (u.id == id) {
            printf("Novo Nome: "); getchar(); fgets(u.nome, 50, stdin); u.nome[strcspn(u.nome, "\n")] = 0;
            printf("Nova quantidade de favoritos: "); scanf("%d", &u.qtd_favoritos);
            for (int i = 0; i < u.qtd_favoritos; i++) {
                printf("ID do video favorito %d: ", i+1);
                scanf("%d", &u.favoritos[i]);
            }
            fseek(fp, -sizeof(Usuario), SEEK_CUR);
            fwrite(&u, sizeof(Usuario), 1, fp);
            break;
        }
    }
}

void removerRegistro(const char *arquivo, int id, size_t tamanhoRegistro) {
    FILE *fp = fopen(arquivo, "rb");
    FILE *fp_tmp = fopen("tmp.dat", "wb");
    char buffer[tamanhoRegistro];

    while (fread(buffer, tamanhoRegistro, 1, fp)) {
        int *idRegistro = (int*)buffer;
        if (*idRegistro != id) {
            fwrite(buffer, tamanhoRegistro, 1, fp_tmp);
        }
    }

    fclose(fp);
    fclose(fp_tmp);
    remove(arquivo);
    rename("tmp.dat", arquivo);
    printf("Registro removido com sucesso!\n");
}

void relatorioUsuarios(FILE *fpUsuarios, FILE *fpVideos) {
    Usuario u;
    Video v;
    rewind(fpUsuarios);
    printf("\nRelatorio de Usuarios e seus Videos Favoritos:\n");
    while (fread(&u, sizeof(Usuario), 1, fpUsuarios)) {
        printf("Usuario: %s\n", u.nome);
        for (int i = 0; i < u.qtd_favoritos; i++) {
            rewind(fpVideos);
            while (fread(&v, sizeof(Video), 1, fpVideos)) {
                if (v.id == u.favoritos[i]) {
                    printf("  - %s (%d min)\n", v.titulo, v.duracao);
                    break;
                }
            }
        }
    }
}
