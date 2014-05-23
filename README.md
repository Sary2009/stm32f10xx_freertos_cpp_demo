stm32f10xx_freertos_cpp_demo
============================

stm32f10xx freertos with cli plugin demo, support backspace, ctrl+c and so on.

This demo can let you interact with stm32 by using serial command line.

Support backspace key in command line.

Support ctrl+c to cancel command.

Support ctrl+l to clean terminal screen.

type "help" see all command help information

>help

>help:

> Lists all the registered commands
>
>gettmpr:
> show stm32's temperature 
>
>date:
> show date
>
>reset:
> reset system
>
>hello:
> print Hello world!
>
>led <0|1> <on|off>:
> turn on or off led0|1
>
>clear:
> clear screen

Now is your turn to make your own commands in job.cpp
