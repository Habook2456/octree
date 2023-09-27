# Instrucciones para Ejecutar el Código

Para ejecutar este programa en un entorno Linux, asegúrate de tener instalado VTK (Visualization Toolkit). A continuación, se describen los pasos para compilar y ejecutar el código:

1. **Instalar VTK**:
   - Asegúrate de tener VTK instalado en tu sistema. Si no lo tienes, puedes instalarlo utilizando tu gestor de paquetes favorito. En sistemas basados en Debian (como Ubuntu), puedes ejecutar el siguiente comando:

     ```
     sudo apt-get install libvtk7-dev
     ```

2. **Compilar el Código**:
   - Abre una terminal y navega hasta la carpeta "source" del proyecto.

   - Ejecuta el siguiente comando para configurar el proyecto con CMake:

     ```
     cmake ..
     ```

   - Luego, utiliza el comando "make" para compilar el código:

     ```
     make
     ```

3. **Ejecutar el Programa**:
   - Una vez que se haya completado la compilación sin errores, puedes ejecutar el programa utilizando el siguiente comando:

     ```
     ./mi_programa
     ```

   Asegúrate de reemplazar "mi_programa" con el nombre del archivo ejecutable generado durante la compilación.

Ahora deberías poder ejecutar el programa con éxito y utilizar las funcionalidades del Octree junto con la visualización de datos en 3D proporcionada por VTK.
