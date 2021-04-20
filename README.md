<p align="center">
  <img src="https://www.tutorialsdaddy.com/wp-content/uploads/2016/11/linux-mmap.png">
</p>

# _MMAP_

## Tópicos
* [Introdução](#introdução)
* [Implementação](#implementação)
* [launch_processes](#launch_processes)
* [button_interface](#button_interface)
* [led_interface](#led_interface)
* [Compilando, Executando e Matando os processos](#compilando-executando-e-matando-os-processos)
* [Compilando](#compilando)
* [Clonando o projeto](#clonando-o-projeto)
* [Selecionando o modo](#selecionando-o-modo)
* [Modo PC](#modo-pc)
* [Modo RASPBERRY](#modo-raspberry)
* [Executando](#executando)
* [Interagindo com o exemplo](#interagindo-com-o-exemplo)
* [MODO PC](#modo-pc-1)
* [MODO RASPBERRY](#modo-raspberry-1)
* [Matando os processos](#matando-os-processos)
* [Conclusão](#conclusão)
* [Referência](#referência)

## Introdução
O _mmap_ cria um novo mapeamento de memória virtual, que pode ser de dois tipos: Mapeamento de arquivo, onde é necessário um arquivo para poder mapear essa memória, e o mapeamento anônimo, nesse caso não é necessário um arquivo para realizar o mapeamento. O mapeamento pode ser de dois tipos privado ou compartilhado, para que esse recurso possa ser usado na forma de IPC, precisa ser do tipo compartilhado, sendo esse o modo que será abordado no artigo. Se dois processos utilizarem o mesmo arquivo para mapear essa memória com o tipo compartilhado, um processo pode enxergar o que o outro está alterando, permitindo assim a troca de mensagem entre eles.

## Criando um mapeamento de memória
Para realizar a criação desse mapeamento faz-se uso da _system call_ mmap, onde o primeiro argumento representa o endereço onde esse mapeamento vai ser feito, o segundo argumento representa o tamanho dessa memória, o terceiro argumento representa a proteção dessa memória, o quarto o descritor do arquivo usado para esse mapeamento e por fim o quinto é o offset.

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```

para mais informações sobre esse _system call_ utilize o manual do linux
```bash
$ man mmap
```
## Destruindo um mapeamento de memória
Para destruir um mapeamento realizado, é utilizado a _system call munmap_, que libera essa porção de memória alocada.
```c
#include <sys/mman.h>

int munmap(void *addr, size_t length);
```

## Implementação

Para demonstrar o uso desse IPC, iremos utilizar o modelo Produtor/Consumidor, onde o processo Produtor(_button_process_) vai escrever seu estado interno no arquivo, e o Consumidor(_led_process_) vai ler o estado interno e vai aplicar o estado para si. Aplicação é composta por três executáveis sendo eles:
* _launch_processes_ - é responsável por lançar os processos _button_process_ e _led_process_ atráves da combinação _fork_ e _exec_
* _button_interface_ - é reponsável por ler o GPIO em modo de leitura da Raspberry Pi e escrever o estado interno no arquivo
* _led_interface_ - é reponsável por ler do arquivo o estado interno do botão e aplicar em um GPIO configurado como saída

Para facilitar o desenvolvimento da aplicação, as funções pertinentes ao _mmap_ foram abstraídas na forma de biblioteca para que facilite o uso na aplicação.

### Biblioteca mapping
Essa biblioteca é um wrapper para as funções mmap, onde sua inicialização é comum para ambos os processos, para maior praticidade foi encapsulado em duas funções, mapping file e mapping_cleanup.

Essa função retorna o endereço de uma memória compartilhada, par
```c
void *mapping_file(const char *filename, int file_size)

{
    int fd;
    void *file_memory;

    fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    lseek(fd, file_size + 1, SEEK_SET);
    write(fd, "", 1);
    lseek(fd, 0, SEEK_SET);

    /* Create the memory mapping. */
    file_memory = mmap(0, file_size, PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    return file_memory;
}
```

```c
void mapping_cleanup(void *mapping, int file_size)
{
    munmap(mapping, file_size);
}
```

### *launch_processes*

No _main_ criamos duas variáveis para armazenar o PID do *button_process* e do *led_process*, e mais duas variáveis para armazenar o resultado caso o _exec_ venha a falhar.
```c
int pid_button, pid_led;
int button_status, led_status;
```

Em seguida criamos um processo clone, se processo clone for igual a 0, criamos um _array_ de *strings* com o nome do programa que será usado pelo _exec_, em caso o _exec_ retorne, o estado do retorno é capturado e será impresso no *stdout* e aborta a aplicação. Se o _exec_ for executado com sucesso o programa *button_process* será carregado. 
```c
pid_button = fork();

if(pid_button == 0)
{
    //start button process
    char *args[] = {"./button_process", NULL};
    button_status = execvp(args[0], args);
    printf("Error to start button process, status = %d\n", button_status);
    abort();
}   
```

O mesmo procedimento é repetido novamente, porém com a intenção de carregar o *led_process*.

```c
pid_led = fork();

if(pid_led == 0)
{
    //Start led process
    char *args[] = {"./led_process", NULL};
    led_status = execvp(args[0], args);
    printf("Error to start led process, status = %d\n", led_status);
    abort();
}
```

## *button_interface*
descrever o código
## *led_interface*
descrever o código

## Compilando, Executando e Matando os processos
Para compilar e testar o projeto é necessário instalar a biblioteca de [hardware](https://github.com/NakedSolidSnake/Raspberry_lib_hardware) necessária para resolver as dependências de configuração de GPIO da Raspberry Pi.

## Compilando
Para faciliar a execução do exemplo, o exemplo proposto foi criado baseado em uma interface, onde é possível selecionar se usará o hardware da Raspberry Pi 3, ou se a interação com o exemplo vai ser através de input feito por FIFO e o output visualizado através de LOG.

### Clonando o projeto
Pra obter uma cópia do projeto execute os comandos a seguir:

```bash
$ git clone [projeto]
$ cd [projeto]
$ mkdir build && cd build
```

### Selecionando o modo
Para selecionar o modo devemos passar para o cmake uma variável de ambiente chamada de ARCH, e pode-se passar os seguintes valores, PC ou RASPBERRY, para o caso de PC o exemplo terá sua interface preenchida com os sources presentes na pasta src/platform/pc, que permite a interação com o exemplo através de FIFO e LOG, caso seja RASPBERRY usará os GPIO's descritos no [artigo](https://github.com/NakedSolidSnake/Raspberry_lib_hardware#testando-a-instala%C3%A7%C3%A3o-e-as-conex%C3%B5es-de-hardware).

#### Modo PC
```bash
$ cmake -DARCH=PC ..
$ make
```

#### Modo RASPBERRY
```bash
$ cmake -DARCH=RASPBERRY ..
$ make
```

## Executando
Para executar a aplicação execute o processo _*launch_processes*_ para lançar os processos *button_process* e *led_process* que foram determinados de acordo com o modo selecionado.

```bash
$ cd bin
$ ./launch_processes
```

Uma vez executado podemos verificar se os processos estão rodando atráves do comando 
```bash
$ ps -ef | grep _process
```

O output 
```bash
cssouza  16871  3449  0 07:15 pts/4    00:00:00 ./button_process
cssouza  16872  3449  0 07:15 pts/4    00:00:00 ./led_process
```
## Interagindo com o exemplo
Dependendo do modo de compilação selecionado a interação com o exemplo acontece de forma diferente

### MODO PC
Para o modo PC, precisamos abrir um terminal e monitorar os LOG's
```bash
$ sudo tail -f /var/log/syslog | grep LED
```

Dessa forma o terminal irá apresentar somente os LOG's referente ao exemplo.

Para simular o botão, o processo em modo PC cria uma FIFO para permitir enviar comandos para a aplicação, dessa forma todas as vezes que for enviado o número 0 irá logar no terminal onde foi configurado para o monitoramento, segue o exemplo
```bash
colocar comando de interacao de fila
```

Output do LOG quando enviado o comando algumas vezez
```bash
colocar log
```

### MODO RASPBERRY
Para o modo RASPBERRY a cada vez que o botão for pressionado irá alternar o estado do LED.

## Matando os processos
Para matar os processos criados execute o script kill_process.sh
```bash
$ cd bin
$ ./kill_process.sh
```

## Conclusão
Preencher

## Referência
* [Link do projeto completo](https://github.com/NakedSolidSnake/Raspberry_IPC_MMAP)
* [Mark Mitchell, Jeffrey Oldham, and Alex Samuel - Advanced Linux Programming](https://www.amazon.com.br/Advanced-Linux-Programming-CodeSourcery-LLC/dp/0735710430)
* [fork, exec e daemon](https://github.com/NakedSolidSnake/Raspberry_fork_exec_daemon)
* [biblioteca hardware](https://github.com/NakedSolidSnake/Raspberry_lib_hardware)
