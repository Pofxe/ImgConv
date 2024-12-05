# Используем последнюю версию Ubuntu
FROM ubuntu:latest

# Обновляем систему и устанавливаем необходимые пакеты
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \ 
    && rm -rf /var/lib/apt/lists/*  # Очищаем кэш пакетов

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем содержимое текущей директории в рабочую директорию контейнера
COPY . /app

# Создаем директорию для сборки
RUN mkdir build

# Переходим в директорию для сборки
WORKDIR /app/build

# Запускаем CMake для настройки проекта
RUN cmake ..

# Компилируем проект
RUN make