# parallel-det

Индивидуальное задание Жемелева Г. А. (гр. 13541/3) по дисциплине "Параллельные вычисления", 2018 г.

Программы для вычисления определителя матрицы на языке C++:
- последовательная реализация (Det)
- реализация с использованием потоков Windows (Det_WinThreads)
- реализация с использованием MPI (Det_MPI)

Скрипт matrixGenerator.m (MATLAB) позволяет генерировать файлы, которые подаются на вход программам.

Скрипт stats.py (Python 3.6) позволяет вычислять статистику по итогам многократных запусков этих программ.

catch.hpp - библиотека для модульного тестирования.
