#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define STAGING_DIR ".ugit/stage"
#define INDEX_FILE ".ugit/index"

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
    // Crear el directorio .ugit
    if (create_directory(".ugit") != 0) {
        return -1;
    }

    // Crear el directorio de stage
    if (create_directory(STAGING_DIR) != 0) {
        return -1;
    }

    // Crear archivos necesarios dentro de .ugit
    if (create_file(".ugit/HEAD", "master") != 0) {
        return -1;
    }

    // Crear un archivo vacío para los commits
    if (create_file(".ugit/commits", "") != 0) {
        return -1;
    }

    // Crear el archivo índice vacío
    if (create_file(INDEX_FILE, "") != 0) {
        return -1;
    }

    printf("Repositorio µGit inicializado.\n");
    return 0;
}

// Función para verificar si un archivo existe
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Función para copiar el contenido de un archivo
int copy_file(const char *src, const char *dest) {
    FILE *source_file = fopen(src, "r");
    if (source_file == NULL) {
        printf("Error al abrir el archivo fuente '%s': %s\n", src, strerror(errno));
        return -1;
    }

    FILE *dest_file = fopen(dest, "w");
    if (dest_file == NULL) {
        printf("Error al crear el archivo destino '%s': %s\n", dest, strerror(errno));
        fclose(source_file);
        return -1;
    }

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }

    fclose(source_file);
    fclose(dest_file);
    return 0;
}

// Función para añadir el archivo al index
int add_to_index(const char *filename) {
    FILE *index_file = fopen(INDEX_FILE, "a");
    if (index_file == NULL) {
        printf("Error al abrir el archivo índice '%s': %s\n", INDEX_FILE, strerror(errno));
        return -1;
    }

    fprintf(index_file, "%s\n", filename);
    fclose(index_file);
    return 0;
}

// Función principal para el comando "ugit add"
int ugit_add(const char *filename) {
    // Verificar si el archivo existe
    if (!file_exists(filename)) {
        printf("Error: El archivo '%s' no existe.\n", filename);
        return -1;
    }

    // Copiar el archivo al área de preparación (stage)
    char staged_file[256];
    snprintf(staged_file, sizeof(staged_file), "%s/%s", STAGING_DIR, filename);

    if (copy_file(filename, staged_file) != 0) {
        return -1;
    }

    // Añadir el archivo al index
    if (add_to_index(filename) != 0) {
        return -1;
    }

    printf("Archivo '%s' añadido al área de preparación.\n", filename);
    return 0;
}

// Main que maneja los comandos "init" y "add"
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Debes proporcionar un comando. Uso: ./ugit <comando> [archivo]\n");
        return 1;
    }

    // Comprobamos si el comando es "init"
    if (strcmp(argv[1], "init") == 0) {
        return ugit_init();  // Ejecuta la función init
    }
    // Comprobamos si el comando es "add" y hay un archivo
    else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Error: Debes proporcionar un archivo para añadir. Uso: ./ugit add <archivo>\n");
            return 1;
        }
        return ugit_add(argv[2]);  // Ejecuta la función add con el archivo
    } else {
        printf("Error: Comando no reconocido '%s'.\n", argv[1]);
        return 1;
    }
}
