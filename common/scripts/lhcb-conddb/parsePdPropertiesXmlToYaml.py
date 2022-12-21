#!/usr/bin/env python3

'''
Create conditions for PD properties in YAML (lhcb-conddb) parsing the XML (SIMCOND) version.

Needs RICH environment set.

Based on the code in: https://gitlab.cern.ch/lhcb/testscope/-/blob/master/scripts/convert_cond_to_yml.py.
'''

import sys
import os

from parse import parse as parseString

from importlib.machinery import SourceFileLoader
smartIdHelper = SourceFileLoader('smartId', os.path.join(os.environ['RICH_BASE_SCRIPTS_UTILS'], 'smartId.py')).load_module()

# config
inputPathBase = os.path.join(os.environ['RICH_BASE_GITCONDDB'], 'SIMCOND')
outputPathBase = "output"

condNameGlobal = "PDProperties"

headerPdProperties = """\
# QEType            : Flag QE shape for the given PD type/series (dummy value for now).
# QEScalingFactor   : QE scaling factor for the given PD (dummy value for now).
# GainMean          : Gain mean values for PD channels [in millions of electrons].
# GainRms           : Gain RMS values for PD channels [in millions of electrons].
# Threshold         : Threshold values for PD channels [in millions of electrons].
# AverageOccupancy  : Average PD occupancy [probability in range 0-1]. This is the MB occupancy simulated for nu=7.6 with Gauss/v55r4 after the numbering scheme updates in https://gitlab.cern.ch/lhcb-conddb/DDDB/-/merge_requests/107.
# SINRatio          : SIN B/S ratio for PD channels (values measured with HV = 900 V).

"""

listToParse = [
    {'in': os.path.join(inputPathBase, 'Conditions/Rich1/ChannelInfo/PMTProperties.xml'), 'out': os.path.join(outputPathBase, 'Conditions/Rich1/HardwareProperties/PDProperties.yml/0'), 'header' : headerPdProperties},
    {'in': os.path.join(inputPathBase, 'Conditions/Rich2/ChannelInfo/PMTProperties.xml'), 'out': os.path.join(outputPathBase, 'Conditions/Rich2/HardwareProperties/PDProperties.yml/0'), 'header' : headerPdProperties}
]

# functions
def fileOpen(filePath):
    myFile = open(filePath, "w")
    return myFile


def fileClose(myFile):
    myFile.close()


def to_dict(xml_file):
    from xml.etree.ElementTree import parse
    tree = parse(xml_file)
    out = {}
    for cond in tree.iter('condition'):
        name = cond.get('name')

        # TODO improve (reg expressions?)
        idPattern = 'PMT{}_Properties'
        parsedId = parseString(idPattern, name)

        if parsedId:
            copyNumber = int(parsedId[0])

            # ! account for the RICH2 column numbering scheme difference (12->14 columns between DetDesc/DD4HEP)
            name = f'{smartIdHelper.SmartId.initFromCopyNumber(copyNumber).stringFormattedInDd4hepScheme()}'     

        out[name] = {}
        for el in list(cond):
            value = el.text.strip()
            # ! read values as strings (keep same formatting as in XML)
            if el.tag.lower().endswith('vector'):
                value = value.split()
                if el.get('type') == 'double':
                    value = f'[{", ".join(str(v) for v in value)}]'
                elif el.get('type') == 'int':
                    value = f'[{", ".join(str(v) for v in value)}]'
            else:
                if el.get('type') == 'double':
                    value = str(value)
                elif el.get('type') == 'int':
                    value = str(value)
            out[name][el.get('name')] = value
    return out


def to_yaml(mapping):
    subkey_map = {
    }
    for key in mapping:
        # ! adjust indent level
        yield "  {}:".format(key)
        for subkey in mapping[key]:
            yield "    {}: {}".format(
                subkey_map.get(subkey, subkey), mapping[key][subkey])

def xml_to_yaml(input, output):
    
    # get dictionary
    conditionsDict = to_dict(input)

    # dump to YAML
    # import yaml
    # yaml.dump(conditionsDict, output, allow_unicode=True, sort_keys=False)

    # custom YAML
    for l in to_yaml(conditionsDict):
        output.write(f"{l}\n")

# run
if __name__ == "__main__":

    # check if RICH environment is set (needs a known host)
    if not 'RICH_HOST' in os.environ:
        print('\nRICH environment not set. Exitting.\n')
        sys.exit(1)

    # prepare output directory
    if os.path.isdir(outputPathBase):
        print('\nOutput directory already exists. Exitting.\n')
        sys.exit(1)
    else:
        os.mkdir(outputPathBase)
    
    # run
    print('\n')
    for curParse in listToParse:
        
        print(f"Parsing:\n- from: {curParse['in']}\n- to: {curParse['out']}\n")

        os.makedirs(os.path.dirname(curParse['out']), exist_ok=True)
        outputFile = fileOpen(curParse['out'])
        outputFile.write(f"{curParse['header']}")

        # ! add global condition name if required
        outputFile.write(f"{condNameGlobal}:\n")

        xml_to_yaml(curParse['in'], outputFile)
        
        fileClose(outputFile)
