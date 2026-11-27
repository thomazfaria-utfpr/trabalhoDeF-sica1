# trabalhoDeF-sica1
Trabalho de colisões requisitado pelo professor Rafael Barreto para a turma de engenharia de computação 2026.2 da UTFPR.

Thomaz Miguel Biernaski Faria ; RA: 2939207 

  Optei por fazer o trabalho usando C e a biblioteca para imagem SDL2 (por ser uma das mais comuns, famosas e fáceis de fazer). O projeto foi feito ao longo de +- 2 dias, com minhas ideias iniciais sendo colocadas no papel e pré "codadas" no primeiro dia, e no segundo dia com a assistência de agentes de IA fiz a refinação do meu código e implementação de mais features que pensei (com ajuda do meu tio) de um dia para o outro.
  O uso dos agentes de IA foi feito apenas para tornar mais fácil a implementação de features, correção de bugs e eficientização do código, toda a estrutura base foi desenvolvida por mim, assim como o meio de funcionamento do código (com exceção de alguns detalhes que eu não tinha domínio sobre, como a implementação da biblioteca de imagem, suas funcionalidades e o uso do tipo static nas funções).
  No título da janela do programa é possível ver a energia cinética total, que é constante e o momento linear total das bolas, 

 Simulador de colisões de bolas de sinuca.
 
Compilar (Linux / MSYS2 / MinGW):
  gcc colisoes.c -o colisoes $(sdl2-config --cflags --libs) -lm
 
Executar (todos os argumentos são opcionais):
  ./colisoes [N] [largura] [altura] [vel_media] [dt] [raio]
  ./colisoes 30 900 600 200 0.002 15
  use R para reiniciar e esc para sair do programa.
 
   Unidades: pixels e segundos. Todas as bolas têm massa 1,
   e as colisões são perfeitamente elásticas.
