# Fuzzing

## Manual usage based on Ledger container

### Preparation

The fuzzer can run from the docker `ledger-app-builder-legacy`. You can download it from the `ghcr.io` docker repository:

```console
sudo docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

You can then enter this development environment by executing the following command from the repository root directory:

```console
sudo docker run --rm -ti --user "$(id -u):$(id -g)" -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

### Compilation

Once in the container, go into the `fuzzing` folder to compile the fuzzer:

```console
cd fuzzing

# cmake initialization
cmake -B build

# Fuzzer compilation
make -C build
```

### Run

```console
./build/fuzz_apdu_parser
./build/fuzz_base58
./build/fuzz_bip32
./build/fuzz_qrcodegen
```
