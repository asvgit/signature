[![C/C++ CI](https://github.com/asvgit/signature/workflows/C/C++%20CI/badge.svg)](https://github.com/asvgit/signature/actions?query=workflow%3A%22C%2FC%2B%2B+CI%22)

# Signature

Программа для генерации сигнатуры указанного файла.
Используемый для расчёта хэша: CRC32.

## Аргументы программы signature

**help (h)** - Показать справочную информацию

**input-file (i)** - Путь до входного файла

**output-file (o)** - Путь до выходной файла

**block-size (b)** - Размер блока (по умолчанию, 1 Мб)

Также используется задание аргументов без использования без указания опций:

```
signature ./inp.txt ./out.txt 7
```
