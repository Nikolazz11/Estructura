#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

// Función para crear un directorio
int create_directory(const char *dir_name) {
    if (mkdir(dir_name, 0700) != 0) {
        if (errno == EEXIST) {
            printf("Error: El directorio '%s' ya existe.\n", dir_name);
        } else {
            printf("Error al crear el directorio '%s': %s\n", dir_name, strerror(errno));
        }
        return -1;
    }
    return 0;
}

// Función para crear archivos
int create_file(const char *file_path, const char *content) {
    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        printf("Error al crear el archivo '%s': %s\n", file_path, strerror(errno));
        return -1;
    }
    if (content != NULL) {
        fprintf(file, "%s", content);
    }
    fclose(file);
    return 0;
}

// Función que inicializa el repositorio ugit
int ugit_init() {
    if (create_directory(".ugit") != 0) {
        return -1;
    }
    if (create_file(".ugit/HEAD", "master") != 0) {
        return -1;
    }
    if (create_file(".ugit/commits", "") != 0) {
        return -1;
    }
    printf("Repositorio µGit inicializado.\n");
    return 0;
}

// Main que recibe comandos desde la terminal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Debes proporcionar un comando. Uso: ./ugit <comando>\n");
        return 1;
    }

    // Comprobamos si el comando es "init"
    if (strcmp(argv[1], "init") == 0) {
        return ugit_init();  // Ejecuta la función init
    } else {
        printf("Error: Comando no reconocido '%s'.\n", argv[1]);
        return 1;
    }
}