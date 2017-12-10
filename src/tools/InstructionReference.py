import requests
from lxml.html import fromstring
from collections import namedtuple
import pyperclip,re
import pandas as pd


def get_inst_ref():
    url = 'http://obelisk.me.uk/6502/reference.html'
    ref = fromstring(requests.get(url).text)
    # 获取指令名，指令表
    opName = [name.attrib['name'] for name in ref.cssselect('h3 a')]
    opTable = ref.cssselect('table')[2::2]
    opDescription = ref.cssselect('h3')
    opDetail = namedtuple('opDetail', ['name', 'addressingMode', 'code', 'bytes', 'cycles', 'extraCycles', 'description'])
    opDetailTable = []
    for (name, table, description) in zip(opName, opTable, opDescription):
        des = []
        while True:
            content = description.text_content().strip()
            if content == '' or 'Processor Status after use:' in content:
                break
            des.append(content)
            description = description.getnext()

        for detail in table.cssselect('tr')[1:]:
            td = detail.cssselect('td')
            d = opDetail(name, td[0].text_content().strip(),
                         td[1].text_content().strip(),
                         td[2].text_content().strip(),
                         td[3].text_content().strip(),
                         '0', des)

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
    df['cycles'] = df['cycles'].str.replace('-', '0');

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
        prs += '\t{{ {0}, OpAddressingMode::{1:>13}, {2}, {3}, CPU::OP_{4} }}, \n'.format(
            row['code'], row['addressingMode'],
            row['bytes'], row['cycles'],
            row['name'])
    prs = prs[:-3] + '\n};\n'
    print(prs)
    pyperclip.copy(prs)


def print_op_exec_func_decl(df):
    # 按指令实现难易程度排序
    df_sorted = df.sort_values(by=['bytes', 'cycles', 'extraCycles']).drop_duplicates(subset='name')
    prs = '/**************** 指令声明区Begin ****************/\n'
    for (_, row) in df_sorted.iterrows():
        prs += 'OpExeFuncDecl(OP_{}); // {}\n'.format(row['name'], row['description'][0])
    prs += '/****************  指令声明区End  ****************/\n'
    print(prs)
    print(len(df_sorted))
    pyperclip.copy(prs)

def print_op_exec_func_define(df):
    # 按指令实现难易程度排序
    df_sorted = df.sort_values(by=['bytes', 'cycles', 'extraCycles']).drop_duplicates(subset='name')
    prs = '参考手册： http://obelisk.me.uk/6502/reference.html\n' \
          '/**************** 指令实现区Begin ****************/\n'
    for (_, row) in df_sorted.iterrows():
        d = row['description'].copy()
        d[1:] = [d.replace('\n', '\n\t * ') for d in d[1:]]
        if len(d) < 3:
            d.append('')
        else:
            d[2] = '\t * {}\n'.format(d[2])

        prs += 'OpExeFuncDefine(OP_{}) {{\n' \
               '\t// TODO: wait for implements: {}\n' \
               '\t/**\n' \
               '\t * {}\n' \
               '\t * {}\n' \
               '{}' \
               '\t **/\n' \
               '\n\treturn self.cycles;\n' \
               '}}\n\n'.format(row['name'], row['name'], *d)

    prs += '/****************  指令实现区End  ****************/\n'
    print(prs)
    print(len(df_sorted))
    pyperclip.copy(prs)

def undocumented_op_table():
    txt = requests.get('http://nesdev.com/undocumented_opcodes.txt').text
    opName = []
    for m in re.finditer('(\w+) \(\w+\) \[\w+\]\s=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D\n', txt):
        opName.append(m.group(1))

    opDescription = []
    for m in re.finditer('=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D=3D\s(.*?)\n\n', txt, re.DOTALL):
        opDescription.append(m.group(1))

    opMap = dict(zip(opName, opDescription))
    opTable = []
    for m in re.finditer(
            '(?P<addressingMode>[ \w()#$,]+)\s*\|(?P<name>\w+)\s.*?\|(?P<code>\$\w{2})\|\s(?P<bytes>\d)\s\|\s(?P<cycles>[\d-])',
            txt):
        d = m.groupdict()
        d['description'] = [d['name'], opMap[d['name']]]
        opTable.append(d)

    return pd.DataFrame(opTable)


if __name__ == '__main__':
    op_df = clean_inst_table(get_inst_ref())
    undoc_df = clean_inst_table(undocumented_op_table())
    # print_inst_table(op_df)
    print_inst_table(undoc_df)
    # print_op_exec_func_decl(op_df)
    # print_op_exec_func_decl(undoc_df)
    # print_op_exec_func_define(op_df)
    print_op_exec_func_define(undoc_df)
