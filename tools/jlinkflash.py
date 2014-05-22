#!/usr/bin/env python

import os,sys,pexpect

loadfile=os.popen('ls *.bin').read().rstrip('\n')

child = pexpect.spawn('JLinkExe')
child.logfile = sys.stdout
child.expect('J-Link>')
child.sendline('si 1')  # switch to SWD
child.expect('J-Link>')
child.sendline('speed 4000')
child.expect('J-Link>')
child.sendline('device = STM32F103RB')
child.expect('J-Link>')
child.sendline('h')
child.expect('J-Link>')
child.sendline('r')
child.expect('J-Link>')
child.sendline('loadbin '+loadfile+' 0x08000000')
child.expect('J-Link>')
child.sendline('g')
child.expect('J-Link>')

#child.interact()     # give control of the child to the user.

child.close()
