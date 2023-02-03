# Ledger Secure SDK

## Are you developing an application?

If you are developing an application, for a smooth and quick integration:
- See the developers’ documentation on the [Developer Portal](https://developers.ledger.com/)
- [Go on Discord](https://developers.ledger.com/discord-pro/) to chat with developer support and the developer community.

## Introduction

This directory contains the SDK for Nano S, Nano X, Nano S+ and Stax applications development.

It is the unified version of the previous Nano S, Nano X, Nano S+ SDKs.

As the previous SDKs, this SDK is tightly linked to the Ledger Hardware Wallet OS: BOLOS.

Indeed, it allows to interact with `syscalls` and `cxlib` functions which are embedded in the OS.

Hence you should make sure to use the right SDK version matching your development device OS.

You can find below two possibilities to build against the right SDK version.

## Using the docker image

The easiest way to build against last device OS is to use the docker image provided by ledger and accessible on ghcr.io.

The corresponding GIT repository can be found [here](https://github.com/LedgerHQ/ledger-app-builder/). Please have a look at its `README.md` for information about its usage.

## Using the SDK directly (advanced users)

Advanced users that have setup the Ledger development environment can use this repository to build apps.

But for that, they need to understand how OS and SDK compatibility are tracked. This is done with the `API_LEVEL` which is defined in `Makefile.defines`.

The `API_LEVEL` on `master` branch is kept as the reserved value `0`.

For each released OS there is a corresponding tag in the format `<Device>_<os_version>`, e.g. `NanoX_v2.1.0` for the release of the OS version `v2.1.0` for Nano X device. While on this tag, if you look at the value of the `API_LEVEL` which is defined in `Makefile.defines` you will retrieve the OS `API_LEVEL`.

There are also `API_LEVEL_<N>` branches with `API_LEVEL` value set to `N`. Their purpose is to allow cherry-picks of bug fixes and improvements that are merged on `master` so that they are available when building the apps for the corresponding OS.

On these `API_LEVEL_<N>` branches, there are tags following the format `v<N>.<minor>.<patch>`, e.g. `v1.1.0` where `N` is the `API_LEVEL`. These tags are used to generate the `SDK_VERSION` which is available at compile time and allows to track the SDK version used to build an app.

In short, to build an app for an OS, you should:
- Retrieve the OS `API_LEVEL`:
    - `git checkout <device>_<os_version>`
    - `grep API_LEVEL Makefile.defines | head -n1`
- Check out the `API_LEVEL_<N>` branch related to the OS `API_LEVEL` and make sure it is up to date:
    - `git checkout API_LEVEL_<N>`
    - `git pull`
    - The last commit should be tagged with the complete version of the SDK (`v<N>.<x>.<y>`)
- Build the app from your app folder:
    - `make BOLOS_SDK=<path_to_sdk> TARGET=<target>` where `target` is one of `nanos`, `nanox`, `nanos2`, `stax` (`nanos2` is used for Nano S+ device).

## Contributing

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
