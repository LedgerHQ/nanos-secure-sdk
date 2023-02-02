# Ledger Secure SDK

## Are you developing an application?

If you are developing an application, for a smooth and quick integration:
- See the developers’ documentation on the [Developer Portal](https://developers.ledger.com/)
- [Go on Discord](https://developers.ledger.com/discord-pro/) to chat with developer support and the developer community.

## Introduction

This directory contains the SDK for Nanos, Nanox, Nanos+ and Stax applications development.

It is the unified version of the previous Nanos, Nanox, Nanos+ SDKs.

As the previous SDKs, this SDK is tightly linked to the Ledger Hardware Wallet OS: BOLOS.

Indeed, it allows to interact with `syscalls` and `cxlib` functions which are embedded in the OS.

Hence you should make sure to use the right SDK version matching your development device OS.

You can find below to possibilities to build against the right SDK version.

## Using the docker image

The easiest way to do build against last device OS is to use the docker image provided by ledger and accessible on ghcr.io.

The corresponding GIT repository can be find [here](https://github.com/LedgerHQ/ledger-app-builder/). Please have a look at it's `Readme` for information about its usage.

## Using the SDK directly (advanced users)

Advanced users that have setup the Ledger development environment can use this repository to build apps.

But for that, they need to understand how OS and SDK compatibility are tracked. This is done with the `API_LEVEL` which is defined in `Makefile.defines`.

The `API_LEVEL` on `master` branch is kept as the reserved value `0`.

For each released OS there is a corresponding tag in the format `<device>_<os_version>`, e.g. `nanox_2.1.0` for the release of the OS version `2.1.0` for Nano X device. While on this tag, if you look at the value of the `API_LEVEL` which is defined in `Makefile.defines` you will retrieve the OS `API_LEVEL`.

There are also `API_LEVEL_<N>` branches with `API_LEVEL` value set to `N`. Theirs purpose is to allow cherry-picks of bug fixes and improvements that are merged on `master` so that they are available when building the apps for the corresponding OS.

On these `API_LEVEL_<N>` branches, there are tags following the format `v<N>.<minor>.<patch>`, e.g. `v1.1.0` where `N` is the `API_LEVEL`. These tags are used to generate the `SDK_VERSION` which is available at compile time and allows to tracks the SDK version used to build an app.

In short, to build an app for an OS, you should:
- Retrieve the OS `API_LEVEL`: `git checkout <device>_<os_version>` and then `grep API_LEVEL Makefile.defines | head -n1`
- Use the last tag (`v<N>.<x>.<y>`) of the corresponding `API_LEVEL_<N>` branch, which should be the same commit than the head of the `API_LEVEL_<N>` branch: `git checkout API_LEVEL_<N>`
- Build the app from app folder with `make BOLOS_SDK=<path_to_sdk> TARGET=<target>` where `target` is one of `nanos`, `nanox`, `nanos2`, `stax`.

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
