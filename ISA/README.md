# Filtering DNS resolver

## November 2025

### Description

The `dns` program filters incoming type A queries based on a predefined list of
domains and their subdomains. Queries for blocked domains are refused. All other
queries are forwarded to a specified resolver, and the received responses are
returned to the original requester.

### Usage

```sh
make
dns -s server [-p port] -f filter_file [-v]
```

Options:

- `-s`: IP address of the upstream DNS resolver.
- `-p` port: Port for listening to incoming DNS queries.
- `-f` filter_file: Path to the file containing blocked domains.
- `-v`: Verbose output mode.

### Tests

```sh
make
./run_tests
```
