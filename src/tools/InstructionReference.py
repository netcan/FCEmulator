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

def clean_inst_table(opDetailTable):
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
    # df['name'] = df['name'].str.replace('^\w+$', lambda s: 'OpName::' + s.group(0))
    # df['addressingMode'] = df['addressingMode'].str.replace('^\w+$', lambda s: 'OpAddressingMode::' + s.group(0))
    return df

def print_inst_table(df):
    prs = '{\n'
    for (_, row) in df.iterrows():
        prs += '\t{{ {0}, OpAddressingMode::{1:>13}, {2}, {3}, {4}, CPU::OP_{5} }}, \n'.format(
            row['code'], row['addressingMode'],
            row['bytes'], row['cycles'],
            row['extraCycles'], row['name'])
    prs = prs[:-3] + '\n};\n'
    print(prs)
    pyperclip.copy(prs)


def print_op_exec_func_decl(df):
    # 按指令实现难易程度排序
    df_sorted = df.sort_values(by=['bytes', 'cycles', 'extraCycles']).drop_duplicates(subset='name')
    prs = '/**************** 指令声明区Begin ****************/\n'
    for (_, row) in df_sorted.iterrows():
        prs += 'OpExeFuncDecl(OP_{});\n'.format(row['name'])
    prs += '/****************  指令声明区End  ****************/\n'
    print(prs)
    print(len(df_sorted))
    pyperclip.copy(prs)

def print_op_exec_func_define(df):
    # 按指令实现难易程度排序
    df_sorted = df.sort_values(by=['bytes', 'cycles', 'extraCycles']).drop_duplicates(subset='name')
    prs = '/**************** 指令实现区Begin ****************/\n'
    for (_, row) in df_sorted.iterrows():
        prs += 'OpExeFuncDefine(OP_{}) {{\n'.format(row['name']) + \
               '\t// TODO: wait for implements: {}\n\n'.format(row['name']) +\
                '\treturn self.cycles;\n' + \
               '}\n\n'
    prs += '/****************  指令实现区End  ****************/\n'
    print(prs)
    print(len(df_sorted))
    pyperclip.copy(prs)


if __name__ == '__main__':
    df = clean_inst_table(get_inst_ref())
    print_inst_table(df)
    # print_op_exec_func_decl(df)
    # print_op_exec_func_define(df)
