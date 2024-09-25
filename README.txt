µGit - Simulador de Control de Versiones
µGit es una simulación básica de un sistema de control de versiones que emula funcionalidades básicas de Git. Está desarrollado en C y permite realizar operaciones como init, add, commit, log, y checkout para gestionar el historial de versiones de un proyecto.

Características
init: Inicializa un nuevo repositorio µGit.
add: Añade archivos al área de preparación (staging area).
commit: Crea un commit que captura el estado actual del proyecto.
log: Muestra el historial de commits.
checkout: Permite cambiar a una versión anterior del proyecto utilizando un identificador de commit.

Requisitos
Este proyecto está implementado en C, por lo que se necesita un compilador de C, como gcc, para compilar el código.

Instalación
Clona o descarga el repositorio.

Compila el archivo ugit.c con el siguiente comando: gcc ugit.c -o ugit

1. Inicializar un repositorio
Para comenzar a utilizar µGit, se debe inicializar un nuevo repositorio. Esto creará la estructura de archivos y carpetas necesarias para almacenar los commits y los archivos añadidos.

./ugit init
Salida esperada: Repositorio µGit inicializado.

2. Añadir archivos al área de preparación
Para agregar archivos al área de preparación (staging area), se utiliza el comando add seguido del nombre del archivo:

./ugit add archivo.txt
Esto copiará el archivo archivo.txt al área de preparación y lo registrará en el archivo .ugit/index.

Salida esperada: Archivo 'archivo.txt' añadido al área de preparación.

3. Crear un commit
Después de añadir archivos al área de preparación, se puede crear un commit que capture el estado actual de esos archivos.

./ugit commit -m "Mensaje del commit"
Esto creará un commit con un identificador único, almacenará los archivos en un directorio separado y registrará el commit en el archivo .ugit/log.

Salida esperada: Commit realizado con éxito: <commit_id>

4. Ver el historial de commits
Se puede ver el historial de commits ejecutando el comando log:

./ugit log
Esto mostrará una lista de los commits realizados, junto con el identificador del commit, la fecha y el mensaje del commit.

Salida esperada:

Commit: <commit_id>
Fecha: <fecha>
Mensaje: <mensaje>
-----------------------

5. Cambiar a una versión anterior
Se puede volver a un estado anterior del proyecto al usar el comando checkout con el identificador del commit al que deseas cambiar:

./ugit checkout <commit_id>
Esto restaurará los archivos a su estado en el commit especificado, sobrescribiendo los archivos en el directorio de trabajo.

Salida esperada: Cambiaste al commit '<commit_id>'.
