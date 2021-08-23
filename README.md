# 42 Intra profiler

[![Build](https://github.com/42sp/42labs-selection-process-paulo-santana/actions/workflows/build.yml/badge.svg)](https://github.com/42sp/42labs-selection-process-paulo-santana/actions/workflows/build.yml)
[![Docker Image CI](https://github.com/42sp/42labs-selection-process-paulo-santana/actions/workflows/docker-image.yml/badge.svg)](https://github.com/42sp/42labs-selection-process-paulo-santana/actions/workflows/docker-image.yml)

Esta é a minha solução do desafio 42 Labs: uma API REST escrita em C :).

## Utilização

### GET `/api/v1/{login}`

Procura por dados do usuário identificado pelo campo `{login}` na Intranet
da 42 e retorna informações relevantes conforme o JSON abaixo:

```json
{
  "intra_id": 86667,
  "login": "psergio-",
  "first_name": "Paulo",
  "url": "https://api.intra.42.fr/v2/users/psergio-",
  "image_url": "https://cdn.intra.42.fr/users/default.png",
  "pool_month": "march",
  "pool_year": "2021",
  "correction_points": 7,
  "wallet": 178,
  "is_staff": false,
  "current_project": "so_long",
  "avg_days_project": 21 // media de dias por projeto
}
```

## Instalação

### Compilando os fontes

Para compilar o código fonte da API, você precisa instalar as seguintes
dependências:

- GCC
- Make
- libmbedtls
- libmongoc
- libbson

As dependências podem ser instaladas com o seguinte comando em um Ubuntu (Focal):
```shell
$ sudo apt update && sudo apt install gcc make libmongoc-1.0-0 libmongoc-dev libmbedtls-dev
```

Com as dependências instaladas, entre no diretório do projeto e execute
```shell
$ make
```
para compilar o projeto.

### Iniciando o servidor

O servidor da API pode ser executado diretamente pelo seu binário ou através da
regra `run` do Makefile

```shell
$ ./intra_profiler
# ou
$ make run
```

O servidor utiliza credenciais em variáveis de ambiente para acessar a intranet
da 42 e o servidor de banco de dados. Essas variáveis estão listadas no arquivo
.env.example

> Dica: Você pode atribuir a uma variável de ambiente `$PORT` um número de
porta em que a API ficará escutando

### Docker

Para utilizar o docker, utilize a regra `docker` do Makefile

```shell
$ make docker
```

Isto irá gerar uma imagem chamada `intra-profiler` e instanciará um container
a partir dessa imagem, com o mesmo nome.
