#!/bin/bash

# Compilar o programa
make

# Função para gerar matrizes aleatórias
generate_matrix() {
    local size=$1
    local filename=$2
    awk -v size="$size" 'BEGIN { srand(); for(i=0; i<size; i++) { for(j=0; j<size; j++) printf("%d ", int(rand()*10)); printf("\n"); } }' > "$filename"
}

# Gerar matrizes de diferentes tamanhos
generate_matrix 3 "Teste/mat3x3.dat"
generate_matrix 100 "Teste/mat100x100.dat"
generate_matrix 1000 "Teste/mat1000x1000.dat"

# Testar o programa com diferentes configurações de threads e tamanhos de matrizes
for threads in 1 2 4; do
    for size in 3 100 1000; do
        echo "Executando com $threads threads e matrizes de ${size}x${size}"
        ./programa $threads $size "Teste/mat${size}x${size}.dat" "Teste/mat${size}x${size}.dat" "Teste/mat${size}x${size}.dat" "Teste/SaidaD.dat" "Teste/SaidaE.dat"
        echo ""
    done
done

