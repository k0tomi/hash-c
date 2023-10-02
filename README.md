# SHA256 C Cracker
Um programa simples de crackear SHA256 em C utilizando o OpenSSL
Testado e compilado com o GCC 12.2.0
# Como hash cracking funciona
O método utilizado para crackear hashes é:
1. Obter a hash a ser crackeada
2. Identificar o tipo de encripção da hash
    -Nesse caso assumimos sempre SHA256
3. Adquirir plaintext
4. Passar o plaintext pelo algorítmo de encripção
5. Comparar o digest com a hash a ser crackeada

Dentro desse processo, existem variações entre salt, strings, algorítmo de hashing, entre outros, tornando o processo intenso em processamento, e um excelente candidato a otimização, paralelização, e utilização da GPU.

# Obejtivos e problemas do programa
O objetivo principal e inicial para funcionalidade do programa é fazer um algorítmo de hashing, nesse caso o SHA256.
O programa foi projetado para funcionar com um modo de ataque único, um "dictionary attack", que consiste de comparar o digest de todas as palavras em uma determinada wordlist com a hash a ser crackeada.

A implementação em C foi especialmente sofrida. O C tem uma curva de aprendizado extremamente alta, mas dá resultados excelentes caso dominado. No meu caso, tal curva acabou desacelerando consideravelmente o programa.

Um dos maiores desafios encontrados foi trabalhar com o string terminator do C. Por não ter um tipo definido, e todas as funções relacionadas a string necessitarem do string terminator no final para serem operáveis com tais funções e consideradas strings, criou um problema imenso.
Outro desafio foi a presença de junk na memória. Como estou lidando com alocação dinâmica e encripção, um bit individual errado pode gerar uma hash completamente diferente após a função ser executada; e ao mesmo tempo, funções como `calloc()` e qualquer coisa que inicializa o espaço de memória não podem ser utilizadas, já que o espaço inicializado também consta como algo a ser lido e processado pelo algorítmo de encripção.

Dado as restrições e a necessidade de eliminar qualquer chance de junk na memória, a solução que encontrei foi:
1. Abrir a wordlist
2. Verificar quantas palavras ela tem
3. Alocar a primeira dimensão do buffer, o número de palavras
4. Percorrer a wordlist novamente, pegando o charcount de cada palavra
    - Durante esse processo, alocar a memória necessária para guardar exatamente cada char da palavra, sem um string terminator ou espaço em excesso para evitar junk, evitando interferências na função de encryption
5. Percorrer a wordlist uma terceira vez, colocando cada char individualmente em seu respectivo espaço alocado na array
    - Durante esse processo, ao terminar de colocar uma palavra no array, passar ela pelo algorítmo de hashing e comparar com a hash a ser crackeada. Em caso de falha continuar, em caso de sucesso interromper qualquer loop em andamento e retornar a versão plaintext da hash a ser crackeada

# Próximos passos
- Recode completo em C++
- Implementação na GPU
- Adição de uma segunda wordlist
- Implementação de ataque combinatório
    - O ataque combinatório usa as palavras da wordlist 2, ou 1 e 2, concatenando-as no final ou começo do plaintext. Utilizado para tentar crackear salted hashes (hashsalt ou salthash nesse caso, sendo o "salt" uma string aleatória, e "hash" o plaintext 

