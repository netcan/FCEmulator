import requests
from lxml.html import fromstring
from collections import namedtuple
import pyperclip
import pandas as pd


def get_inst_ref():
    url = 'http://obelisk.me.uk/6502/reference.html'
    ref = fromstring(requests.get(url).text)
    # 获取指令名，指令表
    opName = [name.attrib['name'] for name in ref.cssselect('h3 a')]
    opTable = ref.cssselect('table')[2::2]
    opDetail = namedtuple('opDetail', ['name', 'addressingMode', 'code', 'bytes', 'cycles', 'extraCycles'])
    opDetailTable = []
    for (name, table) in zip(opName, opTable):
        for detail in table.cssselect('tr')[1:]:
            td = detail.cssselect('td')
            d = opDetail(name, td[0].text_content().strip(),
                         td[1].text_content().strip(),
                         td[2].text_content().strip(),
                         td[3].text_content().strip(),
                         '0')
            opDetailTable.append(d)

    return opDetailTable

def process_inst_table(opDetailTable):
    df = pd.DataFrame(opDetailTable)
    df['addressingMode'] = df['addressingMode'].str.replace('Implied', 'Implicit'
                                        ).str.replace('Zero\s+Page', 'ZeroPage'
                                        ).str.replace('ZeroPage,X', 'ZeroPageX'
                                        ).str.replace('ZeroPage,Y', 'ZeroPageY'
                                        ).str.replace('Absolute,X', 'AbsoluteX'
                                        ).str.replace('Absolute,Y', 'AbsoluteY'
                                        ).str.replace('\(Indirect,X\)', 'IndexIndirect'
                                        ).str.replace('\(Indirect\),Y', 'IndirectIndex')

    df['code'] = df['code'].str.replace('$', '0x')
    df.loc[df['cycles'].str.contains('\+1'), 'extraCycles'] = 1
    df.loc[df['cycles'].str.contains('\+2'), 'extraCycles'] = 2
    df['cycles'] = df['cycles'].str.replace('[\n\s]', ''
                                ).str.replace('(\d+).*', lambda s: s.group(1))
    # 加前缀
    df['name'] = df['name'].str.replace('^\w+$', lambda s: 'OpName::' + s.group(0))
    df['addressingMode'] = df['addressingMode'].str.replace('^\w+$', lambda s: 'OpAddressingMode::' + s.group(0))
    return df

def print_inst_table(df):
    prs = '{\n'
    for (_, row) in df.iterrows():
        prs += '\t{{ {}, {}, {}, {}, {}, nullptr }}, \n'.format(
            row['code'], row['addressingMode'],
            row['bytes'], row['cycles'],
            row['extraCycles'])
    prs = prs[:-3] + '\n};\n'
    print(prs)
    pyperclip.copy(prs)


if __name__ == '__main__':
    df = process_inst_table(get_inst_ref())
    print_inst_table(df)
