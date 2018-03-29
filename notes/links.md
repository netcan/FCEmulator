## Description
- 综述：[http://web.textfiles.com/games/nestech.txt](http://web.textfiles.com/games/nestech.txt)

## Cpu 6502
- 指令手册：[http://obelisk.me.uk/6502](http://obelisk.me.uk/6502)
- 启动状态：[http://wiki.nesdev.com/w/index.php/CPU_power_up_state](http://wiki.nesdev.com/w/index.php/CPU_power_up_state)
- 指令细节：[https://nesdev.com/6502_cpu.txt](https://nesdev.com/6502_cpu.txt)

## PPU
手册：
- [https://wiki.nesdev.com/w/index.php/PPU_programmer_reference](https://wiki.nesdev.com/w/index.php/PPU_programmer_reference)
- [https://n3s.io/index.php?title=How_It_Works](https://n3s.io/index.php?title=How_It_Works)
- [https://wiki.nesdev.com/w/index.php/PPU_scrolling](https://wiki.nesdev.com/w/index.php/PPU_scrolling)
- [https://wiki.nesdev.com/w/index.php/PPU_rendering](https://wiki.nesdev.com/w/index.php/PPU_rendering)
- [https://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png](https://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png)
- [https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation](https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation)

## Controller
手柄输入：
- [https://wiki.nesdev.com/w/index.php/Controller_port_registers](https://wiki.nesdev.com/w/index.php/Controller_port_registers)
- [https://wiki.nesdev.com/w/index.php/Standard_controller](https://wiki.nesdev.com/w/index.php/Standard_controller)


## Developing
- gtest框架：[https://github.com/google/googletest/blob/master/googletest/docs/Primer.md](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md)
- Nes测试rom：[http://www.qmtpro.com/~nes/misc/nestest.txt](http://www.qmtpro.com/~nes/misc/nestest.txt)
- Nes文件格式：[https://wiki.nesdev.com/w/index.php/INES](https://wiki.nesdev.com/w/index.php/INES)
- 迭代器：[http://www.cplusplus.com/reference/iterator/iterator/](http://www.cplusplus.com/reference/iterator/iterator/)
- 编译器：[http://cc65.github.io/doc/nes.html](http://cc65.github.io/doc/nes.html)
- 6502汇编：
	- [http://nesdev.com/6502guid.txt](http://nesdev.com/6502guid.txt)
	- [https://en.wikibooks.org/wiki/6502_Assembly](https://en.wikibooks.org/wiki/6502_Assembly)

- Overflow与Carry位：[http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt](http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt)
- Undocumented opcodes:
	- [http://nesdev.com/undocumented_opcodes.txt](http://nesdev.com/undocumented_opcodes.txt)
	- [http://nesdev.com/extra_instructions.txt](http://nesdev.com/extra_instructions.txt)

- const与非const函数代码复用：[https://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func](https://stackoverflow.com/questions/123758/how-do-i-remove-code-duplication-between-similar-const-and-non-const-member-func)

## Bug
- PLP的Brk位：
	- [https://forums.nesdev.com/viewtopic.php?f=3&t=13343](https://forums.nesdev.com/viewtopic.php?f=3&t=13343)
	- [http://visual6502.org/wiki/index.php?title=6502_BRK_and_B_bit](http://visual6502.org/wiki/index.php?title=6502_BRK_and_B_bit)

- TXS的N位：Note that TXS (Transfer X to S) is not an arithmetic operation
	- [https://nesdev.com/6502_cpu.txt](https://nesdev.com/6502_cpu.txt)
