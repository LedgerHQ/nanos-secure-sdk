#!/usr/bin/env python3

"""

/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2022 Ledger
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

import sys
import argparse
import tomli_w

def auto_int(x):
	return int(x, 0)

def get_argparser():
	parser = argparse.ArgumentParser(description="Load an app onto the device from a hex file.")
	parser.add_argument("-output", help="File where the manifest is saved", required=True)
	parser.add_argument("--targetId", help="The device's target ID (default is Ledger Blue)", type=auto_int, default=0x31000002)
	parser.add_argument("--apiLevel", help="Set the API level of the SDK used to build the app", type=auto_int, default=0)
	parser.add_argument("--fileName", help="The application hex file to be loaded onto the device", required=True)
	parser.add_argument("--curve", help="""A curve on which BIP 32 derivation is locked ("secp256k1", "secp256r1", "ed25519" or "bls12381g1"), can be repeated""", action='append')
	parser.add_argument("--path", help="""A BIP 32 path to which derivation is locked (format decimal a'/b'/c), can be repeated""", action='append')
	parser.add_argument("--appName", help="The name to give the application after loading it", required=True)
	parser.add_argument("--appFlags", help="The application flags", type=auto_int, default=0)
	parser.add_argument("--dataSize", help="The code section's size in the provided hex file (to separate data from code, if not provided the whole allocated NVRAM section for the application will remain readonly.", type=auto_int)
	parser.add_argument("--appVersion", help="The application version (as a string)")
	parser.add_argument("--gif", help="The gif icon")

	# Not used - to be compatible with python3 ledgerblue
	parser.add_argument("--icon", help="The icon content to use (hex encoded)")
	parser.add_argument("--targetVersion", help="Set the chip target version")
	parser.add_argument("--path_slip21", help="""A SLIP 21 path to which derivation is locked""", action='append')
	parser.add_argument("--signature", help="A signature of the application (hex encoded)")
	parser.add_argument("--signApp", help="Sign application with provided signPrivateKey", action='store_true')
	parser.add_argument("--bootAddr", help="The application's boot address", type=auto_int)
	parser.add_argument("--rootPrivateKey", help="""The Signer private key used to establish a Secure Channel (otherwise
a random one will be generated)""")
	parser.add_argument("--signPrivateKey", help="Set the private key used to sign the loaded app")
	parser.add_argument("--apdu", help="Display APDU log", action='store_true')
	parser.add_argument("--deployLegacy", help="Use legacy deployment API", action='store_true')
	parser.add_argument("--apilevel", help="Use given API level when interacting with the device", type=auto_int, default=10)
	parser.add_argument("--delete", help="Delete the app with the same name before loading the provided one", action='store_true')
	parser.add_argument("--params", help="Store icon and install parameters in a parameter section before the code", action='store_true')
	parser.add_argument("--tlv", help="Use install parameters for all variable length parameters", action='store_true')
	parser.add_argument("--offline", help="Request to only output application load APDUs into given filename")
	parser.add_argument("--offlineText", help="Request to only output application load APDUs into given filename in text mode", action='store_true')
	parser.add_argument("--installparamsSize", help="The loaded install parameters section size (when parameters are already included within the .hex file.", type=auto_int)
	parser.add_argument("--tlvraw", help="Add a custom install param with the hextag:hexvalue encoding", action='append')
	parser.add_argument("--dep", help="Add a dependency over an appname[:appversion]", action='append')
	parser.add_argument("--nocrc", help="Skip CRC generation when loading", action='store_true')

	return parser

def main():
	args = get_argparser().parse_args()
	manifest = {}
	manifest["name"] = args.appName
	if args.appVersion: 
		manifest["version"] = args.appVersion

	if args.targetId:
		target = hex(args.targetId)
		manifest[target] = {}
		manifest[target]["binary"] = "../" + args.fileName
		if args.gif:
			manifest[target]["icon"] = args.gif
		if args.appFlags:
			manifest[target]["flags"] = args.appFlags
		manifest[target]["derivationPath"] = {}
		if args.curve:
			manifest[target]["derivationPath"]["curves"] = args.curve
		if args.path:
			manifest[target]["derivationPath"]["paths"] = args.path
		if args.apiLevel:
			manifest[target]["apiLevel"] = str(args.apiLevel)
		if args.dataSize:
			manifest[target]["dataSize"] = args.dataSize
	
	with open(args.output, 'wb') as f:
		tomli_w.dump(manifest, f)

if __name__ == "__main__":
    main()
