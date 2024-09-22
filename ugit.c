#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

#define STAGING_DIR ".ugit/stage"
#define INDEX_FILE ".ugit/index"
#define COMMITS_DIR ".ugit/commits"
#define HEAD_FILE ".ugit/HEAD"
#define LOG_FILE ".ugit/log"

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
    if (create_directory(".ugit") != 0) return -1;
    if (create_directory(STAGING_DIR) != 0) return -1;
    if (create_directory(COMMITS_DIR) != 0) return -1;

    // Crear archivos necesarios
    if (create_file(HEAD_FILE, "master") != 0) return -1;
    if (create_file(LOG_FILE, "") != 0) return -1;
    if (create_file(INDEX_FILE, "") != 0) return -1;

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
    char staged_file[512];
    snprintf(staged_file, sizeof(staged_file), "%s/%s", STAGING_DIR, filename);

    if (copy_file(filename, staged_file) != 0) return -1;

    // Añadir el archivo al index
    if (add_to_index(filename) != 0) return -1;

    printf("Archivo '%s' añadido al área de preparación.\n", filename);
    return 0;
}

// Función para generar un identificador único (simple hash basado en timestamp)
char *generate_commit_id() {
    time_t now = time(NULL);
    char *commit_id = malloc(20);  // Asegúrate de liberar esta memoria después
    if (commit_id) {
        snprintf(commit_id, 20, "%ld", now);  // Usa el timestamp como ID
    }
    return commit_id;
}

// Función para copiar archivos del stage a un nuevo directorio del commit
int copy_staged_files_to_commit(const char *commit_id) {
    char commit_dir[512];
    snprintf(commit_dir, sizeof(commit_dir), "%s/%s", COMMITS_DIR, commit_id);

    // Crear el directorio del commit
    if (mkdir(commit_dir, 0700) != 0) {
        printf("Error al crear el directorio del commit '%s': %s\n", commit_dir, strerror(errno));
        return -1;
    }

    // Abrir el archivo index para obtener los archivos a copiar
    FILE *index_file = fopen(INDEX_FILE, "r");
    if (index_file == NULL) {
        printf("Error al abrir el archivo índice '%s': %s\n", INDEX_FILE, strerror(errno));
        return -1;
    }

    char filename[256];
    while (fgets(filename, sizeof(filename), index_file) != NULL) {
        // Remover el salto de línea del nombre del archivo
        filename[strcspn(filename, "\n")] = 0;

        // Construir las rutas de los archivos
        char staged_file[512], commit_file[512];
        int n1 = snprintf(staged_file, sizeof(staged_file), "%s/%s", STAGING_DIR, filename);
        int n2 = snprintf(commit_file, sizeof(commit_file), "%s/%s", commit_dir, filename);

        if (n1 >= sizeof(staged_file) || n2 >= sizeof(commit_file)) {
            printf("Error: Ruta de archivo demasiado larga para manejarla.\n");
            return -1;
        }

        // Copiar el archivo al directorio del commit
        if (copy_file(staged_file, commit_file) != 0) return -1;
    }

    fclose(index_file);
    return 0;
}

// Función para registrar el commit en el archivo de log
int log_commit(const char *commit_id, const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        printf("Error al abrir el archivo de log '%s': %s\n", LOG_FILE, strerror(errno));
        return -1;
    }

    time_t now = time(NULL);
    fprintf(log_file, "Commit: %s\n", commit_id);
    fprintf(log_file, "Fecha: %s", ctime(&now));
    fprintf(log_file, "Mensaje: %s\n", message);
    fprintf(log_file, "-----------------------\n");

    fclose(log_file);
    return 0;
}

// Función para actualizar el archivo HEAD
int update_head(const char *commit_id) {
    FILE *head_file = fopen(HEAD_FILE, "w");
    if (head_file == NULL) {
        printf("Error al actualizar el HEAD '%s': %s\n", HEAD_FILE, strerror(errno));
        return -1;
    }

    fprintf(head_file, "%s", commit_id);
    fclose(head_file);
    return 0;
}

// Función principal para el comando ugit commit
int ugit_commit(const char *message) {
    // Generar un commit_id único
    char *commit_id = generate_commit_id();
    if (commit_id == NULL) {
        printf("Error al generar el ID del commit.\n");
        return -1;
    }

    // Copiar los archivos del stage al commit
    if (copy_staged_files_to_commit(commit_id) != 0) {
        free(commit_id);  // Liberar la memoria asignada a commit_id
        return -1;
    }

    // Registrar el commit en el log
    if (log_commit(commit_id, message) != 0) {
        free(commit_id);
        return -1;
    }

    // Actualizar el HEAD al nuevo commit
    if (update_head(commit_id) != 0) {
        free(commit_id);
        return -1;
    }

    printf("Commit realizado con éxito: %s\n", commit_id);
    free(commit_id);  // Liberar la memoria asignada a commit_id
    return 0;
}

// Función para mostrar el log de commits
int ugit_log() {
    FILE *log_file = fopen(LOG_FILE, "r");
    if (log_file == NULL) {
        printf("Error al abrir el archivo de log '%s': %s\n", LOG_FILE, strerror(errno));
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), log_file)) {
        printf("%s", line);  // Imprimir cada línea del archivo de log
    }

    fclose(log_file);
    return 0;
}

// Función para cambiar a un commit específico
int ugit_checkout(const char *commit_id) {
    char commit_dir[512];
    snprintf(commit_dir, sizeof(commit_dir), "%s/%s", COMMITS_DIR, commit_id);

    // Verificar si el commit existe
    if (!file_exists(commit_dir)) {
        printf("Error: El commit '%s' no existe.\n", commit_id);
        return -1;
    }

    // Abrir el archivo index para obtener los archivos a restaurar
    FILE *index_file = fopen(INDEX_FILE, "r");
    if (index_file == NULL) {
        printf("Error al abrir el archivo índice '%s': %s\n", INDEX_FILE, strerror(errno));
        return -1;
    }

    char filename[512];
    while (fgets(filename, sizeof(filename), index_file) != NULL) {
        // Remover el salto de línea del nombre del archivo
        filename[strcspn(filename, "\n")] = 0;

        // Construir las rutas de los archivos
        char commit_file[512], working_file[512];
        int n1 = snprintf(commit_file, sizeof(commit_file), "%s/%s", commit_dir, filename);
        int n2 = snprintf(working_file, sizeof(working_file), "%s", filename);

        if (n1 >= sizeof(commit_file) || n2 >= sizeof(working_file)) {
            printf("Error: Ruta de archivo demasiado larga para manejarla.\n");
            return -1;
        }

        // Restaurar el archivo del commit al directorio de trabajo
        if (copy_file(commit_file, working_file) != 0) return -1;
    }

    fclose(index_file);
    printf("Cambiaste al commit '%s'.\n", commit_id);
    return 0;
}

// Main que maneja los comandos "init", "add", "commit", "log" y "checkout"
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Debes proporcionar un comando. Uso: ./ugit <comando> [archivo/mensaje]\n");
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
    }

    // Comprobamos si el comando es "commit"
    else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Error: Debes proporcionar un mensaje de commit. Uso: ./ugit commit \"mensaje\"\n");
            return 1;
        }
        return ugit_commit(argv[2]);  // Ejecuta la función commit con el mensaje
    }

    // Comprobamos si el comando es "log"
    else if (strcmp(argv[1], "log") == 0) {
        return ugit_log();  // Ejecuta la función log para mostrar los commits
    }

    // Comprobamos si el comando es "checkout" y se proporciona un commit_id
    else if (strcmp(argv[1], "checkout") == 0) {
        if (argc < 3) {
            printf("Error: Debes proporcionar un commit_id. Uso: ./ugit checkout <commit_id>\n");
            return 1;
        }
        return ugit_checkout(argv[2]);  // Ejecuta la función checkout con el commit_id
    }

    else {
        printf("Error: Comando no reconocido '%s'.\n", argv[1]);
        return 1;
    }
}
