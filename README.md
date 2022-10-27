# Ledger Secure SDK

## Development

### Pre-commit

This repository uses [pre-commit](https://pre-commit.com/) to identify simple programming issues at the time of code check-in.

To enable pre-commit in your development environment:

1. Install pre-commit:

    ```shell
    pip install pre-commit
    ```

2. Add pre-commit hooks

    ```shell
    pre-commit install --hook-type pre-commit
    pre-commit install --hook-type commit-msg
    ```
