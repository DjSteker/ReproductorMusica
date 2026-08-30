# 🎵 ReproductorMusica

Un reproductor de música moderno y funcional desarrollado en C++ para Linux, con interfaz gráfica GTK+ y soporte para audio ALSA.

---

## ✨ Características

### Reproductor de Audio
- ✅ Reproducción de archivos WAV con calidad sin pérdidas
- ✅ Control de reproducción (play, pause, stop)
- ✅ Ajuste de volumen en tiempo real
- ✅ Barra de progreso interactiva
- ✅ Soporte para múltiples dispositivos de audio ALSA
- ✅ Callbacks de progreso para actualización de interfaz

### Gestión de Playlists
- 📋 Crear y eliminar listas de reproducción
- 🎵 Agregar canciones a playlists
- ⭐ Sistema de calificación por playlist (1-5 estrellas)
- ❤️ Marcar canciones como favoritas
- 🔄 Modo aleatorio (shuffle)
- 🔁 Modo de repetición

### Interfaz de Usuario
- 🖥️ Interfaz gráfica moderna con GTK+
- 🌍 Soporte para múltiples idiomas (localización)
- 📁 Carga de archivos individuales y carpetas
- 🎚️ Selector visual de dispositivos de audio
- 🎡 Indicador de canción actual en reproducción

### Persistencia de Datos
- 💾 Almacenamiento de playlists en XML
- 🔐 Base de datos thread-safe con mutex
- 📊 Preservación de configuración entre sesiones

---

## 📦 Requisitos

### Dependencias del Sistema
```bash
# GTK+ 4.x
sudo apt-get install libgtk-4-dev

# ALSA (Advanced Linux Sound Architecture)
sudo apt-get install libasound2-dev

# TinyXML2 (para XML parsing)
sudo apt-get install libtinyxml2-dev

# GCC/G++ compiler
sudo apt-get install build-essential

# Git
sudo apt-get install git
