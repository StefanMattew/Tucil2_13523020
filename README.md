# Tucil2_13523020

- Deskripsi Singkat
  Program ini merupakan implementasi kompresi citra menggunakan metode Quadtree dengan menerapkan algoritma Divide and Conquer. Citra akan dibagi menjadi blok-blok, dan jika blok tersebut memiliki ukuran error tertentu dengan metode variance, MAD, MPD, atau entropy , kurang dari nilai threshold , maka blok tidak akan dibagi lagi dan warnanya akan diseragamkan. Tujuan dari program ini adalah mengurangi ukuran file gambar tanpa mengorbankan terlalu banyak kualitas visual.

- Requirement dan instalasi
  * Bahasa pemrograman C
  * Compiler gcc
  * Library yang digunakan : stb_image.h , stb_image_write.h

- Cara kompilasi:
  Dengan menggunakan perintah: "gcc -o main main.c quadtreeCompression.c errMethod.c inputOutput.c -lm"

- Cara menjalankan program
  1. Clone repository ini
  2. Dengan menggunakan terminal pada directory bin dan menjalankan perintah "./main.exe"


- Identitas Pembuat:
    Stefan Mattew Susanto
    13523020
    
