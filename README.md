# ReproductorMusica

Un reproductor de música simple (plantilla) pensado como punto de partida para implementar una aplicación web/desktop/móvil con reproducción local, gestión de listas y soporte de metadatos.

Estado

- Repositorio inicial: actualmente solo contiene la licencia y este README.
- Objetivo: implementar un reproductor de música con gestión de listas de reproducción, reproducción sin pausas, lectura de metadatos y una UI sencilla y accesible.

Características propuestas

- Reproducción local de archivos (MP3, WAV, OGG)
- Soporte para listas de reproducción (.m3u, .pls)
- Lectura de metadatos (ID3 tags)
- Controles básicos: reproducir, pausar, detener, siguiente/anterior, buscar (seek), control de volumen
- Normalización de volumen y opción de ecualizador (opcional)
- Interfaz accesible y soporte para atajos de teclado
- Guardado y carga de listas de reproducción
- Soporte multiplataforma y empaquetado (si aplica)

Requisitos (ejemplo)

- Node.js >= 18 (para la versión web/desktop basada en Electron o frameworks similares)
- npm o yarn
- Herramientas de compilación para empaquetado nativo (opcional)

Instalación (ejemplo para versión web)

1. Clonar el repositorio:

   git clone https://github.com/DjSteker/ReproductorMusica.git
2. Instalar dependencias:

   npm install
3. Ejecutar en desarrollo:

   npm run dev

(Actualiza estos pasos cuando se añadan scripts reales al proyecto.)

Estructura de proyecto recomendada

- src/ - código fuente (frontend o backend)
  - assets/ - imágenes, iconos
  - components/ - componentes UI
  - styles/ - CSS/SCSS
  - lib/ - utilidades (p. ej. lectura de metadatos, audio manager)
- public/ - archivos estáticos (index.html)
- tests/ - pruebas unitarias y de integración
- scripts/ - scripts de build/deploy
- README.md - este archivo
- LICENSE - licencia del proyecto

Buenas prácticas y sugerencias (cuando haya código)

- Añadir linting y formateo (ESLint + Prettier) y configuración para TypeScript si se usa.
- Escribir pruebas unitarias y de integración (Jest, Vitest o similares).
- Añadir GitHub Actions para CI: ejecutar linters y tests en cada PR.
- Separar la lógica de reproducción del UI (p. ej. un servicio/audio-manager) para facilitar pruebas y mantenimiento.
- Manejar errores y excepciones (archivo corrupto, codecs no soportados) y mostrar mensajes claros al usuario.
- Evitar fugas de memoria por listeners de audio: limpiar eventos al desmontar componentes.
- Documentar la API interna y los puntos de extensión (plugins, skins, backends).
- Añadir pruebas de accesibilidad automatizadas (axe-core) y auditorías con Lighthouse.

Sugerencias técnicas concretas

- Arquitectura del audio: crear un servicio `AudioManager` que abstraiga la API `HTMLAudioElement` o la API nativa (WebAudio) y exponga métodos: load(), play(), pause(), stop(), seek(), setVolume(), next(), prev(), setPlaylist(). Esto permite tests unitarios y separación de UI.
- Metadatos: usar bibliotecas como `music-metadata` (Node) para extracción de tags en backend/desktop y `jsmediatags` en navegador si fuera necesario.
- Listas de reproducción: persistir en IndexedDB (web) o en archivos JSON en desktop. Soportar import/export .m3u simple.
- Crossfade / gapless: considerar WebAudio API y buffers para reproducir sin cortes entre pistas.
- CI/CD: configurar GitHub Actions con jobs para lint, tests y build. Añadir badge en README.

Roadmap sugerido (prioridades)

1. Crear estructura de proyecto y esqueleto (index.html + app entry).
2. Implementar `AudioManager` y reproducir archivos locales usando `HTMLAudioElement`.
3. Añadir UI básica con controles y lista de reproducción.
4. Implementar lectura de metadatos y mostrar carátula/título/artista.
5. Tests básicos y configuración de CI.
6. Funciones avanzadas: crossfade, ecualizador, normalización, streaming.

Cómo contribuir

- Haz un fork y envía Pull Requests con descripciones claras.
- Abre issues para reportar bugs o proponer mejoras.
- Sigue las convenciones de estilo y añade tests para cambios importantes.

Licencia

Este proyecto usa GNU General Public License v2.0 (ver archivo LICENSE).

Contacto

- Autor: @DjSteker (GitHub)

Próximos pasos que puedo hacer por ti

- Crear un esqueleto inicial (index.html, CSS mínimo y un simple reproductor con HTML5 Audio + un `AudioManager` en JS) y hacer push al repositorio.
- Añadir configuración básica de ESLint + Prettier y un workflow de GitHub Actions para CI.
- Generar plantillas de issues/PR y archivos CONTRIBUTING.md.

Si quieres que implemente alguno de los puntos anteriores, dime cuál y lo crearé en una rama y abriré un PR.
