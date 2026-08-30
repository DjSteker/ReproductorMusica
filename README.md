# ReproductorMusica

Un reproductor de música simple (plantilla).

Estado

- Repositorio inicial vacío: actualmente solo contiene la licencia y este README.
- Objetivo: implementar un reproductor de música (web/desktop/móvil) con gestión de listas de reproducción, reproducción sin pausas, soporte de metadatos y UI sencilla.

Características propuestas

- Reproducción local de archivos (MP3, WAV, OGG)
- Soporte para listas de reproducción (.m3u, .pls)
- Lectura de metadatos (ID3 tags)
- Controles básicos: play, pause, stop, siguiente/anterior, seek, volume
- Normalización de volumen y opción de ecualizador (opcional)
- Interfaz accesible y soporte para atajos de teclado
- Guardado y carga de listas de reproducción
- Soporte multiplataforma y empaquetado (si es aplicable)

Requisitos (ejemplo)

- Node.js >= 18 (si se hace versión web con un build system)
- npm o yarn
- Herramientas de compilación para empaquetado nativo (opcional)

Instalación (ejemplo para versión web)

1. Clonar el repositorio:

   git clone https://github.com/DjSteker/ReproductorMusica.git
2. Instalar dependencias:

   npm install
3. Ejecutar en desarrollo:

   npm run dev

(Este apartado debe actualizarse cuando haya código y scripts reales.)

Estructura de proyecto recomendada

- /src - código fuente (frontend o backend)
  - /assets - imágenes, iconos
  - /components - componentes UI
  - /styles - CSS/SCSS
  - /lib - utilidades (p. ej. lectura de metadatos)
- /public - archivos estáticos (index.html)
- /tests - pruebas unitarias y de integración
- /scripts - scripts de build/deploy
- README.md - este archivo
- LICENSE - licencia del proyecto

Buenas prácticas y sugerencias de mejora (para cuando haya código)

- Añadir linting y formateo (ESLint + Prettier) y extras para TypeScript si se usa.
- Escribir pruebas unitarias y de integración (Jest / Vitest para JS; pytest para Python).
- Añadir GitHub Actions para CI: ejecutar linters y tests en cada PR.
- Separar la lógica de reproducción del UI (p. ej. un servicio/audio-manager) para facilitar pruebas.
- Manejar errores y excepciones (archivo corrupto, codecs no soportados) y mostrar mensajes al usuario.
- Optimizar uso de memoria y objetos de audio (evitar fugas de eventos/handlers).
- Medir y optimizar el tiempo de carga para grandes listas de reproducción.
- Documentar la API interna y los puntos de extensión (plugins, skins, backends).
- Soporte para pruebas manuales y automatizadas de accesibilidad (axe-core, Lighthouse).

Roadmap sugerido (prioridades)

1. Crear estructura de proyecto y esqueleto de UI.
2. Implementar reproducción local básica y controles.
3. Añadir soporte de metadatos y listas de reproducción.
4. Tests básicos y CI.
5. Funciones avanzadas: crossfade, ecualizador, normalización, streaming.

Cómo contribuir

- Crear un fork y enviar pull requests con descripciones claras.
- Abrir issues para reportar bugs o proponer mejoras.
- Seguir las convenciones de estilo y añadir tests para cambios nuevos.

Licencia

Este proyecto usa la licencia existente: GNU General Public License v2.0 (ver archivo LICENSE).

Contacto

- Autor: @DjSteker (GitHub)

Notas finales

Si quieres, puedo:

- Proponer una estructura de archivos concreta con ejemplos de componentes.
- Generar un esqueleto inicial (index.html, un simple reproductor con HTML5 Audio y JS) y pusharlo al repositorio.
- Añadir plantillas de issues y PR, y configurar GitHub Actions para CI.

