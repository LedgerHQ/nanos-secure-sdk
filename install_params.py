"""
/*******************************************************************************
 *   Ledger - Secure firmware
 *   (c) 2023 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/
"""

import struct
import argparse
import binascii


BOLOS_TAG_APPNAME = 1
BOLOS_TAG_APPVERSION = 2
BOLOS_TAG_ICON = 3
BOLOS_TAG_DERIVEPATH = 4
BOLOS_TAG_DEPENDENCY = 6


CURVE_SECP256K1 = 1 << 0
CURVE_PRIME256R1 = 1 << 1
CURVE_ED25519 = 1 << 2
CURVE_SLIP21 = 1 << 3
CURVE_BLS12381G1 = 1 << 4


def auto_int(x):
    return int(x, 0)


def string_to_bytes(x):
    return bytes(x, 'ascii')


def encodelv(value):
    length = len(value)
    res = b""
    if length < 128:
        res += struct.pack(">B", length)
    elif length < 256:
        res += struct.pack(">B", 0x81)
        res += struct.pack(">B", length)
    elif length < 65536:
        res += struct.pack(">B", 0x82)
        res += struct.pack(">H", length)
    else:
        raise Exception("Unimplemented LV encoding")
    res += value
    return res


def encodetlv(tag, value):
    res = struct.pack(">B", tag)
    res += encodelv(value)
    return res


def get_curve_mask(curves, slip21_paths):
    if curves:
        curve_mask = 0x00
        for curve in curves:
            if curve == 'secp256k1':
                curve_mask |= CURVE_SECP256K1
            elif curve == 'secp256r1':
                curve_mask |= CURVE_PRIME256R1
            elif curve == 'ed25519':
                curve_mask |= CURVE_ED25519
            elif curve == 'bls12381g1':
                curve_mask |= CURVE_BLS12381G1
            else:
                raise Exception("Unknown curve " + curve)

        if slip21_paths:
            curve_mask |= CURVE_SLIP21
    else:
        curve_mask = 0xff

    return curve_mask


def parse_bip32_path(path):
    elements = path.split('/')
    result = struct.pack('>B', len(elements))
    for element in elements:
        if element.endswith("\'"):
            value = 0x80000000 | int(element[:-1])
        else:
            value = int(element)
        result += struct.pack(">I", value)
    return result


def parse_slip21_path(path):
    result = struct.pack('>B', 0x80 | (len(path) + 1))
    result = result + b'\x00' + string_to_bytes(path)
    return result


def get_serialized_path(curves, bip32_paths, slip21_paths):
    curve_mask = get_curve_mask(args.curve, args.path_slip21)
    serialized_path = struct.pack('>B', curve_mask)

    serialized_bip32_paths = b""
    if bip32_paths:
        for path in bip32_paths:
            if path:
                serialized_bip32_paths += parse_bip32_path(path)
    serialized_path += serialized_bip32_paths

    serialized_slip21_path = b""
    if slip21_paths:
        for path in slip21_paths:
            if path:
                serialized_slip21_path += parse_slip21_path(path)
        if not serialized_bip32_paths:
            # Unrestricted, authorize all paths for regular derivation
            serialized_slip21_path += struct.pack('>B', 0)
    serialized_path += serialized_slip21_path

    return serialized_path


def get_argparser():
    parser = argparse.ArgumentParser(description="Generate application install_params TLV bytes.")
    parser.add_argument("--appName", help="The name to give the application after loading it", required=True)
    parser.add_argument("--appVersion", help="The application version (as a string)")
    parser.add_argument("--icon", help="The icon content to use (hex encoded)")
    parser.add_argument("--curve", help="""A curve on which BIP 32 derivation is locked ("secp256k1", "secp256r1",
"ed25519" or "bls12381g1"), can be repeated""", action='append')
    parser.add_argument("--path", help="""A BIP 32 path to which derivation is locked (format decimal a'/b'/c), can be
repeated""", action='append')
    parser.add_argument("--path_slip21", help="""A SLIP 21 path to which derivation is locked""", action='append')
    parser.add_argument("--tlvraw", help="Add a custom install param with the hextag:hexvalue encoding", action='append')
    parser.add_argument("--dep", help="Add a dependency over an appname[:appversion]", action='append')

    return parser


if __name__ == '__main__':

    args = get_argparser().parse_args()

    # Build install parameters
    install_params = b""

    # express dependency
    if args.dep:
        for dep in args.dep:
            app_name, app_version = dep, None

            # split if version is specified
            if ":" in dep:
                app_name, app_version = dep.split(":")

            dep_value = encodelv(string_to_bytes(app_name))
            if app_version:
                dep_value += encodelv(string_to_bytes(app_version))
            install_params += encodetlv(BOLOS_TAG_DEPENDENCY, dep_value)

    # Add raw install parameters as requested
    if args.tlvraw:
        for tlvraw in args.tlvraw:
            hextag, hexvalue = tlvraw.split(":")
            install_params += encodetlv(int(hextag, 16), binascii.unhexlify(hexvalue))

    # App name is mandatory
    install_params += encodetlv(BOLOS_TAG_APPNAME, string_to_bytes(args.appName))

    if args.appVersion:
        install_params += encodetlv(BOLOS_TAG_APPVERSION, string_to_bytes(args.appVersion))

    if args.icon:
        install_params += encodetlv(BOLOS_TAG_ICON, bytes.fromhex(args.icon))

    serialized_path = get_serialized_path(args.curve, args.path, args.path_slip21)
    if len(serialized_path) > 0:
        install_params += encodetlv(BOLOS_TAG_DERIVEPATH, serialized_path)

    output = ",".join(f"0x{i:02x}" for i in install_params)
    print(output)
