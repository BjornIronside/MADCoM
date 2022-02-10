ECHO OFF

::gencarp
ECHO gencarp
ECHO gencarp >> log.txt

ECHO %DATE% %TIME% - 1A Run 1/2
ECHO %DATE% %TIME% - 1A Run 1/2 >> log.txt
gencarp "../Instances/CARP/1A.dat" -type 30 -t 3
ECHO %DATE% %TIME% - 1A Run 2/2
ECHO %DATE% %TIME% - 1A Run 2/2 >> log.txt
gencarp "../Instances/CARP/1A.dat" -type 30 -t 3
ECHO %DATE% %TIME% - C01 Run 1/2
ECHO %DATE% %TIME% - C01 Run 1/2 >> log.txt
gencarp "../Instances/CARP/C01.dat" -type 30 -t 3
ECHO %DATE% %TIME% - C01 Run 2/2
ECHO %DATE% %TIME% - C01 Run 2/2 >> log.txt
gencarp "../Instances/CARP/C01.dat" -type 30 -t 3

::madcom
ECHO madcom
ECHO madcom >> log.txt

ECHO %DATE% %TIME% - 1A Run 1/2
ECHO %DATE% %TIME% - 1A Run 1/2 >> log.txt
madcom "../Instances/CARP/1A.dat" -type 30 -t 3
ECHO %DATE% %TIME% - 1A Run 2/2
ECHO %DATE% %TIME% - 1A Run 2/2 >> log.txt
madcom "../Instances/CARP/1A.dat" -type 30 -t 3
ECHO %DATE% %TIME% - C01 Run 1/2
ECHO %DATE% %TIME% - C01 Run 1/2 >> log.txt
madcom "../Instances/CARP/C01.dat" -type 30 -t 3
ECHO %DATE% %TIME% - C01 Run 2/2
ECHO %DATE% %TIME% - C01 Run 2/2 >> log.txt
madcom "../Instances/CARP/C01.dat" -type 30 -t 3

